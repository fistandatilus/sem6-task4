
#include <QPainter>
#include <math.h>
#include <stdio.h>

#include "graph.h"

static double f_0(double /* x */)
{
    return 1.;
}

static double f_1(double x)
{
    return x;
}

static double f_2(double x)
{
    return x * x;
}

static double f_3(double x)
{
    return x * x * x;
}

static double f_4(double x)
{
    return x * x * x * x;
}

static double f_5(double x)
{
    return exp(x);
}

static double f_6(double x)
{
    return 1 / (25 * x * x + 1);
}

Graph::Graph(QWidget *parent)
    : QWidget(parent)
{
    a = DEFAULT_A;
    b = DEFAULT_B;
    nx = DEFAULT_N;
    ny = DEFAULT_N;
    mx = DEFAULT_N;
    my = DEFAULT_N;
    mode = 0;

    func_id = 0;
}

QSize Graph::minimumSizeHint() const
{
    return QSize(100, 100);
}

QSize Graph::sizeHint() const
{
    return QSize(1000, 1000);
}

int Graph::parse_command_line(int argc, char *argv[])
{
    int k, max_it, threads;
    double eps;
    if (!(argc == 11 && sscanf(argv[1], "%lf", &a) == 1 && sscanf(argv[2], "%lf", &b) == 1
          && sscanf(argv[3], "%lf", &c) == 1 && sscanf(argv[4], "%lf", &d) == 1
          && sscanf(argv[5], "%lu", &nx) == 1 && nx > 0 && sscanf(argv[6], "%lu", &ny) == 1 && ny > 0
          && sscanf(argv[7], "%lu", &mx) == 1 && mx > 0 && sscanf(argv[8], "%lu", &my) == 1 && my > 0
          && sscanf(argv[9], "%d", &k) == 1 && k >= 0 && k <= 7 && sscanf(argv[10], "%lf", &eps) == 1
          && eps > 0 && b - a > eps && d - c > eps && sscanf(argv[11], "%d", &max_it) == 1
          && max_it >= 1 && sscanf(argv[12], "%d", &threads) == 1 && threads >= 1)) {
        printf("Program usage: %s a b c d nx ny mx my k eps max_it p\na b c d - ends of segments. their "
               "length should be larger than the precision\nnx ny - amounts of segments "
               "subdivisions\nmx my - visualization subdivizions\n"
               "k - function to approximate\neps - desired precision\nmax_it - "
               "maximum number of iterations\np - amount of threads to use\n",
               argv[0]);
        return -1;
    }

    set_func(k);

    return 0;
}

/// change current function for drawing
void Graph::set_func(int id)
{
    func_id = (id) % FUNCTION_AMOUNT;

    switch (func_id) {
    case 0:
        f_name = "k = 0 f (x) = 1";
        f = f_0;
        break;
    case 1:
        f_name = "k = 1 f (x) = x";
        f = f_1;
        break;
    case 2:
        f_name = "k = 2 f (x) = x*x";
        f = f_2;
        break;
    case 3:
        f_name = "k = 3 f (x) = x*x*x";
        f = f_3;
        break;
    case 4:
        f_name = "k = 4 f (x) = x*x*x*x";
        f = f_4;
        break;
    case 5:
        f_name = "k = 5 f (x) = e^x";
        f = f_5;
        break;
    case 6:
        f_name = "k = 6 f (x) = 1/(25*x*x + 1)";
        f = f_6;
        break;
    }
    update_func();
    update();
}

void Graph::update_func()
{
    trivapp.init(f);
    newtapp.init(a, b, n, p, f);
    bessapp.init(a, b, n, p, f);

    eval_y_max_min();
}

