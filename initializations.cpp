#include "msr.h"

int msr::set_template(size_t *ind, size_t n, size_t size)
{
  erase();
  indexes = ind;
  this->n = n;
  this->size = size;
  data = new double[size];
  if (data == nullptr)
    return 1;
  return 0;
}

size_t size_by_nx_ny(size_t nx, size_t ny)
{
  return (nx + 1)*(ny + 1) + 1 + 6*(nx - 1)*(ny - 1) + 2*4*(nx - 1) + 2*4*(ny - 1) + 6 + 4;
}

int init_gramm_struct(size_t nx, size_t ny, int p, int thread, size_t **indexes_ret)
{
  size_t n = (nx + 1)*(ny + 1);
  size_t size = n + 1 + 6*(nx - 1)*(ny - 1) + 2*4*(nx - 1) + 2*4*(ny - 1) + 6 + 4;
  size_t flag = 0;
  if (thread == 0)
  {
    *indexes_ret = new size_t[size];
    if (!*indexes_ret)
      flag = 1;
  }

  reduce_sum(p, &flag, 1);
  size_t *indexes = *indexes_ret;
  //printf ("thread = %d, flag = %lu, *indexes_ret = %lu\n", thread, flag, (size_t)*indexes_ret, (size_t)*indexes));
  //printf ("thread = %d, flag = %lu, *indexes_ret = %lu\n", thread, flag, (size_t)*indexes);
  if (flag)
    return 1;

  size_t stride, start;
  start_and_size(p, thread, ny + 1, start, stride);

  if (start == 0)
  {
    indexes[0] = n + 1;
    indexes[indexes[0]]     = 1;
    indexes[indexes[0] + 1] = nx + 1;

    for (size_t i = 1; i < nx; i++)
    {
      indexes[i] = (n + 1) + i*4 - 2;
      indexes[indexes[i]]     = i - 1;
      indexes[indexes[i] + 1] = i + 1;
      indexes[indexes[i] + 2] = i + nx;
      indexes[indexes[i] + 3] = i + nx + 1;
    }

    indexes[nx] = (n + 1) + nx*4 - 2;
    indexes[indexes[nx]]     = nx - 1;
    indexes[indexes[nx] + 1] = 2*nx;
    indexes[indexes[nx] + 2] = 2*nx + 1;

    start = 1;
    stride--;
  }

  size_t index_step = (6*nx + 2);
  size_t index_offset = n + 1 + (start - 1)*index_step + 4*(nx + 1) - 3;

  if (start + stride == ny + 1)
  {
    indexes[n] = size;

    size_t base = n - (nx + 1);
    size_t loc_index_offset = index_offset + index_step*(stride - 1);
    indexes[base] = loc_index_offset;
    indexes[indexes[base]]     = base - (nx + 1);
    indexes[indexes[base] + 1] = base - nx;
    indexes[indexes[base] + 2] = base + 1;

    for (size_t j = base + 1; j - base < nx; j++)
    {
      indexes[j] = loc_index_offset + (j - base)*4 - 1;
      indexes[indexes[j]]     = j - (nx + 1);
      indexes[indexes[j] + 1] = j - nx;
      indexes[indexes[j] + 2] = j - 1;
      indexes[indexes[j] + 3] = j + 1;
    }
    base = n - 1;
    indexes[base] = loc_index_offset + 4*nx - 1;
    indexes[indexes[base]]     = base - (nx + 1);
    indexes[indexes[base] + 1] = base - 1;
    stride--;

  }

  for (size_t i = start; i < start + stride; i++, index_offset += index_step)
  {
    size_t base = i*(nx + 1);
    indexes[base] = index_offset;
    indexes[indexes[base]]     = base - (nx + 1);
    indexes[indexes[base] + 1] = base - nx;
    indexes[indexes[base] + 2] = base + 1;
    indexes[indexes[base] + 3] = base + nx + 1;

    for (size_t j = base + 1; j - base < nx; j++)
    {
      indexes[j] = index_offset + (j - base)*6 - 2;
      indexes[indexes[j]]     = j - (nx + 1);
      indexes[indexes[j] + 1] = j - nx;
      indexes[indexes[j] + 2] = j - 1;
      indexes[indexes[j] + 3] = j + 1;
      indexes[indexes[j] + 4] = j + nx;
      indexes[indexes[j] + 5] = j + nx + 1;
    }
    base = (i + 1)*(nx + 1) - 1;
    indexes[base] = index_offset + index_step - 4;
    indexes[indexes[base]]     = base - (nx + 1);
    indexes[indexes[base] + 1] = base - 1;
    indexes[indexes[base] + 2] = base + nx;
    indexes[indexes[base] + 3] = base + nx + 1;
  }

  return 0;
}

