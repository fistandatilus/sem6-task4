#ifndef MSR_H
#define MSR_H

#include <sys/types.h>
#include <string.h>
#include <math.h>
#include "thread_related.h"

struct msr
{
  double *data = nullptr;
  size_t *indexes = nullptr; //does not own
  double norm = 0;
  size_t n = 0;
  size_t size = 0;

  int set_template(size_t *ind, size_t n, size_t size);
  void erase()
  {
    if (data)
      delete[] data;
    data = nullptr;
    indexes = nullptr;
    n = 0;
    size = 0;
    norm = 0;
  }
  ~msr()
  {
    erase();
  }
  msr() = default;
  msr(msr &) = delete;
  msr &operator=(msr &) = delete;
  int copy(msr &x);
  int copy_template(msr &x);
  void print(size_t p, FILE *fp = stdout);
};

int form_preconditioner(msr &a, msr &precond, double *diag, double eps, int p, int thread);
size_t bin_search(size_t *a, size_t n, size_t x);
double bprod(size_t i, size_t j, size_t nx, size_t ny, double a, double b, double hx, double hy, double f(double, double));
void fill_right_side(size_t nx, size_t ny, double *right, int p, int thread, double a, double b, double c, double d, double f(double, double));
int init_gramm_struct(size_t nx, size_t ny, int p, int thread, size_t **indexes_ret);
void fill_gramm(msr &matr, size_t nx, size_t ny, int p, int thread, double a, double b, double c, double d);
size_t size_by_nx_ny(size_t nx, size_t ny);

void inv_m_mul_vec(msr &m, double *d, double *r, double *v, size_t start, size_t stride);
int solve(msr &a, double *b, msr &m, double *d, double *x, double *r, double *u, double *v, double desired_eps, int p, int thread, int max_it, int &iter);
void mul_msr_by_vec(msr &a, double *x, double *ret, size_t start, size_t stride);
double dot_prod(const double *u, const double *v, size_t start, size_t stride);

void print_vec(double *a, size_t n, size_t p, FILE *fp = stdout);

double eval_approx(msr &matr, double x, double y, double a, double b, double c, double d);
#endif //MSR_H
