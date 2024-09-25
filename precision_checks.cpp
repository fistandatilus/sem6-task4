#include "approximation.h"

double approximation::residual1(int p, int thread)
{
  size_t start, stride;
  start_and_size(p, thread, ny, start, stride);
  double hx = (b - a)/nx, hy = (d - c)/ny;
  double max = 0;
  approximation &pf = *this;

  for (size_t j = start; j < start + stride; j++)
  {
    double y = d - (j + 0.5)*hy;
    for (size_t i = 0; i < 2*nx; i++)
    {
      double x = a + i*0.5*hx;
      double value = fabs(pf(x, y) -  f(x, y));
      if (value > max)
        max = value;
    }
  }
  reduce_max(p, &max, 1);
  return max;
}

double approximation::residual2(int p, int thread)
{
  size_t start, stride;
  start_and_size(p, thread, ny, start, stride);
  double hx = (b - a)/nx, hy = (d - c)/ny;
  double sum = 0;
  approximation &pf = *this;

  for (size_t j = start; j < start + stride; j++)
  {
    double y = d - (j + 0.5)*hy;
    for (size_t i = 0; i < 2*nx; i++)
    {
      double x = a + i*0.5*hx;
      double value = fabs(pf(x, y) -  f(x, y)) * hx * hy * 0.5;
      sum += value;
    }
  }
  reduce_sum(p, &sum, 1);
  return sum;
}

double approximation::residual3(int p, int thread)
{
  size_t start, stride;
  start_and_size(p, thread, ny + 1, start, stride);
  double hx = (b - a)/nx, hy = (d - c)/ny;
  double max = 0;
  approximation &pf = *this;

  for (size_t j = start; j < start + stride; j++)
  {
    double y = d - j*hy;
    for (size_t i = 0; i <= nx; i++)
    {
      double x = a + i*hx;
      double value = fabs(pf(x, y) -  f(x, y));
      if (value > max)
        max = value;
    }
  }
  reduce_max(p, &max, 1);
  return max;
}

double approximation::residual4(int p, int thread)
{
  size_t start, stride;
  start_and_size(p, thread, ny + 1, start, stride);
  double hx = (b - a)/nx, hy = (d - c)/ny;
  double sum = 0;
  approximation &pf = *this;

  for (size_t j = start; j < start + stride; j++)
  {
    double y = d - j*hy;
    for (size_t i = 0; i <= nx; i++)
    {
      double x = a + i*hx;
      double value = fabs(pf(x, y) -  f(x, y)) * hx * hy;
      sum += value;
    }
  }
  reduce_sum(p, &sum, 1);
  return sum;
}