void fill_gramm(msr &matr, size_t nx, size_t ny, int p, int thread, double a, double b, double c, double d)
{
  size_t stride, start;
  start_and_size(p, thread, ny + 1, start, stride);

  double hx = (b - a)/nx;
  double hy = (d - c)/ny;
  matr.norm = hx*hy;
  double *data = matr.data;
  size_t *indexes = matr.indexes;
  size_t n = matr.n;

  if (start == 0)
  {
    size_t j = 0;

    data[j]              = hx*hy/12.;
    data[indexes[j]]     = hx*hy/24.;
    data[indexes[j] + 1] = hx*hy/24.;

    for (j++; j < nx; j++)
    {
      data[j]              = hx*hy/4;
      data[indexes[j]]     = hx*hy/24.;
      data[indexes[j] + 1] = hx*hy/24.;
      data[indexes[j] + 2] = hx*hy/12.;
      data[indexes[j] + 3] = hx*hy/12.;
    }

    data[j]              = hx*hy/6.;
    data[indexes[j]]     = hx*hy/24.;
    data[indexes[j] + 1] = hx*hy/12.;
    data[indexes[j] + 2] = hx*hy/24.;

    stride--;
    start = 1;
  }

  if (start + stride == ny + 1)
  {
    size_t j = (nx + 1)*ny;

    data[n] = 0;

    data[j]              = hx*hy/6.;
    data[indexes[j]]     = hx*hy/24.;
    data[indexes[j] + 1] = hx*hy/12.;
    data[indexes[j] + 2] = hx*hy/24.;

    for (j++; j < (nx + 1)*(ny + 1) - 1; j++)
    {
      data[j]              = hx*hy/4;
      data[indexes[j]]     = hx*hy/12.;
      data[indexes[j] + 1] = hx*hy/12.;
      data[indexes[j] + 2] = hx*hy/24.;
      data[indexes[j] + 3] = hx*hy/24.;
    }

    data[j]              = hx*hy/12.;
    data[indexes[j]]     = hx*hy/24.;
    data[indexes[j] + 1] = hx*hy/24.;

    stride--;
  }

  //printf("thread = %d, start = %lu, stride = %lu\n", thread, start, stride);


  for (size_t i = start; i < start + stride; i++)
  {
    size_t j = i*(nx + 1);

    data[j]              = hx*hy/4;
    data[indexes[j]]     = hx*hy/24.;
    data[indexes[j] + 1] = hx*hy/12.;
    data[indexes[j] + 2] = hx*hy/12.;
    data[indexes[j] + 3] = hx*hy/24.;

    for (j++; j < (i + 1)*(nx + 1) - 1; j++)
    {
      data[j]              = hx*hy/2.;
      data[indexes[j]]     = hx*hy/12.;
      data[indexes[j] + 1] = hx*hy/12.;
      data[indexes[j] + 2] = hx*hy/12.;
      data[indexes[j] + 3] = hx*hy/12.;
      data[indexes[j] + 4] = hx*hy/12.;
      data[indexes[j] + 5] = hx*hy/12.;
    }

    data[j]              = hx*hy/4;
    data[indexes[j]]     = hx*hy/24.;
    data[indexes[j] + 1] = hx*hy/12.;
    data[indexes[j] + 2] = hx*hy/12.;
    data[indexes[j] + 3] = hx*hy/24.;
  }
}

#define F(I, J) (f(a + (I)*hx, b - (J)*hy))

