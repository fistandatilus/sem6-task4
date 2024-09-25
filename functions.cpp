#include "functions.h"

double f0(double /*x*/, double /*y*/)
{
  return 1.;
}

double f1(double x, double /*y*/)
{
  return x;
}

double f2(double /*x*/, double y)
{
  return y;
}

double f3(double x, double y)
{
  return x + y;
}

double f4(double x, double y)
{
  return sqrt(x*x + y*y);
}

double f5(double x, double y)
{
  return x*x + y*y;
}

double f6(double x, double y)
{
  return exp(x*x - y*y);
}

double f7(double x, double y)
{
  return 1./(25.*(x*x + y*y) + 1);
}

void set_func (double (**f)(double, double), int k)
{
  switch (k)
  {
  case 0:
    *f = f0;
    return;
  case 1:
    *f = f1;
    return;
  case 2:
    *f = f2;
    return;
  case 3:
    *f = f3;
    return;
  case 4:
    *f = f4;
    return;
  case 5:
    *f = f5;
    return;
  case 6:
    *f = f6;
    return;
  case 7:
    *f = f7;
    return;
  }
}
