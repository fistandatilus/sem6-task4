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
    zoom *= 2;
    if (zoom > 1) zoom = 1;
    eval_y_max_min();
    update();
}

void Graph::shrink_segment()
{
    zoom *= 0.5;
    eval_y_max_min();
    update();
}

void Graph::enlarge_n()
{
    nx *= 2;
    ny *= 2;
    args.nx = nx;
    args.ny = ny;
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
    args.nx = nx;
    args.ny = ny;
    update_func();
    update();
}

void Graph::fluctuate_plus()
{
    p++;
    update_func();
    update();
}

void Graph::fluctuate_minus()
{
    p--;
    update_func();
    update();
}

void Graph::enlarge_m()
{
    mx *= 2;
    my *= 2;
    eval_y_max_min();
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
    eval_y_max_min();
    update();
}

void Graph::ready_approx(arguments args)
{
    if (args.stat != status::ok && args.stat != status::error_out_of_iterations)
    {
        printf("something went wrong\n");
        emit fatal_error();
        return;
    }
    printf("recieved correct approx\n");
    trivapp.init(f);
    diffapp.init(approx, f);
    eval_y_max_min();
    emit enable(true);
    update();
}

void Graph::set_enable(bool state) {
    enabled = state;
}
