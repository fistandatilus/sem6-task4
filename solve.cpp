#include "msr.h"

#define UNFOLD 8
#define R_PERIOD 50

void mul_msr_by_vec(msr &a, double *x, double *ret, size_t start, size_t stride)
{
    for (size_t i = start; i < start + stride; i++) {
        double s = a.data[i] * x[i];
        size_t len = a.indexes[i + 1] - a.indexes[i];
        for (size_t j = 0; j < len; j++)
            s += a.data[a.indexes[i] + j] * x[a.indexes[a.indexes[i] + j]];
        ret[i] = s;
    }
}

static inline void subtract_vecs_coeff(double *a, double *b, double c, int n)
//does a -= c*b in sense of vectors
{
    int i, rem = n % UNFOLD;
    for (i = 0; i < rem; i++)
        a[i] -= c * b[i];
    for (; i < n; i += UNFOLD) {
        a[i] -= c * b[i];
        a[i + 1] -= c * b[i + 1];
        a[i + 2] -= c * b[i + 2];
        a[i + 3] -= c * b[i + 3];
        a[i + 4] -= c * b[i + 4];
        a[i + 5] -= c * b[i + 5];
        a[i + 6] -= c * b[i + 6];
        a[i + 7] -= c * b[i + 7];
    }
}

void inv_m_mul_vec(msr &m, double *d, double *r, double *v, size_t start, size_t stride)
{
    memcpy(v + start, r + start, stride * sizeof(double));

    for (size_t i = start; i < start + stride; i++) {
        size_t l, row_len = m.indexes[i + 1] - m.indexes[i];

        for (l = 0; l < row_len && m.indexes[m.indexes[i] + l] < i; l++)
            ;
        for (; l < row_len && m.indexes[m.indexes[i] + l] < start + stride; l++) {
            size_t cur_line = m.indexes[m.indexes[i] + l];
            size_t k = bin_search(m.indexes + m.indexes[cur_line],
                                  m.indexes[cur_line + 1] - m.indexes[cur_line],
                                  i);
            v[cur_line] -= v[i] * m.data[m.indexes[cur_line] + k] / m.data[i];
        }
    }

    for (size_t i = start; i < start + stride; i++)
        v[i] /= d[i];

    for (size_t i = start + stride; i > start; i--) {
        i--;
        size_t l, row_len = m.indexes[i + 1] - m.indexes[i];
        for (l = 0; l < row_len && m.indexes[m.indexes[i] + l] < start; l++)
            ;
        for (; l < row_len && m.indexes[m.indexes[i] + l] < i; l++) {
            size_t cur_line = m.indexes[m.indexes[i] + l];
            size_t k = bin_search(m.indexes + m.indexes[cur_line],
                                  m.indexes[cur_line + 1] - m.indexes[cur_line],
                                  i);
            v[cur_line] -= v[i] * m.data[m.indexes[cur_line] + k] / m.data[i];
        }
        i++;
    }
}

double dot_prod(const double *u, const double *v, size_t start, size_t stride)
{
    double res = 0;
    for (size_t i = start; i < start + stride; i++)
        res += u[i] * v[i];
    return res;
}

int solve(msr &a,
          double *b,
          msr &m,
          double *d,
          double *x,
          double *r,
          double *u,
          double *v,
          double desired_eps,
          int p,
          int thread,
          int max_it,
          int &iter)
{
    size_t stride, start, n = a.n;
    start_and_size(p, thread, n, start, stride);

    memset(x + start, 0, stride * (sizeof(double)));
    reduce_sum<double>(p);
    mul_msr_by_vec(a, x, r, start, stride);
    subtract_vecs_coeff(r + start, b + start, 1, stride);
    double eps = 0;

    for (size_t i = start; i < start + stride; i++)
        eps += fabs(b[i]);
    reduce_sum(p, &eps, 1);
    eps *= desired_eps * desired_eps;
    /*
  if (thread == 0)
    printf("%lu\n", a.size);
  if (thread == 0)
  {
    m.print(n);
    print_vec(d, n, n);
  }
*/
    double t;
    for (iter = 1; iter <= max_it; iter++) {
        //memcpy(v + start, r + start, stride*(sizeof(double))); (void) m; (void) d;
        inv_m_mul_vec(m, d, r, v, start, stride);
        reduce_sum<double>(p);
        mul_msr_by_vec(a, v, u, start, stride);
        double c[2];
        c[0] = dot_prod(v, r, start, stride);
        c[1] = dot_prod(u, v, start, stride);
        reduce_sum(p, c, 2);
        //printf("thread = %d, iter = %d, c0 = %le, c1 = %le, eps = %le\n", thread, iter, c[0], c[1], eps);
        if (fabs(c[0]) <= eps || fabs(c[1]) <= eps)
            return 0;
        t = c[0] / c[1];
        //printf("thread = %d, c0 = %le, c1 = %le,  t = %le\n", thread, c[0], c[1], t);
        subtract_vecs_coeff(x + start, v + start, t, stride);
        subtract_vecs_coeff(r + start, u + start, t, stride);
        //    if (iter%R_PERIOD == R_PERIOD - 1)
        //    {
        //    mul_msr_by_vec(a, x, r, start, stride);
        //    subtract_vecs_coeff(r + start, b + start, 1, stride);
        //    }
    }
    return 1;
}
