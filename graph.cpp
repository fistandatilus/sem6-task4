
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
    n = DEFAULT_N;
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
    int k;
    if (argc < 5 || sscanf(argv[1], "%le", &a) != 1 || sscanf(argv[2], "%le", &b) != 1
        || b - a < 1.e-20 || sscanf(argv[3], "%d", &n) != 1 || sscanf(argv[4], "%d", &k) != 1
        || n <= 0) {
        printf("Usage: %s a b n k\na, b - left and right ends of the segment\nn - amount of points "
               "to interpolate by\nk - which function to use\n",
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
    double y_w = (y_max - y_m) / (fabs(y_max - y_min) > VERY_SMALL_NUMBER ? y_max - y_min : 1)
                 * height();
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

    // draw axis
    QPen pen_red(Qt::red, 0, Qt::SolidLine);
    painter.setPen(pen_red);
    painter.drawLine(m2w(a, 0), m2w(b, 0));
    painter.drawLine(m2w(0, y_min), m2w(0, y_max));

    //draw functions
    double max;
    switch (mode) {
    case 0:
        printf("maximum of |f|:");
        max = paint_approx(trivapp, painter, Qt::black);
        painter.setPen(Qt::blue);
        painter.drawText(0, 30, QString::asprintf("maximum of |f| = %le", max));
        printf("maximum of newton:");
        max = paint_approx(newtapp, painter, Qt::blue);
        painter.setPen(Qt::blue);
        painter.drawText(0, 45, QString::asprintf("maximum of newton = %le", max));
        break;
    case 1:
        printf("maximum of |f|:");
        max = paint_approx(trivapp, painter, Qt::black);
        painter.setPen(Qt::blue);
        painter.drawText(0, 30, QString::asprintf("maximum of |f| = %le", max));
        printf("maximum of bessel:");
        max = paint_approx(bessapp, painter, Qt::darkCyan);
        painter.setPen(Qt::darkCyan);
        painter.drawText(0, 45, QString::asprintf("maximum of bessel = %le", max));
        break;
    case 2:
        printf("maximum of |f|:");
        max = paint_approx(trivapp, painter, Qt::black);
        painter.setPen(Qt::blue);
        painter.drawText(0, 30, QString::asprintf("maximum of |f| = %le", max));
        printf("maximum of newton:");
        max = paint_approx(newtapp, painter, Qt::blue);
        painter.setPen(Qt::blue);
        painter.drawText(0, 45, QString::asprintf("maximum of newton = %le", max));
        printf("maximum of bessel:");
        max = paint_approx(bessapp, painter, Qt::darkCyan);
        painter.setPen(Qt::darkCyan);
        painter.drawText(0, 60, QString::asprintf("maximum of bessel = %le", max));
        break;
    case 3:
        DifferenceApproximation diff;
        if (n <= 50) {
            diff.init(&trivapp, &newtapp);
            printf("maximum of newton:");
            max = paint_approx(diff, painter, Qt::blue);
            painter.setPen(Qt::blue);
            painter.drawText(0, 30, QString::asprintf("maximum of newton = %le", max));
        }
        diff.init(&trivapp, &bessapp);
        printf("maximum of bessel:");
        max = paint_approx(diff, painter, Qt::darkCyan);
        painter.setPen(Qt::darkCyan);
        painter.drawText(0, 45, QString::asprintf("maximum of bessel = %le", max));

        break;
    }
    // render function name
    painter.setPen("blue");
    painter.drawText(0,
                     15,
                     QString::asprintf("%s n = %d, a = %.2e b = %.2e, p = %d, mode = %d",
                                       f_name,
                                       n,
                                       a,
                                       b,
                                       p,
                                       mode));
}

double Graph::paint_approx(Approximation &approx, QPainter &painter, Qt::GlobalColor color)
{
    QPen pen_black(color, 0, Qt::SolidLine);
    painter.setPen(pen_black);
    double x_1, x_2, y_1, y_2, max;
    x_1 = x_w2m(0);
    y_1 = approx(x_1);
    max = fabs(y_1);
    for (int i = 1; i < width(); i++) {
        x_2 = x_w2m(i);
        y_2 = approx(x_2);
        painter.drawLine(m2w(x_1, y_1), m2w(x_2, y_2));
        if (max < fabs(y_2))
            max = fabs(y_2);
        x_1 = x_2;
        y_1 = y_2;
    }
    printf("%le\n", max);
    return max;
}