void Graph::eval_y_max_min()
{
    y_max = mode == 3 ? 0 : trivapp(a), y_min = y_max;

    //evaluating y_max and y_min
    for (int i = 0; i < width(); i++) {
        double triv, newt, bess;
        switch (mode) {
        case 0:
            triv = trivapp(x_w2m(i));
            newt = n > 50 ? triv : newtapp(x_w2m(i));
            if (triv > y_max)
                y_max = triv;
            if (newt > y_max)
                y_max = newt;
            if (triv < y_min)
                y_min = triv;
            if (newt < y_min)
                y_min = newt;
            break;
        case 1:
            triv = trivapp(x_w2m(i));
            bess = bessapp(x_w2m(i));
            if (triv > y_max)
                y_max = triv;
            if (bess > y_max)
                y_max = bess;
            if (triv < y_min)
                y_min = triv;
            if (bess < y_min)
                y_min = bess;
            break;
        case 2:
            triv = trivapp(x_w2m(i));
            newt = n > 50 ? triv : newtapp(x_w2m(i));
            bess = bessapp(x_w2m(i));
            if (triv > y_max)
                y_max = triv;
            if (newt > y_max)
                y_max = newt;
            if (bess > y_max)
                y_max = bess;
            if (triv < y_min)
                y_min = triv;
            if (newt < y_min)
                y_min = newt;
            if (bess < y_min)
                y_min = bess;
            break;
        case 3:
            triv = trivapp(x_w2m(i));
            newt = n > 50 ? 0 : newtapp(x_w2m(i)) - triv;
            bess = bessapp(x_w2m(i)) - triv;
            if (newt > y_max)
                y_max = newt;
            if (bess > y_max)
                y_max = bess;
            if (newt < y_min)
                y_min = newt;
            if (bess < y_min)
                y_min = bess;
            break;
        }
    }
}

//math coords to window coords
QPointF Graph::m2w(double x_m, double y_m)
{
    double x_w = (x_m - a) / (b - a) * width();
    double y_w = (y_m - c) / (d - c) * height();
    return QPointF(x_w, y_w);
}

//window coord to math coord
double Graph::x_w2m(double x_w)
{
    return x_w / width() * (b - a) + a;
}

/// render graph
void Graph::paintEvent(QPaintEvent * /* event */)
{
    QPainter painter(this);

    //draw functions
    double max;
    QString label;
    switch (mode) {
    case 0:
        max = fabs(max_f) > fabs(min_f) ? fabs(max_f) : fabs(min_f);
        paint_approx(trivapp, painter, Qt::black);
        break;
    case 1:
        max = fabs(max_approx) > fabs(min_approx) ? fabs(max_approx) : fabs(min_approx);
        paint_approxrox(approxroximation, painter, Qt::black);
        break;
    case 2:
        max = fabs(max_f - max_approx) > fabs(min_f - min_approx) ? fabs(max_f - max_approx) : fabs(min_f - min_approx);
        paint_approxrox(diffapprox, painter, Qt::black);
        break;
    }
    printf("maximum of |f| = %le ", max);
    label.append(QString::asprintf("maximum of |f| = %le", max));
    // render function name
    painter.setPen("blue");
    label.append(QString::asprintf("%s nx = %lu, ny = %lu a = %.2e b = %.2e, p = %d, mode = %d",
                                       f_name, nx, ny,  a, b, p, mode));
    emit set_label(label);
}

QColor color_maker(double a) {
    unsigned int r = ((unsigned int)(2*a - 1)) * 255;
    unsigned int g = ((unsigned int)(1 - fabs(2*a - 1))) * 255;
    unsigned int b = ((unsigned int)(1 - 2*a)) * 255;

    return QColor(r, g, b);
}

void Graph::paint_approx(Paintable &approx, QPainter &painter)
{
    QPointF triangle[3];
    double hx = (b - a) / mx;
    double hy = (d - c) / my;
    for (size_t i = 0; i < mx; i++)
        for (size_t j = 0; j < my; j++) {
            triangle[0] = m2w(a + hx*i, c + hy*j);
            triangle[1] = m2w(a + hx*(i + 1), c + hy*j);
            triangle[2] = m2w(a + hx*(i + 1), c + hy*(j + 1));

            double value;
            value = (approx(a + hx*(i + 2/3), c + hy*(j + 1/3)) - min_approx)/(max_approx - min_approx);
            QColor color = color_maker(value);
            QBrush brush(color);
            painter.setBrush(brush);

            painter.drawConvexPolygon(triangle, 3);

            triangle[1] = m2w(a + hx*i, c + hy*(j + 1));
            value = (approx(a + hx*(i + 1/3), c + hy*(j + 2/3)) - min_approx)/(max_approx - min_approx);
            color = color_maker(value);
            brush.setColor(color);
            painter.setBrush(brush);

            painter.drawConvexPolygon(triangle, 3);
        }
}
