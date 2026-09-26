# -*- coding: utf-8 -*-
'''
Script: deploy_det_video.py
脚本名称：deploy_det_video.py

Description:
    This script runs a real-time object detection application on an embedded device.
    It uses a pipeline to capture video frames, performs inference using a pre-trained Kmodel,
    and displays the detection results (bounding boxes, class labels) on screen.

    The model configuration is loaded from the Canaan online training platform via a JSON config file.

脚本说明：
    本脚本在嵌入式设备上运行实时目标检测应用。它通过捕获视频帧，使用预训练的 Kmodel 进行推理，并在屏幕上显示检测结果（边界框、类别标签）。

    模型配置文件通过 Canaan 在线训练平台从 JSON 文件加载。

Author: Canaan Developer
作者：Canaan 开发者
'''


import os, gc
from libs.PlatTasks import DetectionApp
from libs.PipeLine import PipeLine
from libs.Utils import *
from media.sensor import Sensor
try:
    from media.sensor import CAM_CHN_ID_0
except Exception:
    CAM_CHN_ID_0 = 0
try:
    from media.sensor import CAM_CHN_ID_1
except Exception:
    CAM_CHN_ID_1 = 1
try:
    from media.sensor import CAM_CHN_ID_2
except Exception:
    CAM_CHN_ID_2 = 2
from media.display import Display

# Set display mode: options are 'hdmi', 'lcd', 'lt9611', 'st7701', 'hx8399'
# 'hdmi' defaults to 'lt9611' (1920x1080); 'lcd' defaults to 'st7701' (800x480)
display_mode = "lcd"

# Define the input size for the RGB888P video frames
rgb888p_size = [1280, 720]

# Set root directory path for model and config
root_path = "/sdcard/mp_deployment_source/"

# Load deployment configuration
deploy_conf = read_json(root_path + "/deploy_config.json")
kmodel_path = root_path + deploy_conf["kmodel_path"]              # KModel path
labels = deploy_conf["categories"]                                # Label list
confidence_threshold = deploy_conf["confidence_threshold"]        # Confidence threshold
nms_threshold = deploy_conf["nms_threshold"]                      # NMS threshold
model_input_size = deploy_conf["img_size"]                        # Model input size
nms_option = deploy_conf["nms_option"]                            # NMS strategy
model_type = deploy_conf["model_type"]                            # Detection model type
anchors = []
if model_type == "AnchorBaseDet":
    anchors = deploy_conf["anchors"][0] + deploy_conf["anchors"][1] + deploy_conf["anchors"][2]

# Inference configuration
inference_mode = "video"                                          # Inference mode: 'video'
debug_mode = 0                                                    # Debug mode flag

H_MIRROR = True          # True=水平镜像(左右翻转)
V_FLIP = True            # True=垂直翻转(上下颠倒)


# Create and initialize the video/display pipeline
pl = PipeLine(rgb888p_size=rgb888p_size, display_mode=display_mode)
pl.create(hmirror=H_MIRROR, vflip=V_FLIP)
display_size = pl.get_display_size()

