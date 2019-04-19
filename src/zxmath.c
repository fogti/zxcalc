#include "zxmath.h"
#include <math.h>

double zx_modulo(double x, double y) {
  x /= y;
  x -= trunc(x);
  return x * y;
}
