# -*- coding: utf-8 -*-
# 模拟 LTCK.c Go_To_Warehouse() 中"处理仓库"循环
# 验证写死的 HoleArr 编码在各种识别数据下, find_hole 是否总能找到正确的孔位
from itertools import permutations, product

# 用户写死的孔位编码 (0~9 号孔位)
BASE_IC = [0x13, 0x31, 0x21, 0x11, 0x32, 0x22, 0x12, 0x33, 0x23, 0x13]


def simulate(real_col, layer, blocks):
    """
    real_col : K230 识别的列顺序(第1/2/3物理列的仓库列号)
    layer    : OpenMV 识别的每层有积木的物理列 (layer[0]=顶层, layer[1]=中层, layer[2]=底层), 0=无
    blocks   : (b3,b2,b1) 顶层/中层/底层积木上的数字
    返回 (events, issues); events: (列,行,分支,抓积木动作组,命中孔,放球动作组)
    """
    Hole = BASE_IC[:]

    def find(ic):
        for i in range(10):
            if Hole[i] == ic:
                return i
        return -1

    events, issues = [], []
    for wi in range(3):                 # 物理列 1 -> 2 -> 3
        col = wi + 1
        vcol = real_col[wi]             # = warehouse_virtual_col
        for row in (3, 2, 1):           # 顶层 -> 中层 -> 底层
            branch = 'A' if layer[3 - row] == col else 'B'
            ag1 = None
            if branch == 'A':           # 该层本列有积木: 先抓积木上车
                ag1 = (row + 2) * 10 + blocks[3 - row]
            hole = find((row << 4) | vcol)
            if hole == -1:
                issues.append((col, row, hex((row << 4) | vcol), 'find_hole = -1'))
            else:
                events.append((col, row, branch, ag1, hole, 10 + row))
                if branch == 'B':       # 分支B才清 ic (与 C 代码一致)
                    Hole[hole] = 0
    return events, issues


# ---------- 1) 全量统计: real_col 为 1/2/3 的排列, layer 任意, 积木数字 1..3 ----------
total = 0
miss_combo = 0
reuse_combo = 0
hole9_used = 0
used_sets = set()
bad_ag = 0

for rc in permutations([1, 2, 3]):
    for lay in product([0, 1, 2, 3], repeat=3):
        for bl in product([1, 2, 3], repeat=3):
            ev, iss = simulate(rc, lay, bl)
            total += 1
            if iss:
                miss_combo += 1
                if miss_combo <= 3:
                    print("miss:", rc, lay, bl, iss)
            holes = [e[4] for e in ev]
            if len(holes) != len(set(holes)):
                reuse_combo += 1
                if reuse_combo <= 3:
                    print("reuse:", rc, lay, bl, holes)
            if 9 in holes:
                hole9_used += 1
            used_sets.add(tuple(sorted(holes)))
            for e in ev:
                if e[3] is not None and e[3] not in (31, 32, 33, 41, 42, 43, 51, 52, 53):
                    bad_ag += 1

print("=" * 60)
print("组合总数:", total)
print("find_hole 出现 -1 的组合数:", miss_combo)
print("同一次流程里重复用到同一个孔的组合数:", reuse_combo)
print("9 号孔被用到的组合数:", hole9_used)
print("9 次放球命中的孔位集合(去重后共有 %d 种):" % len(used_sets), sorted(used_sets)[:3])
print("非法抓积木动作组次数:", bad_ag)

# ---------- 2) 具体演示一例 ----------
print("=" * 60)
print("演示: real_col={2,3,1}  layer={2,3,2}  积木数字(顶/中/底)=1,2,3")
ev, iss = simulate((2, 3, 1), (2, 3, 2), (1, 2, 3))
for col, row, branch, ag1, hole, ag2 in ev:
    tag = "有积木, 抓上车(动作组%s) " % ag1 if branch == 'A' else "无积木               "
    print("  第%d列 行%d: %s-> 找孔 hole=%d -> 放球动作组%d" % (col, row, tag, hole, ag2))
print("issues:", iss)

# ---------- 3) 异常演示: K230 万一发重复列号 ----------
print("=" * 60)
print("异常演示(仅参考): real_col={3,3,1}  layer={1,3,1}")
ev, iss = simulate((3, 3, 1), (1, 3, 1), (1, 1, 1))
for col, row, branch, ag1, hole, ag2 in ev:
    tag = "有积木, 抓上车(动作组%s) " % ag1 if branch == 'A' else "无积木               "
    print("  第%d列 行%d: %s-> 找孔 hole=%d -> 放球动作组%d" % (col, row, tag, hole, ag2))
print("issues:", iss)
