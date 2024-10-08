#include <pthread.h>
#include <sched.h>
#include <sys/sysinfo.h>

#include "controller.h"
#include "functions.h"

void do_threads(pthread_t *tid, arguments *arg, int p, const char *exe_name, int task);
void *thread_func(void *void_arg);

Controller::Controller(approximation *approx) {
    this->approx = approx;
    arg = nullptr;
    ready = true;
    tid = nullptr;
}

void Controller::calculate( arguments args) {
    if (!ready)
        return;
    ready = false;
    int p = args.p;
    int max_it = args.max_it;
    int k = args.k, task = 8;
    int error = args.error;
    double eps = args.eps;
    double a = args.a;
    double b = args.b;
    double c = args.c;
    double d = args.d;
    size_t nx = args.nx, ny = args.ny;
    tid = new pthread_t[p];
    arg = new arguments[p];

    if (tid && arg) {
        for (int i = 0; i < p; i++)
            arg[i].set(approx, a, b, c, d, eps, nx, ny, max_it, k, p, i, error);
        do_threads(tid, arg, p, argv0, task);
    }
    args.stat = arg->stat;
    int ret = 0;
    if (tid)
        delete[] tid;
    else
        ret = -2;
    if (arg)
        delete[] arg;
    else
        ret = -2;
    if (ret) {
        printf("Cannot allocate memory!\n");
        args.stat = status::error_mem;
    }
    emit done(args);
    ready = true;
}

void do_threads(pthread_t *tid, arguments *arg, int p, const char *exe_name, int task)
{
    for (int i = 1; i < p; i++)
        if (pthread_create(tid + i, 0, thread_func, arg + i)) {
            printf("Cannot create thread!\n");
            return;
        }
    thread_func(arg + 0);

    for (int k = 1; k < p; k++)
        pthread_join(tid[k], 0);

    switch (arg->stat) {
    case status::ok:
        break;
    case status::error_out_of_iterations:
        printf("Limit of iterations reached!\n");
        break;
    case status::error_mem:
        printf("Cannot allocate memory!\n");
        return;
    case status::error_data:
        printf("Cannot approximate on this input\n");
        return;
    case status::error_precond:
        printf("Cannot form precomdtioner\n");
        return;
    }
    printf("%s : Task = %d R1 = %e R2 = %e R3 = %e R4 = %e T1 = %.2f T2 = %.2f\
        It = %d E = %e K = %d Nx = %d Ny = %d P = %d\n",
           exe_name,
           task,
           arg->r1,
           arg->r2,
           arg->r3,
           arg->r4,
           arg->t1,
           arg->t2,
           arg->it,
           arg->eps,
           arg->k,
           (int) arg->nx,
           (int) arg->ny,
           arg->p);
}

void *thread_func(void *void_arg)
{
    arguments *arg = (arguments *) void_arg;

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
    int error = arg->error;

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
    int cpu_id = nproc - 1 - thread % nproc;
    CPU_SET(cpu_id, &cpu);
    pthread_t tid = pthread_self();
    pthread_setaffinity_np(tid, sizeof(cpu_set_t), &cpu);

    reduce_sum<double>(p);
    t1 = get_full_time();
    stat = approx->init(a, b, c, d, nx, ny, eps, p, thread, error);
    if (stat != status::ok)
        return nullptr;
    double (*f)(double, double);
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
