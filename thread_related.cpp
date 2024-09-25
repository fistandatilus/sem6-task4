#include "thread_related.h"

double get_cpu_time()
{
  rusage buf;
  getrusage(RUSAGE_THREAD, &buf);
  return buf.ru_utime.tv_sec + buf.ru_utime.tv_usec*1e-6;
}

double get_full_time()
{
  timeval buf;
  gettimeofday(&buf, 0);
  return buf.tv_sec + buf.tv_usec * 1e-6;
}

void start_and_size(unsigned int p, unsigned int thread, size_t n, size_t &start, size_t &size)
{
  size = n/p;
  size_t res = n - size*p;
  start = size*thread + (thread < res ? thread : res);
  if (thread < res) size++;

}
