#include "approximation.h"

double approximation::operator()(double x, double y)
{
  if (x < a || x > b || y < c || y > d)
    return 0;

  double hx = (b - a)/nx, hy = (d - c)/ny;
  size_t i = (x - a)/hx, j = (d - y)/hy;
  if (i >= nx)
    i = nx - 1;
  if (j >= ny)
    j = ny - 1;
  double x_loc = x - hx*i - a, y_loc = d - hy*j - y;

  double kx = x_loc/hx;
  double ky = y_loc/hy;
  if (x_loc/hx + y_loc/hy < 0.5)
  {
    double ka = 1 - ky - kx;
    double kb = kx;
    double kc = ky;
    return ka*coeffs[j*(nx + 1) + i] + kb*coeffs[j*(nx + 1) + i + 1] + kc*coeffs[(j + 1)*(nx + 1) + i];
  }
  double kd = kx + ky - 1;
  double kb = 1 - ky;
  double kc = 1 - kx;
  return kd*coeffs[(j + 1)*(nx + 1) + i + 1] + kb*coeffs[j*(nx + 1) + i + 1] + kc*coeffs[(j + 1)*(nx + 1) + i];
}
