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
    a *= 2;
    b *= 2;
    update_func();
    update();
}

void Graph::shrink_segment()
{
    a /= 2;
    b /= 2;
    update_func();
    update();
}

void Graph::enlarge_n()
{
    n *= 2;
    update_func();
    update();
}

void Graph::shrink_n()
{
    n /= 2;
    if (n < 2)
        n = 2;
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
