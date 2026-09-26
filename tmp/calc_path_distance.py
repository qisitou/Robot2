# -*- coding: utf-8 -*-
# 计算 Chassis_MovePath(Chassis_Path_StartToTurntable) 实际走过的路径与距离
# 换算: R_PARAMETER=0.0762 (76.2mm轮径) -> 1 RPM = pi*76.2/60 = 3.99 mm/s
#       每帧 2ms, 每帧每1RPM走 3.99*0.002 = 0.0079796 mm
import math

k = math.pi * 76.2 / 60.0 * 0.002   # mm per (RPM * frame)
v0 = (-120.0, 100.0)                # 路径点0 速度
v1 = (0.0, 500.0)                   # 路径点1 速度

def ramp(a, b, K):
    """正弦加减速: 从速度a到速度b, K帧, f(i)=0.5-0.5cos(pi*i/K), 取 i=0..K-1"""
    r = []
    for i in range(K):
        f = 0.5 - 0.5 * math.cos(math.pi * i / K)
        r.append((a[0] + (b[0] - a[0]) * f, a[1] + (b[1] - a[1]) * f))
    return r

phases = [
    ("1 accel 0->(-120,100) K=120 ", ramp((0, 0), v0, 120)),
    ("2 const (-120,100) t=220    ", [v0] * 220),
    ("3 trans (-120,100)->(0,500) K=600", ramp(v0, v1, 600)),
    ("4 const (0,500) t=135       ", [v1] * 135),
    ("5 decel (0,500)->0  End_K=600", ramp(v1, (0, 0), 600)),
]

TX = TY = TL = TN = 0
for name, fr in phases:
    sx = sum(v[0] for v in fr)
    sy = sum(v[1] for v in fr)
    sl = sum(math.hypot(v[0], v[1]) for v in fr)
    TX += sx; TY += sy; TL += sl; TN += len(fr)
    print("%s frames=%4d %5.0fms  dx=%7.1f  dy=%8.1f  len=%8.1f mm"
          % (name, len(fr), len(fr) * 2, sx * k, sy * k, sl * k))

print("-" * 72)
print("TOTAL frames=%d  time=%.2fs" % (TN, TN * 0.002))
print("net   dx=%.1f mm  dy=%.1f mm  (dx<0 = left, dy>0 = forward)" % (TX * k, TY * k))
print("straight-line chord = %.1f mm" % (math.hypot(TX, TY) * k))
print("path length = %.1f mm" % (TL * k))
cruise = (sum(math.hypot(*v) for v in phases[1][1]) + sum(math.hypot(*v) for v in phases[3][1])) * k
print("cruise-only (t segments) = %.1f mm ; extra from accel/trans/decel = %.1f mm"
      % (cruise, TL * k - cruise))
