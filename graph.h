
#ifndef GRAPH_H
#define GRAPH_H

#include <QtWidgets/QtWidgets>
#include "approximation.h"

class Graph : public QWidget
{
    Q_OBJECT

private:
    int func_id = 0;
    const char *f_name = nullptr;
    double a = -1;
    double b = 1;
    double c = -1;
    double d = 1;
    int n = 1;
    int m = 1;
    int mode = 0;
    int p = 0;
    double (*f)(double) = nullptr;
    double f_max = 0;
    double f_min = 0;
    TrivialApproximation trivapp;
    NewtonApproximation newtapp;
    BesselApproximation bessapp;

public:
    Graph(QWidget *parent);

    QSize minimumSizeHint() const;
    QSize sizeHint() const;

    int parse_command_line(int argc, char *argv[]);
    QPointF m2w(double x_m, double y_m);
    double x_w2m(double x_w);

public slots:
    void change_func();
    void change_mode();
    void enlarge_segment();
    void shrink_segment();
    void enlarge_n();
    void shrink_n();
    void fluctuate_plus();
    void flustuate_minus();
    void eval_y_max_min();

protected:
    void paintEvent(QPaintEvent *event);
    double paint_approx(Approximation &approx, QPainter &painter, Qt::GlobalColor color = Qt::black);
    void set_func(int id);
    void update_func();
};

#define DEFAULT_A -10
#define DEFAULT_B 10
#define DEFAULT_N 10
#define FUNCTION_AMOUNT 7
#define MODE_AMOUNT 4
#define VERY_SMALL_NUMBER 1e-100

#endif
