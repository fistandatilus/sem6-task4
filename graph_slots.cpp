#include "graph.h"

void Graph::change_func()
{
    set_func(func_id + 1);
}

void Graph::change_mode()
{
    mode = (mode + 1) % MODE_AMOUNT;
    eval_y_max_min();
    update();
}

void Graph::enlarge_segment()
{
    h_x *= 2;
    a = center_x - h_x;
    b = center_x + h_x;
    h_y *= 2;
    c = center_y - h_y;
    d = center_y + h_y;
    update_func();
    update();
}

void Graph::shrink_segment()
{
    h_x *= 0.5;
    a = center_x - h_x;
    b = center_x + h_x;
    h_y *= 0.5;
    c = center_y - h_y;
    d = center_y + h_y;
    update_func();
    update();
}

void Graph::enlarge_n()
{
    nx *= 2;
    ny *= 2;
    update_func();
    update();
}

void Graph::shrink_n()
{
    nx /= 2;
    if (nx < 1)
        nx = 1;
    ny /= 2;
    if (ny < 1)
        ny = 1;
    update_func();
    update();
}

void Graph::fluctuate_plus()
{
    p++;
    update_func();
    update();
}

void Graph::flustuate_minus()
{
    p--;
    update_func();
    update();
}

void Graph::enlarge_m()
{
    mx *= 2;
    my *= 2;
    update_func();
    update();
}

void Graph::shrink_m()
{
    mx /= 2;
    if (mx < 1)
        mx = 1;
    my /= 2;
    if (my < 1)
        my = 1;
    update_func();
    update();
}

void Graph::ready(approximation *approximation)
{
    approx = approximation;
    trivapp.init(f);
    diffapp.init(approx);
    update();
}
