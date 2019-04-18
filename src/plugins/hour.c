#include "plg_common.h"

CALC_FN(hour)    { return x * 60; }
CALCINV_FN(hour) { return x / 60; }