# 颜色任务使用独立的 RGB565 通道，以便直接调用庐山派官方 LAB find_blobs。
# 保持通道2的 RGB888P 配置不变，任务6仍使用原数字模型。
COLOR_IMG_W = 640
COLOR_IMG_H = 360
try:
    # CanMV 要求：停止传感器后，重新配置前必须先 reset()，否则会报
    # AssertionError: should call reset() first。
    pl.sensor.stop()
    pl.sensor.reset()
    try:
        pl.sensor.set_hmirror(H_MIRROR)
        pl.sensor.set_vflip(V_FLIP)
    except Exception:
        pass

    # reset() 会清除之前的通道配置，因此三个通道都要重新配置：
    # 通道0显示、通道1颜色(LAB find_blobs)、通道2数字模型。
    try:
        pl.sensor.set_framesize(width=display_size[0], height=display_size[1],
                                chn=CAM_CHN_ID_0)
    except TypeError:
        pl.sensor.set_framesize(w=display_size[0], h=display_size[1],
                                chn=CAM_CHN_ID_0)
    try:
        pl.sensor.set_pixformat(Sensor.YUV420SP, chn=CAM_CHN_ID_0)
    except TypeError:
        pl.sensor.set_pixformat(Sensor.YUV420SP)

    try:
        pl.sensor.set_framesize(width=COLOR_IMG_W, height=COLOR_IMG_H,
                                chn=CAM_CHN_ID_1)
    except TypeError:
        # 兼容使用 w/h 参数名的旧版 CanMV 固件。
        pl.sensor.set_framesize(w=COLOR_IMG_W, h=COLOR_IMG_H,
                                chn=CAM_CHN_ID_1)
    try:
        pl.sensor.set_pixformat(Sensor.RGB565, chn=CAM_CHN_ID_1)
    except TypeError:
        pl.sensor.set_pixformat(Sensor.RGB565)

    try:
        pl.sensor.set_framesize(width=rgb888p_size[0], height=rgb888p_size[1],
                                chn=CAM_CHN_ID_2)
    except TypeError:
        pl.sensor.set_framesize(w=rgb888p_size[0], h=rgb888p_size[1],
                                chn=CAM_CHN_ID_2)
    try:
        pl.sensor.set_pixformat(Sensor.RGBP888, chn=CAM_CHN_ID_2)
    except TypeError:
        pl.sensor.set_pixformat(Sensor.RGBP888)

    try:
        bind_info = pl.sensor.bind_info(x=0, y=0, chn=CAM_CHN_ID_0)
        Display.bind_layer(**bind_info, layer=Display.LAYER_VIDEO1)
    except Exception:
        pass
    # 颜色追踪时关闭自动增益/自动白平衡，避免同一颜色在相邻帧漂移。
    for _method_name in ("set_auto_gain", "set_auto_whitebal"):
        _method = getattr(pl.sensor, _method_name, None)
        if _method is not None:
            try:
                _method(False)
            except Exception:
                pass
    pl.sensor.run()
except Exception as e:
    print("颜色识别通道初始化失败:", e)
    raise

# Initialize object detection application
det_app = DetectionApp(inference_mode,kmodel_path,labels,model_input_size,anchors,model_type,confidence_threshold,nms_threshold,rgb888p_size,display_size,debug_mode=debug_mode)

# Configure preprocessing for the model
det_app.config_preprocess()


# ===================== 【新增】移植自 OpenMV《机械臂摄像头-立桩拨球.py》 =====================
# 串口: 庐山派 K230 UART2，TX=GPIO11，RX=GPIO12，波特率230400(与OpenMV的UART(3,230400)一致)
# 说明: OpenMV 代码里的 5 个颜色任务(1~5) + 数字识别任务(6) 全部移植到 K230。
#       颜色识别: 采用庐山派官方 RGB565 图像的 find_blobs(LAB阈值) 接口。
#       颜色通道使用独立的 RGB565 图像，数字任务仍使用 RGB888P 图像。
#       上位机协议与原 OpenMV 完全一致，可直接复用原来的主控端程序。
#       数字模型推理只在任务6执行，任务1~5为纯颜色识别，不识别数字，速度更快。
from machine import UART, FPIOA

fpioa = FPIOA()
fpioa.set_function(11, FPIOA.UART2_TXD)   # K230 TX
fpioa.set_function(12, FPIOA.UART2_RXD)   # K230 RX
uart = UART(
    UART.UART2,
    baudrate=230400,
    bits=UART.EIGHTBITS,
    parity=UART.PARITY_NONE,
    stop=UART.STOPBITS_ONE
)

# ---------- 全局状态(与OpenMV同名同义) ----------
Task_flag = 3         # 默认任务6(数字识别)；主控可发 {1}~{6} 切换
is_Mute = 'S'          # 'S'=发送+打印, 'M'=静默
RED_or_BLUE = 'R'      # 任务3发送 {r}/{b} 的依据，任务2/4检测的颜色
threshold_index = 1    # 1=红, 2=蓝

# 官方 find_blobs 参数：已知小球较大时增大步长可提高帧率。
COLOR_X_STRIDE = 2
COLOR_Y_STRIDE = 2

# 恢复串口终端打印；发送 {M} 时按原逻辑同时静默串口和终端。
PRINT_TX = True

# 任务6调试用：识别到数字时每帧打印到串行终端；UART 发送仍保持结果变化才发送。
TASK6_PRINT_EVERY_FRAME = True

# 图像分辨率(取rgb888p, 已16字节对齐) 与显示分辨率
VW, VH = det_app.rgb888p_size[0], det_app.rgb888p_size[1]
DW, DH = det_app.display_size[0], det_app.display_size[1]

