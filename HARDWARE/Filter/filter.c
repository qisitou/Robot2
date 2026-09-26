#include "headfile.h"

static bool CheckCompare(int32_t value, int32_t threshold, CompareType cmp)
{
    switch (cmp) {
        case CMP_GT: return value >  threshold;   // 大于 (>)          value 大于 threshold 时返回 true
        case CMP_GE: return value >= threshold;   // 大于等于 (>=)      value 不小于 threshold 时返回 true
        case CMP_LT: return value <  threshold;   // 小于 (<)          value 小于 threshold 时返回 true
        case CMP_LE: return value <= threshold;   // 小于等于 (<=)      value 不大于 threshold 时返回 true
        case CMP_EQ: return value == threshold;   // 等于 (==)         value 等于 threshold 时返回 true
        case CMP_NE: return value != threshold;   // 不等于 (!=)       value 不等于 threshold 时返回 true
        default:     return false;                // 未知比较类型，一律返回 false（视为不满足条件）
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
