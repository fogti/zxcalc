#include "plg_common.h"

CALC_FN(day)    { return x * 1440; }
CALCINV_FN(day) { return x / 1440; }

CALC_FN(hour)    { return x * 60; }
CALCINV_FN(hour) { return zx_modulo(x / 60, 24); }

//CALC_FN(min) -- defined in raw.c
CALCINV_FN(min) { return zx_modulo(x, 60); }

CALC_FN(sec)    { return              x / 60; }
CALCINV_FN(sec) { return (x - trunc(x)) * 60; }