# ---------- 颜色阈值(LAB，每通道[lo,hi])，可直接填入庐山派阈值编辑器结果 ----------
COLOR_TH = {
    # 格式：(L_min, L_max, A_min, A_max, B_min, B_max)
    # 以下为原 OpenMV/LAB 参数；现场可用阈值编辑器重新测量后替换。
    'R': (1, 100, 32, 127, -18, 76),
    'B': (1, 100, -20, 16, -128, -20),
}

# 只缩小屏幕显示框，不改变颜色检测结果、中心点和串口数据。
# 0.70 表示显示框宽高为原检测框的 70%。
DISPLAY_BOX_SCALE = 0.70

# ---------- 各任务的检测区域(比例与OpenMV代码一致) ----------
rect_task1  = (int(VW*0.9), int(VH*0),   int(VW*0.1), int(VH*1))     # 任务1: 右侧10%竖条带
rect_task5  = (int(VW*0.5), int(VH*0.4), int(VW*0.5), int(VH*0.6))   # 任务5: 右下半区域

rect_h = [                                                           # 任务2: 水平三条带(上/中/下)
    (0, 0,               VW, int(VH*0.25)),
    (0, int(VH*0.25),    VW, int(VH*0.35)),
    (0, int(VH*0.6),     VW, int(VH*0.4))
]
rect_v = (int(VW*0.35), 0, int(VW*0.3), VH)                          # 任务2: 中间30%竖条带

rect_lz_v = (int(VW*0.3), int(VH*0.2), int(VW*0.2), int(VH*0.5))     # 任务3: 立柱识别区域

rect = [                                                             # 任务4: 3x3九宫格, rect[行][列], 0行=图像上部
    [   (int(VW*0.05),int(VH*0.15),int(VW*0.30),int(VH*0.3)),
        (int(VW*0.42),int(VH*0.15),int(VW*0.25),int(VH*0.25)),
        (int(VW*0.7), int(VH*0.15),int(VW*0.30),int(VH*0.2))
    ],
    [   (int(VW*0.05),int(VH*0.45),int(VW*0.30),int(VH*0.3)),
        (int(VW*0.42),int(VH*0.4), int(VW*0.25),int(VH*0.25)),
        (int(VW*0.7), int(VH*0.35),int(VW*0.30),int(VH*0.25))
    ],
    [   (int(VW*0.05),int(VH*0.75),int(VW*0.30),int(VH*0.25)),
        (int(VW*0.42),int(VH*0.7), int(VW*0.25),int(VH*0.3)),
        (int(VW*0.7), int(VH*0.6), int(VW*0.30),int(VH*0.25))
    ]
]

# ---------- 任务6: 数字识别状态 ----------
uart_buf = ""          # 串口接收缓冲
last_digit_msg = ""    # 上一次发送的任务6结果，用于去重


# 串口发送+打印(与OpenMV的 is_Mute 逻辑一致)
def tx(msg):
    if is_Mute == 'S':
        uart.write(msg.encode())
        if PRINT_TX:
            print(msg)