double bprod(size_t i, size_t j, size_t nx, size_t ny, double a, double b, double hx, double hy, double f(double, double))
{
  double w = hx*hy/192;
  if (0 < i && 0 < j && i < nx && j < ny)
    return w * (36*F(i, j) + 20*(F(i + 0.5, j) + F(i + 0.5, j - 0.5) + F(i, j - 0.5) + F(i - 0.5, j) + F(i - 0.5, j + 0.5) + F(i, j + 0.5))
                + 4*(F(i + 1, j - 0.5) + F(i + 0.5, j + 1) + F(i - 0.5, j - 0.5) + F(i - 1, j + 0.5) + F(i - 0.5, j - 1) + F(i + 0.5, j + 0.5))
                + 2*(F(i + 1, j) + F(i + 1, j - 1) + F(i, j - 1) + F(i - 1, j) + F(i - 1, j + 1) + F(i, j + 1)));
  if (i == 0 && 0 < j && j < ny)
    return w * (18*F(i, j) + 20*(F(i + 0.5, j - 0.5) + F(i + 0.5, j)) + 10*(F(i, j + 0.5) + F(i, j - 0.5))
                + 4*(F(i + 0.5, j - 1) + F(i + 1, j - 0.5) + F(i + 0.5, j + 0.5)) + 2*(F(i + 1, j - 1) + F(i + 1, j)) + (F(i, j + 1) + F(i, j - 1)));
  if (i == nx && 0 < j && j < ny)
    return w * (18*F(i, j) + 20*(F(i - 0.5, j + 0.5) + F(i - 0.5, j)) + 10*(F(i, j - 0.5) + F(i, j + 0.5))
                + 4*(F(i - 0.5, j + 1) + F(i - 1, j + 0.5) + F(i - 0.5, j - 0.5)) + 2*(F(i - 1, j + 1) + F(i - 1, j)) + (F(i, j - 1) + F(i, j + 1)));
  if (0 < i && i < nx && j == 0)
    return w * (18*F(i, j) + 20*(F(i - 0.5, j + 0.5) + F(i, j + 0.5)) + 10*(F(i + 0.5, j) + F(i - 0.5, j))
                + 4*(F(i - 1, j + 0.5) + F(i - 0.5, j + 1) + F(i + 0.5, j + 0.5)) + 2*(F(i - 1, j + 1) + F(i, j + 1)) + (F(i + 1, j) + F(i - 1, j)));
  if (0 < i && i < nx && j == ny)
    return w * (18*F(i, j) + 20*(F(i + 0.5, j - 0.5) + F(i, j - 0.5)) + 10*(F(i - 0.5, j) + F(i + 0.5, j))
                + 4*(F(i + 1, j - 0.5) + F(i + 0.5, j - 1) + F(i - 0.5, j - 0.5)) + 2*(F(i + 1, j - 1) + F(i, j - 1)) + (F(i - 1, j) + F(i + 1, j)));
  if (i == 0 && j == 0)
    return w * (6*F(i, j) + 4*F(i + 0.5, j + 0.5) + 10*(F(i + 0.5, j) + F(i, j + 0.5)) + (F(i + 1, j) + F(i, j + 1)));
  if (i == nx && j == ny)
    return w * (6*F(i, j) + 4*F(i - 0.5, j - 0.5) + 10*(F(i - 0.5, j) + F(i, j - 0.5)) + (F(i - 1, j) + F(i, j - 1)));
  if (i == 0 && j == ny)
    return w * (12*F(i, j) + 10*(F(i + 0.5, j) + F(i, j - 0.5)) + 20*F(i + 0.5, j - 0.5) + 4*(F(i + 0.5, j - 1) + F(i + 1, j - 0.5)) + 2*F(i + 1, j - 1) + (F(i + 1, j) + F(i, j - 1)));
  if (i == nx && j == 0)
    return w * (12*F(i, j) + 10*(F(i - 0.5, j) + F(i, j + 0.5)) + 20*F(i - 0.5, j + 0.5) + 4*(F(i - 0.5, j + 1) + F(i - 1, j + 0.5)) + 2*F(i - 1, j + 1) + (F(i - 1, j) + F(i, j + 1)));
  return 0;
}

void fill_right_side(size_t nx, size_t ny, double *right, int p, int thread, double a, double b, double c, double d, double f(double, double))
{
  size_t stride, start;
  double hx = (b - a)/nx, hy = (d - c)/ny;
  start_and_size(p, thread, ny + 1, start, stride);

  for (size_t i = start; i < start + stride; i++)
    for (size_t j = 0; j <= nx; j++)
      right[i*(nx + 1) + j] = bprod(j, i, nx, ny, a, d, hx, hy, f);
}

