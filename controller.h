#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <QObject>

#include "approximation.h"
#include "graph.h"


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
    void calculate(approximation *, arguments &);

signals:
    void ready(approximation *, arguments &);



};

#endif //CONTROLLER_H