# 颜色色块检测：使用庐山派官方 RGB565 + LAB find_blobs。
# roi 和返回值均换算为原任务使用的 1280x720 坐标，其他任务逻辑无需改动。
# 返回 (cx,cy,x,y,w,h) 或 None
def find_color_blobs(img, roi, color, pixel_th=15):
    x, y, w, h = roi[0], roi[1], roi[2], roi[3]

    # 将任务坐标中的 ROI 换算到 RGB565 颜色通道。
    rx = max(0, x * COLOR_IMG_W // VW)
    ry = max(0, y * COLOR_IMG_H // VH)
    rw = max(1, w * COLOR_IMG_W // VW)
    rh = max(1, h * COLOR_IMG_H // VH)
    if rx + rw > COLOR_IMG_W:
        rw = COLOR_IMG_W - rx
    if ry + rh > COLOR_IMG_H:
        rh = COLOR_IMG_H - ry

    kwargs = {
        "roi": (rx, ry, rw, rh),
        "x_stride": COLOR_X_STRIDE,
        "y_stride": COLOR_Y_STRIDE,
        "pixels_threshold": int(pixel_th),
        "area_threshold": int(pixel_th),
        "merge": True,
    }
    try:
        blobs = img.find_blobs([COLOR_TH[color]], **kwargs)
    except TypeError:
        # 兼容少数不接受 stride 或 merge 参数的旧版固件。
        kwargs.pop("x_stride", None)
        kwargs.pop("y_stride", None)
        try:
            blobs = img.find_blobs([COLOR_TH[color]], **kwargs)
        except TypeError:
            kwargs.pop("merge", None)
            blobs = img.find_blobs([COLOR_TH[color]], **kwargs)

    # 多个候选色块时取颜色像素最多的一个，减少背景小色块误判。
    if not blobs:
        return None
    best = None
    best_pixels = -1
    for blob in blobs:
        try:
            bx, by, bw, bh = int(blob[0]), int(blob[1]), int(blob[2]), int(blob[3])
            pixels = int(blob[4])
            cx, cy = int(blob[5]), int(blob[6])
        except Exception:
            bx, by, bw, bh = [int(v) for v in blob.rect()]
            pixels = int(blob.pixels())
            cx, cy = int(blob.cx()), int(blob.cy())
        if pixels > best_pixels:
            best_pixels = pixels
            best = (cx, cy, bx, by, bw, bh)

    if best is None:
        return None

    cx, cy, bx, by, bw, bh = best
    return (cx * VW // COLOR_IMG_W, cy * VH // COLOR_IMG_H,
            bx * VW // COLOR_IMG_W, by * VH // COLOR_IMG_H,
            max(1, bw * VW // COLOR_IMG_W),
            max(1, bh * VH // COLOR_IMG_H))


# ---------- 屏幕绘制辅助(rgb888p坐标 -> 显示坐标) ----------
def draw_roi(osd, roi, color=(255, 255, 255, 255), thickness=2):
    rx, ry, rw, rh = roi[0], roi[1], roi[2], roi[3]
    osd.draw_rectangle(rx * DW // VW, ry * DH // VH, rw * DW // VW, rh * DH // VH, color=color, thickness=thickness)


def draw_blob(osd, b, color=(255, 0, 255, 0), label=None):
    cx, cy, bx, by, bw, bh = b[0], b[1], b[2], b[3], b[4], b[5]
    scx, scy = cx * DW // VW, cy * DH // VH

    # 以检测中心为基准缩小显示框；b 本身不变，因此不会影响识别精度。
    box_w = max(2, int(bw * DW / VW * DISPLAY_BOX_SCALE))
    box_h = max(2, int(bh * DH / VH * DISPLAY_BOX_SCALE))
    box_x = scx - box_w // 2
    box_y = scy - box_h // 2
    osd.draw_rectangle(box_x, box_y, box_w, box_h, color=color, thickness=2)
    osd.draw_line(scx - 4, scy, scx + 4, scy, color=color, thickness=2)
    osd.draw_line(scx, scy - 4, scx, scy + 4, color=color, thickness=2)
    if label:
        osd.draw_string_advanced(scx, scy, 24, label, color=color)


# ---------- 任务1: 右侧10%竖条带内识别红/蓝 ----------
def task1_color_strip(img):
    for color in ('R', 'B'):
        b = find_color_blobs(img, rect_task1, color, pixel_th=50)
        if b:
            draw_blob(pl.osd_img, b, label=("Red" if color == 'R' else "Blue"))
            tx("{r}" if color == 'R' else "{b}")
    draw_roi(pl.osd_img, rect_task1)


# ---------- 任务2: 中间竖带内, 判断色块处于上/中/下哪一条带, 发送 {条带号,cx} ----------
def task2_row_pos(img):
    color = 'R' if threshold_index == 1 else 'B'
    # 任务2只在中间竖直区域寻找目标，避免整幅图中的背景颜色干扰判断
    b = find_color_blobs(img, rect_v, color, pixel_th=50)
    if b:
        draw_blob(pl.osd_img, b, label=("Red" if color == 'R' else "Blue"))
        for i in range(0, 3):
            if rect_h[i][1] <= b[1] < rect_h[i][1] + rect_h[i][3]:
                tx("{%d,%d}" % (i + 1, b[0]))
                break
    for i in range(0, 3):
        draw_roi(pl.osd_img, rect_h[i])
    draw_roi(pl.osd_img, rect_v)


# ---------- 任务3: 立柱识别区域内出现色块则发送 {r}/{b}(由 {R}/{B} 决定) ----------
def task3_lizhu(img):
    color = 'R' if threshold_index == 1 else 'B'
    # 任务3使用与任务1相同的 LAB 阈值和色块过滤参数。
    # 这里只保留任务3自己的检测区域和串口协议。
    b = find_color_blobs(img, rect_lz_v, color, pixel_th=50)
    if b:
        if RED_or_BLUE == 'R':
            tx("{r}")
        elif RED_or_BLUE == 'B':
            tx("{b}")
    draw_roi(pl.osd_img, rect_lz_v)


# ---------- 任务4: 3x3九宫格, 每行识别色块所在列, 全识别到才发送 {上,中,下} ----------
def task4_floor(img):
    color = 'R' if threshold_index == 1 else 'B'
    floor = [0, 0, 0]
    b = find_color_blobs(img, (0, 0, VW, VH), color, pixel_th=15)
    if b:
        cx, cy = b[0], b[1]
        for i in range(0, 3):
            for j in range(0, 3):
                if cx > rect[i][j][0] and cx < (rect[i][j][0] + rect[i][j][2]) and cy > rect[i][j][1] and cy < (rect[i][j][1] + rect[i][j][3]):
                    floor[i] = j + 1
    if floor[0] > 0 and floor[1] > 0 and floor[2] > 0:
        tx("{%d,%d,%d}" % (floor[0], floor[1], floor[2]))
    for i in range(0, 3):
        for j in range(0, 3):
            draw_roi(pl.osd_img, rect[i][j])


# ---------- 任务5: 右下半区域识别红/蓝 ----------
def task5_rightbottom(img):
    for color in ('R', 'B'):
        b = find_color_blobs(img, rect_task5, color, pixel_th=20)
        if b and b[0] > VW * 0.9 and b[1] > VH * 0.4:
            draw_blob(pl.osd_img, b, label=("Red" if color == 'R' else "Blue"))
            tx("{r}" if color == 'R' else "{b}")


# ---------- 任务6: 九宫格数字识别(最下方一行为第1行，列从左到右1/2/3) ----------
# 发送 {第1行列数,第1行数字,第2行列数,第2行数字,第3行列数,第3行数字}，未识别到为0
def task6_digit_recognition(res):
    global last_digit_msg
    w, h = det_app.rgb888p_size      # AI通道图像分辨率 [1280,720]
    dw, dh = det_app.display_size    # 显示分辨率 [800,480]
    cw, ch = w // 3, h // 3          # 每格宽高（按AI分辨率计算坐标）
    d_cw, d_ch = dw // 3, dh // 3    # 每格宽高（按显示分辨率绘制）

    cols = [0, 0, 0]         # 第1/2/3行识别到数字所在列(1..3)，0=未识别
    digits = [0, 0, 0]       # 第1/2/3行识别到的数字
    best_score = [-1.0, -1.0, -1.0]

    if len(res["boxes"]):
        for i in range(len(res["boxes"])):
            x1 = int(res["boxes"][i][0]); y1 = int(res["boxes"][i][1])
            x2 = int(res["boxes"][i][2]); y2 = int(res["boxes"][i][3])
            xc = (x1 + x2) // 2          # 检测框中心x
            yc = (y1 + y2) // 2          # 检测框中心y
            col = xc // cw + 1           # 列: 1/2/3（从左到右）
            if col > 3:
                col = 3
            if yc < ch:                  # 画面上方1/3 -> 第3行
                r = 2
            elif yc < ch * 2:            # 画面中间1/3 -> 第2行
                r = 1
            else:                        # 画面下方1/3 -> 第1行
                r = 0
            score = float(res["scores"][i])
            if score > best_score[r]:    # 同一行多个候选框，取置信度最高的
                best_score[r] = score
                cols[r] = col
                digits[r] = int(det_app.labels[res["idx"][i]])

    # ---- 屏幕绘制: 九宫格 ----
    white = (255, 255, 255, 255)
    for vx in (d_cw, d_cw * 2):                         # 两条竖线
        pl.osd_img.draw_line(vx, 0, vx, dh, color=white, thickness=2)
    for hy in (d_ch, d_ch * 2):                         # 两条横线
        pl.osd_img.draw_line(0, hy, dw, hy, color=white, thickness=2)
    for s in range(3):                                  # 左侧行号标注, 屏幕第s行(0=上)对应任务行号 3-s
        pl.osd_img.draw_string_advanced(4, s * d_ch + 8, 20, "R%d" % (3 - s), color=(255, 255, 255, 0))

    # ---- 屏幕绘制: 识别到的数字（绿色方框高亮所在格） ----
    for r in range(3):                                  # r=0 第1行(屏幕最下)
        if digits[r] > 0:
            s = 2 - r                                   # 屏幕行号(0=最上)
            x0 = (cols[r] - 1) * d_cw
            y0 = s * d_ch
            pl.osd_img.draw_rectangle(x0, y0, d_cw, d_ch, color=(255, 0, 255, 0), thickness=3)
            pl.osd_img.draw_string_advanced(x0 + d_cw // 2 - 10, y0 + d_ch // 2 - 14, 24,
                                            str(digits[r]), color=(255, 0, 255, 0))

    # ---- 任务6终端打印 + 串口发送 ----
    msg = "{%d,%d,%d,%d,%d,%d}" % (cols[0], digits[0], cols[1], digits[1], cols[2], digits[2])

    # 只有三行数字都识别到时，才允许终端打印和 UART 发送。
    # 任意一行缺失时不输出，避免把带 0 的不完整结果发给主控。
    all_digits_found = (digits[0] > 0 and digits[1] > 0 and digits[2] > 0)
    if all_digits_found:
        if PRINT_TX and TASK6_PRINT_EVERY_FRAME:
            print(msg)
        if msg != last_digit_msg:      # 完整结果变化时只发送一次
            last_digit_msg = msg
            if is_Mute == 'S':
                uart.write(msg.encode())
    else:
        # 允许下一次重新识别完整三行数字后再次发送。
        last_digit_msg = ""


# ---------- 串口命令接收: 与OpenMV相同的"以 } 结尾"协议 ----------
def uart_task_recv():
    global Task_flag, is_Mute, RED_or_BLUE, threshold_index, uart_buf
    try:
        pending = uart.any()
        if not pending:
            return

        data = uart.read()
        if not data:
            return

        # CanMV firmware may return bytes, bytearray, or str from UART.read().
        if isinstance(data, str):
            text = data
        else:
            text = bytes(data).decode('utf-8', 'ignore')

        print("UART_RX:", repr(text))
        uart_buf += text

        while '}' in uart_buf:
            end = uart_buf.index('}')
            start = uart_buf.rfind('{', 0, end)
            if start >= 0:
                cmd = uart_buf[start:end + 1]
                print("UART_CMD:", repr(cmd))

                if cmd == "{B}":
                    RED_or_BLUE = 'B'
                    threshold_index = 2
                elif cmd == "{R}":
                    RED_or_BLUE = 'R'
                    threshold_index = 1
                elif cmd == "{1}":
                    Task_flag = 1
                    tx("{K1}")
                elif cmd == "{2}":
                    Task_flag = 2
                    tx("{K2}")
                elif cmd == "{3}":
                    Task_flag = 3
                    tx("{K3}")
                elif cmd == "{4}":
                    Task_flag = 4
                    tx("{K4}")
                elif cmd == "{5}":
                    Task_flag = 5
                    tx("{K5}")
                elif cmd == "{10}":
                    Task_flag = 6
                    tx("{K6}")
                elif cmd == "{M}":
                    is_Mute = 'M'
                elif cmd == "{S}":
                    is_Mute = 'S'

                print("TASK_FLAG:", Task_flag)

            uart_buf = uart_buf[end + 1:]
    except Exception as e:
        print("UART_RX_ERROR:", repr(e))
# ===================== 【新增】移植部分结束 =====================


# Startup marker for testing without a K230 debug console.
tx("{BOOT3}")

# Main loop: capture, run inference, display results
while True:
    with ScopedTiming("total", 1):
        # 【新增】数字模型推理只在任务6执行；任务1~5为纯颜色识别，不识别数字，速度更快
        if Task_flag == 6:
            img = pl.get_frame()                      # 通道2：RGBP888，数字模型输入
            res = det_app.run(img)                    # Run inference
            det_app.draw_result(pl.osd_img, res)      # Draw detection results (内含清屏)
            task6_digit_recognition(res)
        else:
            img = pl.sensor.snapshot(chn=CAM_CHN_ID_1) # 通道1：RGB565，官方 LAB find_blobs
            pl.osd_img.clear()                        # 颜色任务的清屏(原 draw_result 里的 clear)
            if Task_flag == 1:
                task1_color_strip(img)
            elif Task_flag == 2:
                task2_row_pos(img)
            elif Task_flag == 3:
                task3_lizhu(img)
            elif Task_flag == 4:
                task4_floor(img)
            elif Task_flag == 5:
                task5_rightbottom(img)
        pl.show_image()                               # Show result on display
        gc.collect()                                  # Run garbage collection
    # 【新增】串口命令处理
    uart_task_recv()

# Cleanup: These lines will only run if the loop is interrupted (e.g., by an IDE break or external interruption)
det_app.deinit()                                      # De-initialize detection app
pl.destroy()                                          # Destroy pipeline instance
