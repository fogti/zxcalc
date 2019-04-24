#include "plg_common.h"

CALC_FN(percent)    { return 1.0 +  x  / 100.0; }
CALCINV_FN(percent) { return (x - 1.0) * 100.0; }

__attribute__((const, always_inline))
static inline double invperc_wrap(const double x, const bool cond) {
  const double x2 = percent_calc(x);
  return cond ? (1.0 / x2) : x2;
}

CALC_FN(invnperc) { return invperc_wrap( fabs(x), (x < 0.0)); }
CALC_FN(invpperc) { return invperc_wrap(-fabs(x), (x > 0.0)); }
