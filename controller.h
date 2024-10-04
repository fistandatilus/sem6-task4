#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <QObject>

#include "approximation.h"
#include "graph.h"


class Controller : public QObject {
    Q_OBJECT

private:
    approximation *approx = nullptr;
    bool ready = true;
    pthread_t *tid = nullptr;
    arguments *arg = nullptr;
    const char *argv0 = nullptr;

public:
    Controller(approximation *approx);
    ~Controller() = default;
    void set_argv0(const char *ptr) {argv0 = ptr;}

public slots:
    void calculate(arguments);

signals:
    void done(arguments);



};

#endif //CONTROLLER_H
