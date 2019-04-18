#include "plg_common.h"
#include <math.h>

CALC_FN(invnperc) {
  const double x2 = 1.0 + fabs(x) / 100.0;
  return (x < 0.0) ? (1.0 / x2) : (x2);
}
