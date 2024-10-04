
#include <QPainter>
#include <math.h>
#include <stdio.h>

#include "graph.h"
#include "functions.h"
#include "controller.h"

Graph::Graph(QWidget *parent)
    : QWidget(parent)
{
    a = DEFAULT_A;
    b = DEFAULT_B;
    c = DEFAULT_A;
    d = DEFAULT_B;
    zoom = 1;
    nx = DEFAULT_N;
    ny = DEFAULT_N;
    mx = DEFAULT_N;
    my = DEFAULT_N;
    mode = 0;

    func_id = 0;
    approx = nullptr;
    connect(this, SIGNAL(enable(bool)), this, SLOT(set_enable(bool)));
}

QSize Graph::minimumSizeHint() const
{
    return QSize(100, 100);
}

QSize Graph::sizeHint() const
{
    return QSize(1000, 1000);
}

status Graph::parse_command_line(int argc, char *argv[])
{
    int k, max_it, threads;
    double eps;
    if (!(argc == 13 && sscanf(argv[1], "%lf", &a) == 1 && sscanf(argv[2], "%lf", &b) == 1
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
        return status::error_data;
    }

    if (!approx)
        approx = new approximation;
    if (!approx)
    {
        printf("Cannot allocate memory\n");
        return status::error_mem;
    }
    Controller *contr = new Controller(approx);
    contr->moveToThread(&controller_thread);
    contr->set_argv0(argv[0]);
    connect(&controller_thread, &QThread::finished, contr, &QObject::deleteLater);
    connect(this, &Graph::calculate, contr, &Controller::calculate, Qt::QueuedConnection); 
    connect(contr, &Controller::done, this, &Graph::ready_approx, Qt::QueuedConnection);
    controller_thread.start();
    args.set(
        approx,
        a, b, c, d,
        eps, nx, ny, 
        max_it, k,
        threads, 0, error
    );
    set_func(k);

    return status::ok;
}

/// change current function for drawing
void Graph::set_func(int id)
{
    func_id = (id) % FUNCTION_AMOUNT;

    switch (func_id) {
    case 0:
        f_name = "k = 0 f (x) = 1";
        f = f0;
        break;
    case 1:
        f_name = "k = 1 f (x) = x";
        f = f1;
        break;
    case 2:
        f_name = "k = 2 f (x) = y";
        f = f2;
        break;
    case 3:
        f_name = "k = 3 f (x) = x+y";
        f = f3;
        break;
    case 4:
        f_name = "k = 4 f (x) = sqrt(x^2 + y^2)";
        f = f4;
        break;
    case 5:
        f_name = "k = 5 f (x) = x^2 + y^2";
        f = f5;
        break;
    case 6:
        f_name = "k = 6 f (x) = e^(x^2 - y^2)";
        f = f6;
        break;
    case 7:
        f_name = "k = 7 f (x) = 1/(25*(x^2 +y^2) + 1)";
        f = f7;
        break;
    }
    update_func();
    update();
}

void Graph::update_func()
{ 
    emit enable(false);
    args.k = func_id;
    args.error = error;
    emit calculate(args);
}

void Graph::eval_y_max_min()
{
    approximation &pf = *approx;
    DifferenceApproximation &df = diffapp;
    double a = this->a;
    double b = this->b;
    double c = this->c;
    double d = this->d;
    double center_x = (a + b) * 0.5;
    double center_y = (c + d) * 0.5;
    double len_x = (b - a) * 0.5 * zoom;
    double len_y = (d - c) * 0.5 * zoom;
    a = center_x - len_x;
    b = center_x + len_x;
    c = center_y - len_y;
    d = center_y + len_y;
    draw_a = a;
    draw_b = b;
    draw_c = c;
    draw_d = d;

    double hx = (b - a)/mx;
    double hy = (d - c)/my;
    max[0] = f(a + hx/3, c + 2*hy/3);
    max[1] = pf(a + hx/3, c + 2*hy/3);
    max[2] = df(a + hx/3, c + 2*hy/3);

    for (int i = 0; i < MODE_AMOUNT; i++) {
        min[i] = max[i];
    }
    double value[MODE_AMOUNT];
    for (size_t i = 0; i < mx; i++)
        for (size_t j = 0; j < my; j++) {
            for (int k = 0; k < 2; k++) {
                double x = a + i*hx + hx/3 + k*hx/3;
                double y = c + j*hy + 2*hy/3 - k*hy/3;
                value[0] = f(x, y);
                value[1] = pf(x, y);
                value[2] = df(x, y);
                for (int l = 0; l < MODE_AMOUNT; l++)
                {
                    if (value[l] > max[l])
                        max[l] = value[l];
                    else if (value[l] < min[l])
                        min[l] = value[l];
                }
            }
        }
}

//math coords to window coords
QPointF Graph::m2w(double x_m, double y_m)
{
    double x_w = (x_m - draw_a) / (draw_b - draw_a) * width();
    double y_w = (draw_d - y_m) / (draw_d - draw_c) * height();
    return QPointF(x_w, y_w);
}

/// render graph
void Graph::paintEvent(QPaintEvent * /* event */)
{
    QPainter painter(this);
    QString label;
    if(!enabled)
    {
        emit set_label("Calculating approximation");
        return;
    }

    //draw functions
    double max;
    max = fabs(this->max[mode]) > fabs(this->min[mode]) ? fabs(this->max[mode]) : fabs(this->min[mode]);
    switch (mode) {
    case 0:
        paint_approx(trivapp, painter);
        break;
    case 1:
        paint_approx(*approx, painter);
        break;
    case 2:
        paint_approx(diffapp, painter);
        break;
    }
    printf("maximum of |f| = %le \n", max);
    label.append(QString::asprintf("maximum of |f| = %le ", max));
    // render function name
    label.append(QString::asprintf("%s nx = %lu, ny = %lu, mx = %lu, my = %lu\n a = %.2e b = %.2e, c = %.2e, d = %.2e, p = %d, mode = %d",
                                       f_name, nx, ny, mx, my, draw_a, draw_b, draw_c, draw_d, error, mode));
    emit set_label(label);
}

double clamp(double x, double a = 0, double b = 1);

double clamp(double x, double a, double b) {
    if (x < a)
        return a;
    if (x > b)
        return b;
    return x;
}

QColor color_maker(double a) {
    unsigned int r = clamp(2*a - 1) * 255;
    unsigned int g = clamp(1 - fabs(2*a - 1))* 255;
    unsigned int b = clamp(1 - 2*a) * 255;

    return QColor(r, g, b);
}

void Graph::paint_approx(Paintable &approx, QPainter &painter)
{
    painter.setPen(Qt::NoPen);
    QPointF triangle[3];
    size_t mx = this->mx < (size_t) width() ? this->mx : width();
    size_t my = this->my < (size_t) height() ? this->my : height();
    double a = draw_a;
    double b = draw_b;
    double c = draw_c;
    double d = draw_d;
    double hx = (b - a) / mx;
    double hy = (d - c) / my;
    double norm = max[mode]- min[mode];
    norm = norm > VERY_SMALL_NUMBER ? norm : 1;
    for (size_t i = 0; i < mx; i++)
        for (size_t j = 0; j < my; j++) {
            triangle[0] = m2w(a + hx*i, c + hy*j);
            triangle[1] = m2w(a + hx*(i + 1), c + hy*j);
            triangle[2] = m2w(a + hx*(i + 1), c + hy*(j + 1));

            double value;
            value = (approx(a + hx*(i + 2./3), c + hy*(j + 1./3)) - min[mode])/norm;
            QColor color = color_maker(value);
            QBrush brush(color);
            painter.setBrush(brush);

            painter.drawConvexPolygon(triangle, 3);

            triangle[1] = m2w(a + hx*i, c + hy*(j + 1));
            value = (approx(a + hx*(i + 1./3), c + hy*(j + 2./3)) - min[mode])/norm;     
            color = color_maker(value);
            brush.setColor(color);
            painter.setBrush(brush);

            painter.drawConvexPolygon(triangle, 3);
        }
}
