#include "plg_common.h"

CALC_FN(raw) { return x; }
CALC_FN(min) __attribute__((alias("raw_calc")));
CALCINV_FN(raw) __attribute__((alias("raw_calc")));
