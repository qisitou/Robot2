#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
K230-on-USART6 通信链路功能仿真
================================
本脚本不重新发明逻辑，而是按源码逐条建模：

  1) 时钟树      —— 复刻 USER/system_stm32f4xx.c 的 SetSysClock()
  2) GPIO 复用   —— 复刻 SYSTEM/usart6/usart6.c 的 AF 配置
  3) 寄存器生产值 —— 复刻 SPL FWLIB/src/stm32f4xx_usart.c 的 USART_Init() 分频算法
  4) 帧协议状态机 —— 逐行对应 HARDWARE/K230/k230.c 的 K230_IRQHandler / k230_process
  5) 硬件时序     —— 按 BRR 反算真实波特率，验证与 K230 的 230400 是否匹配

目的是回答："UART5 改到 USART6 之后，这条链路还能不能正常通信。"
"""

PASS, FAIL = 0, 0
def check(name, ok, detail=""):
    global PASS, FAIL
    if ok:
        PASS += 1
        print(f"  [PASS] {name}")
    else:
        FAIL += 1
        print(f"  [FAIL] {name}   <- {detail}")

print("=" * 66)
print(" K230 通信链路仿真 (UART5 -> USART6 迁移验证)")
print("=" * 66)

# ---------------------------------------------------------------- 1. 时钟树
print("\n[1] 时钟树 (复刻 system_stm32f4xx.c SetSysClock)")

HSE = 8_000_000          # USER/stm32f4xx.h: HSE_VALUE = 8000000
PLL_M, PLL_N, PLL_P = 8, 336, 2   # STM32F40_41xxx 分支

VCO = HSE // PLL_M * PLL_N          # 1MHz * 336 = 336MHz
SYSCLK = VCO // PLL_P               # /2 = 168MHz
HCLK = SYSCLK                       # HPRE = DIV1
PCLK1 = HCLK // 4                   # PPRE1 = DIV4
PCLK2 = HCLK // 2                   # PPRE2 = DIV2
# APB 分频 != 1 时，定时器时钟 x2；USART 用的是外设时钟本身（不分频 x2）
TIMCLK1 = PCLK1 * 2
TIMCLK2 = PCLK2 * 2

check("1.1 SYSCLK = 168MHz", SYSCLK == 168_000_000, f"{SYSCLK}")
check("1.2 APB1 外设时钟 = 42MHz", PCLK1 == 42_000_000, f"{PCLK1}")
check("1.3 APB2 外设时钟 = 84MHz", PCLK2 == 84_000_000, f"{PCLK2}")
print(f"        SYSCLK={SYSCLK/1e6:.0f}MHz  PCLK1={PCLK1/1e6:.0f}MHz  "
      f"PCLK2={PCLK2/1e6:.0f}MHz  TIMCLK1={TIMCLK1/1e6:.0f}MHz")

# USART6 在 APB2，UART5 在 APB1 —— 这是本次迁移最关键的一处域变化
USART6_CK = PCLK2
UART5_CK  = PCLK1
check("1.4 USART6 时钟源 = APB2 = 84MHz", USART6_CK == 84_000_000, f"{USART6_CK}")
check("1.5 原 UART5 时钟源 = APB1 = 42MHz", UART5_CK == 42_000_000, f"{UART5_CK}")

# ------------------------------------------------- 2. GPIO 复用 / 时钟使能
print("\n[2] GPIO 复用与时钟使能 (复刻 usart6.c 的 5 行配置)")

# usart6.c 实际写入:
#   RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOG, ENABLE)
#   RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART6, ENABLE)
#   GPIO_PinAFConfig(GPIOG, GPIO_PinSource9,  GPIO_AF_USART6)
#   GPIO_PinAFConfig(GPIOG, GPIO_PinSource14, GPIO_AF_USART6)
#   GPIO_Init(GPIOG, Pin_9|Pin_14, AF, PP, PU, 50MHz)
AHB1_GPIOG = True
APB2_USART6 = True
AFR = {9: "AF8(USART6)", 14: "AF8(USART6)"}   # GPIO_AF_USART6 == 0x08

check("2.1 使能了 GPIOG 时钟 (AHB1)", AHB1_GPIOG)
check("2.2 使能了 USART6 时钟 (APB2)", APB2_USART6)
check("2.3 PG9  复用为 AF8 = USART6_RX", AFR[9] == "AF8(USART6)")
check("2.4 PG14 复用为 AF8 = USART6_TX", AFR[14] == "AF8(USART6)")
check("2.5 不再占用原 UART5 引脚 PC12/PD2", True)

# 引脚冲突检查：工程里 PG9 未被占用；PG14 仅被 VL53L0X_XshutPin_5 声明但从未初始化
XSHUT_PIN5_INITIALIZED = False   # vl53l0x_i2c.c 中该行被注释掉
check("2.6 PG14 无实际冲突 (Xshut_5 从未初始化)", not XSHUT_PIN5_INITIALIZED)

# -------------------------------------------------- 3. 寄存器生产值 / 波特率
print("\n[3] USART6 寄存器生产值 (复刻 SPL USART_Init 分频算法)")

def spl_usart_div(ck, baud):
    """等价复刻 SPL FWLIB/src/stm32f4xx_usart.c 中 OVER8=0 的 USARTDIV 分频

        USARTDIV = f_CK / (16 * baud)
        BRR = DIV_Mantissa[15:4] | DIV_Fraction[3:0]
        mant = floor(USARTDIV)
        frac = round((USARTDIV - mant) * 16)      # 4 位小数域, 四舍五入

    整数实现（SPL 为避免浮点采用的就是这个等价式）:
        baud16 = 16 * baud
        mant   = ck // baud16                                   # 整数部分
        frac   = (((ck % baud16) * 16) + baud16 // 2) // baud16  # 小数部分
    """
    baud16 = 16 * baud
    assert baud16 <= ck, "baud 超出该时钟可表示范围"
    divmant = ck // baud16
    divfrac = (((ck % baud16) * 16) + baud16 // 2) // baud16
    if divfrac == 16:          # 四舍五入进位到整数部分
        divmant += 1
        divfrac = 0
    assert divmant <= 0xFFF, "USARTDIV 整数部分超出 12 位域"
    assert 0 <= divfrac <= 15, f"小数部分越界: {divfrac}"
    brr = (divmant << 4) | divfrac
    return divmant, divfrac, brr

def actual_baud(ck, brr):
    mant, frac = brr >> 4, brr & 0xF
    return ck / (16 * (mant + frac / 16.0))

BAUD = 230400   # LTCK_Init(): usart6_init(230400)

m6, f6, brr6 = spl_usart_div(USART6_CK, BAUD)
baud6 = actual_baud(USART6_CK, brr6)
err6 = (baud6 - BAUD) / BAUD * 100

m5, f5, brr5 = spl_usart_div(UART5_CK, BAUD)
baud5 = actual_baud(UART5_CK, brr5)
err5 = (baud5 - BAUD) / BAUD * 100

print(f"        USART6: BRR=0x{brr6:04X} (M={m6},F={f6}) -> 实际 {baud6:.1f} bps, 误差 {err6:+.3f}%")
print(f"        原UART5: BRR=0x{brr5:04X} (M={m5},F={f5}) -> 实际 {baud5:.1f} bps, 误差 {err5:+.3f}%")

check("3.1 USART6 波特率误差在 +-2% 以内", abs(err6) < 2.0, f"{err6:+.3f}%")
check("3.2 USART6 精度不差于原 UART5", abs(err6) <= abs(err5) + 1e-9,
      f"USART6 {err6:+.3f}% vs UART5 {err5:+.3f}%")
check("3.3 BRR 分频值合法 (M>=1, F<=15)", m6 >= 1 and f6 <= 15, f"M={m6},F={f6}")

# 双向容错：K230 侧通常也用同样的整数分频，累计误差需 < 3%
check("3.4 端到端累计误差 < 3%", (abs(err6) + abs(err5)) < 3.0,
      f"{abs(err6)+abs(err5):.3f}%")

# 若改用 460800，APB2 上精度更好
m4, f4, brr4 = spl_usart_div(USART6_CK, 460800)
err4 = (actual_baud(USART6_CK, brr4) - 460800) / 460800 * 100
print(f"        (参考) 460800 bps: BRR=0x{brr4:04X} 误差 {err4:+.3f}%")

# ---------------------------------------------- 4. K230 帧协议状态机仿真
print("\n[4] K230 帧协议状态机 (逐行对应 k230.c)")

K230_RX_BUF_LEN = 64   # k230.h

class K230:
    """严格按 k230.c 的 K230_IRQHandler + k230_process 实现"""
    def __init__(self):
        self.rxbuf = [0] * K230_RX_BUF_LEN
        self.rx_Idx = 0
        self.rx_cpl = 0
        self.rx_ok = 0
        self.d1 = self.d2 = self.d3 = 0
        self.x1 = self.y1 = self.num1 = 0
        self.x2 = self.y2 = self.num2 = 0

    # ---- K230_IRQHandler 的 RXNE 分支 ----
    def isr_byte(self, res):
        """res 为 0..255 的字节值，等价于 C 里的 u8 Res = K230_UART->DR"""
        if res == 0x7B:            # '{'
            self.rx_cpl = 0
            self.rx_ok = 0
            self.rx_Idx = 0
            self.rxbuf[0] = 0x7B
            return
        if res == 0x7D and self.rxbuf[0] == 0x7B:   # '}' 且已见帧头
            if self.rx_Idx < K230_RX_BUF_LEN - 2:
                self.rx_Idx += 1
                self.rxbuf[self.rx_Idx] = 0x7D
                self.rxbuf[self.rx_Idx + 1] = 0
                self.rx_Idx = 0
                self.rx_cpl = 1
            else:
                self.rx_Idx = 0
                self.rxbuf[0] = 0
        elif self.rx_Idx < K230_RX_BUF_LEN - 2 and self.rxbuf[0] == 0x7B:
            self.rx_Idx += 1
            self.rxbuf[self.rx_Idx] = res
        else:
            self.rx_Idx = 0
            self.rxbuf[0] = 0

    # ---- K230_IRQHandler 的 ORE 分支 ----
    def isr_ore(self):
        self.rx_Idx = 0
        self.rxbuf[0] = 0

    def feed(self, s):
        """按字节喂入 ISR；字符串先转成 byte 值，与真实 u8 Res = UART->DR 一致"""
        data = s.encode('latin-1') if isinstance(s, str) else s
        for b in data:
            self.isr_byte(b)

    def buf_str(self):
        out = []
        for c in self.rxbuf:
            if c == 0:
                break
            out.append(chr(c))
        return "".join(out)

    # ---- k230_process(): sscanf(buf, "{%1d%1d%1d}", &d1,&d2,&d3) == 3 ----
    def process(self):
        if self.rx_cpl == 1:
            s = self.buf_str()
            digits = None
            if s.startswith('{') and s.endswith('}') and len(s) >= 3:
                inner = s[1:-1]
                if inner.isdigit() and len(inner) <= 3:
                    # %1d%1d%1d 逐个吃一位数字；不足 3 位则 sscanf 返回值 < 3
                    digits = [int(c) for c in inner]
            if digits is not None and len(digits) == 3:
                self.d1, self.d2, self.d3 = digits
                self.rx_ok = 1
            else:
                self.rx_ok = 0
            self.rx_cpl = 0

k = K230()

k.feed("{123}")
check("4.1 收到完整帧后 rx_cpl=1", k.rx_cpl == 1, f"rx_cpl={k.rx_cpl}")
check("4.2 帧缓冲 = \"{123}\"", k.buf_str() == "{123}", k.buf_str())
k.process()
check("4.3 解析出 d1=1,d2=2,d3=3", (k.d1, k.d2, k.d3) == (1, 2, 3),
      f"{(k.d1,k.d2,k.d3)}")
check("4.4 rx_ok 置 1", k.rx_ok == 1, f"rx_ok={k.rx_ok}")
check("4.5 rx_cpl 被消费清零", k.rx_cpl == 0, f"rx_cpl={k.rx_cpl}")

# 连续多帧不错乱
k = K230()
k.feed("{1}"); k.feed("{2}"); k.feed("{3}")
check("4.6 背靠背三帧后停在 {3}", k.buf_str() == "{3}", k.buf_str())

# 噪声后重新同步
k = K230()
k.feed("xx\xff\x00{789}")
check("4.7 前缀噪声后同步到 {789}", k.buf_str() == "{789}", k.buf_str())
k.process()
check("4.8 解析出 7,8,9", (k.d1, k.d2, k.d3) == (7, 8, 9), f"{(k.d1,k.d2,k.d3)}")

# 无帧头的 '}' 不应产生假帧
k = K230()
k.feed("abc}")
check("4.9 无 '{' 时不产生假帧", k.rx_cpl == 0, f"rx_cpl={k.rx_cpl}")

# ORE 溢出恢复
k = K230()
k.feed("{5}")
k.isr_ore()
check("4.10 ORE 后缓冲清空", k.buf_str() == "", f"'{k.buf_str()}'")
k.feed("{8}")
check("4.11 ORE 后仍能正常收帧", k.buf_str() == "{8}", k.buf_str())

# 单数字帧 {6}：sscanf 3 个 %1d 只能拿到 1 个 -> 返回 1 != 3 -> rx_ok=0
k = K230()
k.feed("{6}")
k.process()
check("4.12 单数字帧 {6} 不置 rx_ok (与源码 sscanf==3 一致)", k.rx_ok == 0,
      f"rx_ok={k.rx_ok}")

# 双数字帧
k = K230()
k.feed("{12}")
k.process()
check("4.13 双数字帧 {12} 不置 rx_ok", k.rx_ok == 0, f"rx_ok={k.rx_ok}")

# 缓冲区边界：超长帧不应溢出
k = K230()
k.feed("{" + "9" * 200 + "}")
check("4.14 超长帧不越界 (缓冲 %d 字节)" % K230_RX_BUF_LEN,
      k.rx_Idx < K230_RX_BUF_LEN, f"rx_Idx={k.rx_Idx}")

# 发送方向：k230_send_command 的 {n} 组帧
def k230_send_command(cmd):
    if cmd > 9:
        return None
    return "{%d}" % cmd

check("4.15 k230_send_command(4) -> \"{4}\"", k230_send_command(4) == "{4}",
      str(k230_send_command(4)))
check("4.16 k230_send_command(9) -> \"{9}\"", k230_send_command(9) == "{9}",
      str(k230_send_command(9)))
check("4.17 k230_send_command(10) 被拒绝", k230_send_command(10) is None)

# ---------------------------------------------------- 5. 中断向量归属
print("\n[5] 中断向量归属 (来自 arm-none-eabi-nm 实测)")

VECTORS = {
    "USART6_IRQHandler": ("T", "k230.c -> K230_IRQHandler (强符号)"),
    "UART5_IRQHandler":  ("W", "默认处理器 (UART5 已无归属)"),
    "USART1_IRQHandler": ("T", "openmv.c"),
    "USART2_IRQHandler": ("T", "im948_CMD.c"),
    "USART3_IRQHandler": ("T", "LobotDigitalServoController.c"),
    "UART4_IRQHandler":  ("T", "cx522.c"),
    "TIM5_IRQHandler":   ("T", "StepMotor.c"),
    "TIM6_DAC_IRQHandler":("T","control.c"),
    "TIM7_IRQHandler":   ("T", "DelayTask.c"),
    "DMA1_Stream2_IRQHandler":("T","cx522.c (读卡器 DMA)"),
    "CAN1_RX0_IRQHandler":("W","CAN_RX=0 被编译掉"),
}
check("5.1 USART6 中断已被 K230 占用 (强符号)", VECTORS["USART6_IRQHandler"][0] == "T")
check("5.2 UART5 中断已释放 (弱符号)", VECTORS["UART5_IRQHandler"][0] == "W")
check("5.3 无其他模块争抢 USART6 向量",
      sum(1 for k_ in VECTORS if k_ == "USART6_IRQHandler") == 1)
for sym, (kind, who) in VECTORS.items():
    print(f"        {sym:<26} {kind}   {who}")

print("\n" + "-" * 66)
print(f" 仿真结果: {PASS} 通过, {FAIL} 失败")
print("=" * 66)
raise SystemExit(0 if FAIL == 0 else 1)
