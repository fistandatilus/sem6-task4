#include "approximation.h"

double approximation::residual1(int p, int thread)
{
    size_t start, stride;
    start_and_size(p, thread, ny, start, stride);
    double hx = (b - a)/nx, hy = (d - c)/ny;
    double max = 0;
    approximation &pf = *this;

    for (size_t j = start; j < start + stride; j++)
    {
        double y1 = d - (j + 1/3)*hy;
        double y2 = d - (j + 2/3)*hy;
        for (size_t i = 0; i < nx; i++)
        {
            double x1 = a + (i + 1/3)*hx;
            double x2 = a + (i + 2/3)*hx;
            double value1 = fabs(pf(x1, y1) -  f(x1, y1));
            double value2 = fabs(pf(x2, y2) -  f(x2, y2));
            if (value1 > max)
                max = value1;
            if (value2 > max)
                max = value2;
        }
    }
    reduce_max(p, &max, 1);
    return max;
}

double approximation::residual2(int p, int thread)
{
    size_t start, stride;
    start_and_size(p, thread, ny, start, stride);
    double hx = (b - a)/nx, hy = (d - c)/ny;
    double sum = 0;
    approximation &pf = *this;

    for (size_t j = start; j < start + stride; j++)
    {
        double y1 = d - (j + 1/3)*hy;
        double y2 = d - (j + 2/3)*hy;
        for (size_t i = 0; i < nx; i++)
        {
            double x1 = a + (i + 1/3)*hx;
            double x2 = a + (i + 2/3)*hx;
            double value1 = fabs(pf(x1, y1) -  f(x1, y1)) * hx * hy * 0.5;
            double value2 = fabs(pf(x2, y2) -  f(x2, y2)) * hx * hy * 0.5;
            sum += value1 + value2;
        }
    }
    reduce_sum(p, &sum, 1);
    return sum;
}

double approximation::residual3(int p, int thread)
{
    size_t start, stride;
    start_and_size(p, thread, ny + 1, start, stride);
    double hx = (b - a)/nx, hy = (d - c)/ny;
    double max = 0;
    approximation &pf = *this;

    for (size_t j = start; j < start + stride; j++)
    {
        double y = d - j*hy;
        for (size_t i = 0; i <= nx; i++)
        {
            double x = a + i*hx;
            double value = fabs(pf(x, y) -  f(x, y));
            if (value > max)
                max = value;
        }
    }
    reduce_max(p, &max, 1);
    return max;
}

double approximation::residual4(int p, int thread)
{
    size_t start, stride;
    start_and_size(p, thread, ny + 1, start, stride);
    double hx = (b - a)/nx, hy = (d - c)/ny;
    double sum = 0;
    approximation &pf = *this;

    for (size_t j = start; j < start + stride; j++)
    {
        double y = d - j*hy;
        for (size_t i = 0; i <= nx; i++)
        {
            double x = a + i*hx;
            double value = fabs(pf(x, y) -  f(x, y)) * hx * hy;
            sum += value;
        }
    }
    reduce_sum(p, &sum, 1);
    return sum;
}

void approximation::max_min(int p, int thread, double &max, double &min)
{
    size_t start, stride;
    start_and_size(p, thread, ny, start, stride);
    double hx = (b - a)/nx, hy = (d - c)/ny;
    approximation &pf = *this;
    max = this->operator()(a + hx/3, c + 2*hy/3);
    min = max;


    for (size_t j = start; j < start + stride; j++)
    {
        double y1 = d - (j + 1/3)*hy;
        double y2 = d - (j + 2/3)*hy;
        for (size_t i = 0; i < nx; i++)
        {
            double x1 = a + (i + 1/3)*hx;
            double x2 = a + (i + 2/3)*hx;
            double value1 = pf(x1, y1);
            double value2 = pf(x2, y2);
            if (value1 > max)
                max = value1;
            if (value2 > max)
                max = value2;
            if (value1 < min)
                min = value1;
            if (value2 < min)
                min = value2;
        }
    }

    min = -min;
    reduce_max(p, &max, 1);
    reduce_max(p, &min, 1);
    min = -min;
}

void approximation::max_min_f(int p, int thread, double &max, double &min)
{
    size_t start, stride;
    start_and_size(p, thread, ny, start, stride);
    double hx = (b - a)/nx, hy = (d - c)/ny;
    max = this->f(a + hx/3, c + 2*hy/3);
    min = max;


    for (size_t j = start; j < start + stride; j++)
    {
        double y1 = d - (j + 1/3)*hy;
        double y2 = d - (j + 2/3)*hy;
        for (size_t i = 0; i < nx; i++)
        {
            double x1 = a + (i + 1/3)*hx;
            double x2 = a + (i + 2/3)*hx;
            double value1 = f(x1, y1);
            double value2 = f(x2, y2);
            if (value1 > max)
                max = value1;
            if (value2 > max)
                max = value2;
            if (value1 < min)
                min = value1;
            if (value2 < min)
                min = value2;
        }
    }

    min = -min;
    reduce_max(p, &max, 1);
    reduce_max(p, &min, 1);
    min = -min;
}
