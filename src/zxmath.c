#include "zxmath.h"
#include <math.h>

double zx_modulo(double x, const double y) {
  x /= y;
  return (x - trunc(x)) * y;
}

double zx_sgnprop(double v, const double s) {
  if(s < 0) v *= -1;
  return v;
}
