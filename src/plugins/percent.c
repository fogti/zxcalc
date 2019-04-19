#include "plg_common.h"
#include <math.h>

CALC_FN(percent)    { return 1.0 +  x  / 100.0; }
CALCINV_FN(percent) { return (x - 1.0) * 100.0; }

CALC_FN(invnperc) {
  const double x2 = 1.0 + fabs(x) / 100.0;
  return (x < 0.0) ? (1.0 / x2) : (x2);
}

CALC_FN(invpperc) {
  const double x2 = 1.0 - fabs(x) / 100.0;
  return (x > 0.0) ? (1.0 / x2) : (x2);
}
