#include "plg_common.h"

CALC_FN(raw) { return x; }
CALCINV_FN(raw) __attribute__((alias("raw_calc")));
