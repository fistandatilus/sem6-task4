#ifndef THREAD_RELATED_H
#define THREAD_RELATED_H

#include <stdio.h>
#include <pthread.h>
#include <time.h>
#include <sys/resource.h>
#include <sys/time.h>

double get_cpu_time();
double get_full_time();
void start_and_size(unsigned int p, unsigned int thread, size_t n, size_t &start, size_t &size);

enum class status
{
  ok,
  error_mem,
  error_data,
  error_precond,
  error_out_of_iterations,
};

template <class T>
void reduce_sum(int p, T *a = nullptr, size_t n = 0)
{
  static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
  static pthread_cond_t condvar_in = PTHREAD_COND_INITIALIZER;
  static pthread_cond_t condvar_out = PTHREAD_COND_INITIALIZER;
  static int threads_in = 0, threads_out = 0;
  static T *pres = nullptr;
  pthread_mutex_lock(&mutex);
  if (pres == nullptr)
  {
    pres = a;
  }
  else
  {
    for (size_t i = 0; i < n; i++)
    {
      pres[i] += a[i];
    }
  }
  threads_in++;
  if (threads_in >= p)
  {
    threads_out = 0;
    pthread_cond_broadcast(&condvar_in);
  }
  else
  {
    while (threads_in < p)
      pthread_cond_wait(&condvar_in, &mutex);
  }
  if (pres != a)
  {
    for (size_t i = 0; i < n; i++)
      a[i] = pres[i];
  }
  threads_out++;
  if (threads_out >= p)
  {
    pres = nullptr;
    threads_in = 0;
    pthread_cond_broadcast(&condvar_out);
  }
  else
    while (threads_out < p)
      pthread_cond_wait(&condvar_out, &mutex);
  pthread_mutex_unlock(&mutex);
}

template <class T>
void reduce_sum_consistent(int p, T *a = nullptr, size_t n = 0)
{
  static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
  static pthread_cond_t condvar_in = PTHREAD_COND_INITIALIZER;
  static pthread_cond_t condvar_out = PTHREAD_COND_INITIALIZER;
  static int threads_in = 0, threads_out = 0;
  static T *pres = nullptr;
  pthread_mutex_lock(&mutex);
  if (pres == nullptr)
  {
    pres = a;
  }
  else
  {
    for (size_t i = 0; i < n; i++)
    {
      pres[i] += a[i];
    }
  }
  threads_in++;
  if (threads_in >= p)
  {
    threads_out = 0;
    pthread_cond_broadcast(&condvar_in);
  }
  else
  {
    while (threads_in < p)
      pthread_cond_wait(&condvar_in, &mutex);
  }
  if (pres != a)
  {
    for (size_t i = 0; i < n; i++)
      a[i] = pres[i];
  }
  threads_out++;
  if (threads_out >= p)
  {
    pres = nullptr;
    threads_in = 0;
    pthread_cond_broadcast(&condvar_out);
  }
  else
    while (threads_out < p)
      pthread_cond_wait(&condvar_out, &mutex);
  pthread_mutex_unlock(&mutex);
}

template <class T>
void reduce_max(int p, T *a = nullptr, size_t n = 0)
{
  static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
  static pthread_cond_t condvar_in = PTHREAD_COND_INITIALIZER;
  static pthread_cond_t condvar_out = PTHREAD_COND_INITIALIZER;
  static int threads_in = 0, threads_out = 0;
  static T *pres = nullptr;
  pthread_mutex_lock(&mutex);
  if (pres == nullptr)
  {
    pres = a;
  }
  else
  {
    for (size_t i = 0; i < n; i++)
    {
      if (a[i] > pres[i])
      pres[i] = a[i];
    }
  }
  threads_in++;
  if (threads_in >= p)
  {
    threads_out = 0;
    pthread_cond_broadcast(&condvar_in);
  }
  else
  {
    while (threads_in < p)
      pthread_cond_wait(&condvar_in, &mutex);
  }
  if (pres != a)
  {
    for (size_t i = 0; i < n; i++)
      a[i] = pres[i];
  }
  threads_out++;
  if (threads_out >= p)
  {
    pres = nullptr;
    threads_in = 0;
    pthread_cond_broadcast(&condvar_out);
  }
  else
    while (threads_out < p)
      pthread_cond_wait(&condvar_out, &mutex);
  pthread_mutex_unlock(&mutex);
}

#endif //THREAD_RELATED_H
