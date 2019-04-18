#include "plg_common.h"

CALC_FN(percent)    { return 1.0 +  x  / 100.0; }
CALCINV_FN(percent) { return (x - 1.0) * 100.0; }
