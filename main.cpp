#include <QApplication>
#include <QLabel>
#include <QMainWindow>
#include <fenv.h>

#include "controller.h"
#include "graph.h"

int main(int argc, char *argv[])
{
    feenableexcept(FE_ALL_EXCEPT ^ FE_INEXACT);
    QApplication app(argc, argv);

    QMainWindow *window = new QMainWindow;
    QMenuBar *tool_bar = new QMenuBar(window);
    QStatusBar *status_bar = new QStatusBar(window);
    QLabel *label = new QLabel(status_bar);
    Graph *graph = new Graph(window);
    Controller *controller = new Controller(graph);

    QAction *action;

    action = tool_bar->addAction ("&Change function", graph, SLOT (change_func ()));
    action->setShortcut (QString ("0")); 
    QObject::connect(graph, SIGNAL(Graph::enable(bool)), action, SLOT(QAction::setEnabled(bool)));

    action = tool_bar->addAction ("&Change mode", graph, SLOT (change_mode ()));
    action->setShortcut (QString ("1")); 
    QObject::connect(graph, SIGNAL(Graph::enable(bool)), action, SLOT(QAction::setEnabled(bool)));


    action = tool_bar->addAction ("&Wider", graph, SLOT (enlarge_segment ()));
    action->setShortcut (QString ("2")); 
    QObject::connect(graph, SIGNAL(Graph::enable(bool)), action, SLOT(QAction::setEnabled(bool)));


    action = tool_bar->addAction ("&Narrower", graph, SLOT (shrink_segment ()));
    action->setShortcut (QString ("3")); 
    QObject::connect(graph, SIGNAL(Graph::enable(bool)), action, SLOT(QAction::setEnabled(bool)));


    action = tool_bar->addAction ("&n*2", graph, SLOT (enlarge_n ()));
    action->setShortcut (QString ("4")); 
    QObject::connect(graph, SIGNAL(Graph::enable(bool)), action, SLOT(QAction::setEnabled(bool)));


    action = tool_bar->addAction ("&n/2", graph, SLOT (shrink_n ()));
    action->setShortcut (QString ("5")); 
    QObject::connect(graph, SIGNAL(Graph::enable(bool)), action, SLOT(QAction::setEnabled(bool)));


    action = tool_bar->addAction ("f_n/2+", graph, SLOT (fluctuate_plus ()));
    action->setShortcut (QString ("6")); 
    QObject::connect(graph, SIGNAL(Graph::enable(bool)), action, SLOT(QAction::setEnabled(bool)));


    action = tool_bar->addAction ("f_n/2-", graph, SLOT (flustuate_minus ()));
    action->setShortcut (QString ("7")); 
    QObject::connect(graph, SIGNAL(Graph::enable(bool)), action, SLOT(QAction::setEnabled(bool)));


    action = tool_bar->addAction ("&m*2", graph, SLOT (enlarge_m ()));
    action->setShortcut (QString ("6")); 
    QObject::connect(graph, SIGNAL(Graph::enable(bool)), action, SLOT(QAction::setEnabled(bool)));


    action = tool_bar->addAction ("&m/2", graph, SLOT (shrink_m ()));
    action->setShortcut (QString ("9")); 
    QObject::connect(graph, SIGNAL(Graph::enable(bool)), action, SLOT(QAction::setEnabled(bool)));


    action = tool_bar->addAction ("E&xit", window, SLOT (close ()));
    action->setShortcut (QString ("Ctrl+X"));

    switch (graph->parse_command_line(argc, argv)) {
        case status::ok:
            break;
        case status::error_data:
            QMessageBox::warning(0, "Wrong input arguments!", "Wrong input arguments!");
            return -1;
        case status::error_mem:
            QMessageBox::warning(0, "Cannot allocate memory!", "Cannot allocate memory!");
            return -2;
        default:
            QMessageBox::warning(0, "Unknown error!", "Unknown error!");
            return -3;
    }

    tool_bar->setMaximumHeight(30);

    window->setMenuBar(tool_bar);
    window->setStatusBar(status_bar);
    window->setCentralWidget(graph);
    window->setWindowTitle("Graph");

    QObject::connect(graph, SIGNAL(Graph::set_label(QString)), label, SLOT(QLabel::setText(QString)));

    window->show();
    app.exec();
    delete window;
    return 0;
}
