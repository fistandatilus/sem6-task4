#include "approximation.h"

status approximation::init(double a, double b, double c, double d, size_t nx, size_t ny, double eps, int p, int thread)
{
  size_t n = (nx + 1) * (ny + 1);
  size_t size = size_by_nx_ny(nx, ny);
  if (thread == 0)
  {
    erase();
    this->a = a;
    this->b = b;
    this->c = c;
    this->d = d;
    this->nx = nx;
    this->ny = ny;
    coeffs = new double[n];
    v = new double[n];
    u = new double[n];
    r = new double[n];
    diag = new double[n];
    right_side = new double[n];
   // printf("i should've done this\n");
   // printf("1thread = %d, coeffs = %lu, v = %lu, u = %lu, r = %lu, diag = %lu, right_side = %lu\n", thread, (size_t)coeffs, (size_t)v, (size_t)u, (size_t)r, (size_t)diag, (size_t)right_side);
  }
  //printf("2thread = %d, coeffs = %lu, v = %lu, u = %lu, r = %lu, diag = %lu, right_side = %lu\n", thread, (size_t)coeffs, (size_t)v, (size_t)u, (size_t)r, (size_t)diag, (size_t)right_side);
  reduce_sum<double>(p);
  //printf("3thread = %d, coeffs = %lu, v = %lu, u = %lu, r = %lu, diag = %lu, right_side = %lu\n", thread, (size_t)coeffs, (size_t)v, (size_t)u, (size_t)r, (size_t)diag, (size_t)right_side);

  int ret = init_gramm_struct(nx, ny, p, thread, &msr_template);
  if (ret || !coeffs || !v || !u || !r || !diag || !right_side)
  {
    if (thread == 0)
      erase();
    if (ret)
      printf("gramm_struct\n");
    else
      printf("cannot alloc ryly\n");
    return status::error_mem;
  }

  if (thread == 0)
  {
    ret = gramm.set_template(msr_template, n, size);
  }
  reduce_sum(p, &ret, 1);

  if (ret)
  {
    if (thread == 0)
      erase();

    printf("template\n");
    return status::error_mem;
  }

  fill_gramm(gramm, nx, ny, p, thread, a, b, c, d);
//  if (thread == 0)
//    gramm.print(n);


  reduce_sum<int>(p);

  ret = form_preconditioner(gramm, precond, diag, eps, p, thread);

  if (ret)
  {
    if (thread == 0)
      erase();
    return status::error_precond;
  }
  return status::ok;
}

status approximation::init_function(double (*f)(double, double), int max_it, int &it, double eps, int p, int thread)
{
  fill_right_side(nx, ny, right_side, p, thread, a, b, c, d, f);
//  if (thread == 0)
//    print_vec(right_side, (nx+1)*(ny+1), (nx+1)*(ny+1));
  this->f = f;
  reduce_sum<double>(p);
  int ret = solve(gramm, right_side, precond, diag, coeffs, r, u, v, eps, p, thread, max_it, it);
//  if (thread == 0)
//    print_vec(coeffs, (nx+1)*(ny+1), (nx+1)*(ny+1));
  if(ret)
    return status::error_out_of_iterations;
  return status::ok;

}
