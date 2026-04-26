#include "headfile.h"

static bool CheckCompare(int32_t value, int32_t threshold, CompareType cmp)
{
    switch (cmp) {
        case CMP_GT: return value >  threshold;
        case CMP_GE: return value >= threshold;
        case CMP_LT: return value <  threshold;
        case CMP_LE: return value <= threshold;
        case CMP_EQ: return value == threshold;
        case CMP_NE: return value != threshold;
        default:     return false;
    }
}

// 连续 need_hits 次满足比较条件，返回 true
// 不满足时计数清零
bool Range_ConsecutiveMatch_AutoCnt(int32_t value,
                                    CompareType cmp,
                                    int32_t threshold,
                                    uint8_t need_hits)
{
    static uint8_t hit_cnt = 0;

    if (CheckCompare(value, threshold, cmp))
    {
        if (hit_cnt < 255) hit_cnt++;
    }
    else
    {
        hit_cnt = 0;
    }

    if (hit_cnt >= need_hits)
    {
        hit_cnt = 0;   // 命中后自动复位
        return false;
    }

    return true;
}
