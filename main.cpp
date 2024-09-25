#include <pthread.h>
#include <fenv.h>
#include <sched.h>
#include <sys/sysinfo.h>


#include "approximation.h"
#include "functions.h"

void *thread_func(void *void_arg);

struct arguments
{
    approximation *approx;
    double a;
    double b;
    double c;
    double d;
    double eps;
    size_t nx;
    size_t ny;
    int max_it;
    int k;

    double r1;
    double r2;
    double r3;
    double r4;
    double t1;
    double t2;
    int it;
    status stat;

    int p;
    int thread;

    void set(approximation *approx, double a, double b, double c, double d, double eps, size_t nx, size_t ny, int max_it, int k, int p, int thread)
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
};

void do_threads(pthread_t *tid, arguments *arg, int p, const char *exe_name, int task);

int main(int argc, char *argv[])
{
  feenableexcept(FE_ALL_EXCEPT ^ FE_INEXACT);
  int p, max_it, k, task = 8;
  double eps, a, b, c, d;
  size_t nx, ny;

  if (!(argc == 11 && sscanf(argv[1], "%lf", &a) == 1 && sscanf(argv[2], "%lf", &b) == 1 && sscanf(argv[3], "%lf", &c) == 1 && sscanf(argv[4], "%lf", &d) == 1
        && sscanf(argv[5], "%lu", &nx) == 1 && nx > 0 && sscanf(argv[6], "%lu", &ny) == 1 && ny > 0 &&sscanf(argv[7], "%d", &k) == 1 && k >= 0 && k <= 7
        && sscanf(argv[8], "%lf", &eps) == 1 && eps > 0 && b - a > eps && d - c > eps && sscanf(argv[9], "%d", &max_it) == 1 && max_it >= 1 && sscanf(argv[10], "%d", &p) == 1 && p >= 1))
  {
    printf("Program usage: %s a b c d nx ny k eps max_it p\na b c d - ends of segments. their length should be larger than the precision\nnx ny - amounts of segments subdivisions\nk - function to approximate\neps - desired precision\nmax_it - maximum number of iterations\np - amount of threads to use\n", argv[0]);
    return -1;
  }

  pthread_t *tid = new pthread_t[p];
  arguments *arg = new arguments[p];
  if (tid && arg)
  {
    approximation approx;

    for (int i = 0; i < p; i++)
      arg[i].set(&approx, a, b, c, d, eps, nx, ny, max_it, k, p, i);
    do_threads(tid, arg, p, argv[0], task);
  }
  int ret = 0;
  if (tid)
    delete[] tid;
  else
    ret = -2;
  if (arg)
    delete[] arg;
  else
    ret = -2;
  return ret;
}

void do_threads(pthread_t *tid, arguments *arg, int p, const char *exe_name, int task)
{
  for (int i = 1; i < p; i++)
    if (pthread_create(tid+i, 0, thread_func, arg+i))
    {
      printf("Cannot create thread!\n");
      return;
    }
  thread_func(arg+0);

  for (int k = 1; k < p; k++)
    pthread_join(tid[k], 0);

  switch (arg->stat)
  {
  case status::ok :
    break;
  case status::error_out_of_iterations :
    printf("Limit of iterations reached!\n");
    break;
  case status::error_mem :
    printf("Cannot allocate memory!\n");
    return;
  case status::error_data :
    printf("Cannot approximate on this input\n");
    return;
  case status::error_precond :
    printf("Cannot form precomdtioner\n");
    return;
  }
  printf (
        "%s : Task = %d R1 = %e R2 = %e R3 = %e R4 = %e T1 = %.2f T2 = %.2f\
        It = %d E = %e K = %d Nx = %d Ny = %d P = %d\n",
      exe_name, task, arg->r1, arg->r2, arg->r3, arg->r4, arg->t1, arg->t2, arg->it, arg->eps, arg->k, (int)arg->nx, (int)arg->ny, arg->p);


}

void *thread_func(void *void_arg)
{
  arguments *arg = (arguments *)void_arg;

  approximation *approx = arg->approx;
  double a = arg->a;
  double b = arg->b;
  double c = arg->c;
  double d = arg->d;
  double eps = arg->eps;
  size_t nx = arg->nx;
  size_t ny = arg->ny;
  int max_it = arg->max_it;
  int k = arg->k;

  double &t1 = arg->t1;
  double &t2 = arg->t2;
  double &r1 = arg->r1;
  double &r2 = arg->r2;
  double &r3 = arg->r3;
  double &r4 = arg->r4;
  status &stat = arg->stat;
  int &it = arg->it;

  int p = arg->p;
  int thread = arg->thread;
  cpu_set_t cpu;
  CPU_ZERO(&cpu);
  int nproc = get_nprocs();
  int cpu_id = nproc - 1 - thread%nproc;
  CPU_SET(cpu_id, &cpu);
  pthread_t tid = pthread_self();
  pthread_setaffinity_np(tid, sizeof(cpu_set_t), &cpu);

  reduce_sum<double>(p);
  t1 = get_full_time();
  stat = approx->init(a, b, c, d, nx, ny, eps, p, thread);
  if (stat != status::ok)
    return nullptr;
  double (*f) (double, double);
  set_func(&f, k);
  stat = approx->init_function(f, max_it, it, eps, p, thread);
  reduce_sum<double>(p);
  t2 = get_full_time();
  t1 = t2 - t1;
  r1 = approx->residual1(p, thread);
  r2 = approx->residual2(p, thread);
  r3 = approx->residual3(p, thread);
  r4 = approx->residual4(p, thread);
  reduce_sum<double>(p);
  t2 = get_full_time() - t2;
  return nullptr;
}
