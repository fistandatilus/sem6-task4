#include <QMetaType>

#include "msr.h"

#ifndef APPROXIMATION_H
#define APPROXIMATION_H

class Paintable
{
    public:
    virtual double operator()(double x, double y) = 0;
};

class approximation : public Paintable
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
    double a = 0;
    double b = 0;
    double c = 0;
    double d = 0;
    size_t nx = 0;
    size_t ny = 0;

protected:
    double (*f)(double, double) = nullptr;

  public:
    status init(double a, double b, double c, double d, size_t nx, size_t ny, double eps, int p, int thread);
    status init_function(double (*f)(double, double), int max_it, int &it, double eps, int p, int thread);
    virtual double operator()(double x, double y);
    double residual1(int p, int thread);
    double residual2(int p, int thread);
    double residual3(int p, int thread);
    double residual4(int p, int thread);
    void max_min(int p, int thread, double &max, double &min);
    void max_min_f(int p, int thread, double &max, double &min);

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
    approximation() = default;
    virtual ~approximation()
    {
      erase();
    }

    friend class TrivialApproximation;
    friend class DifferenceApproximation;
};

class TrivialApproximation : public Paintable
{
private:
    double (*f)(double, double) = nullptr;

public:
    TrivialApproximation() = default;
    void init(double (*f)(double, double)) {
        this->f = f;
    }
    ~TrivialApproximation() {
        f = nullptr;
    }
    void set_f(double (*f)(double, double)) {
        this->f = f;
    }
    virtual double operator()(double x, double y) {
//      if (f)
            return f(x, y);
//        return 0;
    }
    friend class DifferenceApproximation;
};

class DifferenceApproximation : public Paintable
{
private:
    approximation *approx;
    double (*f)(double, double);
public:
    DifferenceApproximation() = default;
    DifferenceApproximation(approximation *app, double (*f_ptr)(double, double)) {
        approx = app;
        f = f_ptr;
    }
    ~DifferenceApproximation() {
        approx = nullptr;
    }
    void init(approximation *app, double (*f_ptr)(double, double)) {
        approx = app;
        f = f_ptr;
    }
    virtual double operator()(double x, double y) {
        return fabs(f(x, y) - approx->operator()(x, y));
    }
};

struct arguments
{
    approximation *approx = nullptr;
    double a = 0;
    double b = 0;
    double c = 0;
    double d = 0;
    double eps = 0;
    size_t nx = 0;
    size_t ny = 0;
    int max_it = 0;
    int k = 0;

    double r1 = 0;
    double r2 = 0;
    double r3 = 0;
    double r4 = 0;
    double t1 = 0;
    double t2 = 0;
    int it = 0;
    status stat;

    int p = 0;
    int thread = 0;

    void set(approximation *approx,
             double a,
             double b,
             double c,
             double d,
             double eps,
             size_t nx,
             size_t ny,
             int max_it,
             int k,
             int p,
             int thread)
    {
        this->approx = approx;
        this->a = a;
        this->b = b;
        this->c = c;
        this->d = d;
        this->eps = eps;
        this->nx = nx;
        this->ny = ny;
        this->max_it = max_it;
        this->k = k;
        this->p = p;
        this->thread = thread;
    }

    arguments() = default;
    arguments(const arguments & arg) {
        approx = arg.approx;
        a = arg.a;
        b = arg.b;
        c = arg.c;
        d = arg.d;
        eps = arg.eps;
        nx = arg.nx;
        ny = arg.ny;
        max_it = arg.max_it;
        k = arg.k;
        r1 = arg.r1;
        r2 = arg.r2;
        r3 = arg.r3;
        r4 = arg.r4;
        t1 = arg.t1;
        t2 = arg.t2;
        it = arg.it;
        stat = arg.stat;
        p = arg.p;
        approx = arg.approx;
    }
    ~arguments() = default;
};
Q_DECLARE_METATYPE(arguments)

#endif
