#include "zxmath.h"
#include <math.h>

double zx_modulo(double x, const double y) {
  x /= y;
  return (x - trunc(x)) * y;
}
