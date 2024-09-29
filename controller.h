#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <QObject>

#include "approximation.h"
#include "graph.h"

struct arguments
{
    approximation *approx;
    double a;
    double b;
    double c;
    double d;
    double eps;
    size_t nx;
    size_t ny;
    int max_it;
    int k;

    double r1;
    double r2;
    double r3;
    double r4;
    double max;
    double min;
    double t1;
    double t2;
    int it;
    status stat;

    int p;
    int thread;

    void set(approximation *approx,
             double a,
             double b,
             double c,
             double d,
             double eps,
             size_t nx,
             size_t ny,
             int max_it,
             int k,
             int p,
             int thread)
    {
        this->approx = approx;
        this->a = a;
        this->b = b;
        this->c = c;
        this->d = d;
        this->eps = eps;
        this->nx = nx;
        this->ny = ny;
        this->max_it = max_it;
        this->k = k;
        this->p = p;
        this->thread = thread;
    }
};

class Controller : public QObject {
    Q_OBJECT

    enum class State
    {
        standby,
        evaluating,
    };
private:
    approximation *approx = nullptr;
    State state = State::standby;
    pthread_t *tid = nullptr;
    arguments *arg = nullptr;

public:
    Controller(Graph *parent);

public slots:
    void calculate(approximation *);

signals:
    void ready(approximation *);



};

#endif //CONTROLLER_H
