#ifndef __FILTER_H
#define __FILTER_H

#include "sys.h"
#include "stdbool.h"

typedef enum {
    CMP_GT,   // >
    CMP_GE,   // >=
    CMP_LT,   // <
    CMP_LE,   // <=
    CMP_EQ,   // ==
    CMP_NE    // !=
} CompareType;

static bool CheckCompare(int32_t value, int32_t threshold, CompareType cmp);
bool Range_ConsecutiveMatch_AutoCnt(int32_t value,
                                    CompareType cmp,
                                    int32_t threshold,
                                    uint8_t need_hits);

#endif 
