#include "msr.h"

int msr::copy(msr &x)
{
  if (copy_template(x))
	return 1;
  norm = x.norm;
  memcpy(data, x.data, size * sizeof(double));
  if (x.indexes != nullptr)
	memcpy(indexes, x.indexes, size * sizeof(size_t));
  return 0;
}


int msr::copy_template(msr &x)
{
  erase();
  n = x.n;
  size = x.size;
  if (x.data != nullptr)
  {
    data = new double[size];
    if (!data)
      return 1;
  }
  if (x.indexes != nullptr)
  {
    indexes = x.indexes;
  }
  return 0;
}

void msr::print(size_t p, FILE *fp)
{
  for (size_t i = 0; i < p && i < n; i++)
  {
    fprintf(fp, "%10lu : ", indexes[i]);
    for (size_t j = 0; j < indexes[i+1] - indexes[i]; j++)
      fprintf(fp, " %10lu", indexes[indexes[i] + j]);
    fprintf(fp, "\n%10.3e : ", data[i]);
    for (size_t j = 0; j < indexes[i+1] - indexes[i]; j++)
      fprintf(fp, " %10.3e", data[indexes[i] + j]);
    printf("\n\n");
  }
  printf("\n");
}

void print_vec(double *a, size_t n, size_t p, FILE *fp)
{
  size_t np = (n < p) ? n : p;
  for (size_t i = 0; i < np; i++)
    fprintf(fp, "%le ", a[i]);
  fprintf(fp, "\n");
}
