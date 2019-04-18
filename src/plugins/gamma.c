#include "plg_common.h"
#include <math.h>

CALC_FN(gamma) {
  const double x2 = tgamma(fabs(x));
  return (x < 0) ? (-x2) : (x2);
}
