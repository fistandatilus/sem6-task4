#include "msr.h"

#ifndef APPROXIMATION_H
#define APPROXIMATION_H

class approximation
{
  private:
    msr gramm;
    msr precond;
    double *diag = nullptr;
    size_t *msr_template = nullptr;
    double *coeffs = nullptr;
    double *u = nullptr;
    double *v = nullptr;
    double *r = nullptr;
    double *right_side = nullptr;
    double (*f)(double, double) = nullptr;
    double a = 0;
    double b = 0;
    double c = 0;
    double d = 0;
    size_t nx = 0;
    size_t ny = 0;

  public:
    status init(double a, double b, double c, double d, size_t nx, size_t ny, double eps, int p, int thread);
    status init_function(double (*f)(double, double), int max_it, int &it, double eps, int p, int thread);
    double operator()(double x, double y);
    double residual1(int p, int thread);
    double residual2(int p, int thread);
    double residual3(int p, int thread);
    double residual4(int p, int thread);

    void erase()
    {
      gramm.erase();
      precond.erase();
      if (diag)
        delete[] diag;
      if (msr_template)
        delete[] msr_template;
      if (coeffs)
        delete[] coeffs;
      if (u)
        delete[] u;
      if (v)
        delete[] v;
      if (r)
        delete[] r;
      if (right_side)
        delete[] right_side;
      diag = nullptr;
      msr_template = nullptr;
      coeffs = nullptr;
      u = nullptr;
      v = nullptr;
      r = nullptr;
      f = nullptr;
      right_side = nullptr;
      a = 0;
      b = 0;
      c = 0;
      d = 0;
      nx = 0;
      ny = 0;
    }
    ~approximation()
    {
      erase();
    }
};

#endif
