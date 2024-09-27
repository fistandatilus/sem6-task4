#include <QApplication>
#include <QMainWindow>
#include <fenv.h>

#include "graph.h"

int main(int argc, char *argv[])
{
    feenableexcept(FE_ALL_EXCEPT ^ FE_INEXACT);
    QApplication app(argc, argv);

    QMainWindow *window = new QMainWindow;
    QMenuBar *tool_bar = new QMenuBar(window);
    QStatusBar *status_bar = new QStatusBar();
    Graph *graph = new Graph(window);

    QAction *action;
    /*
  action = tool_bar->addAction ("&Change function", graph, SLOT (change_func ()));
  action->setShortcut (QString ("0"));

  action = tool_bar->addAction ("&Change mode", graph, SLOT (change_mode ()));
  action->setShortcut (QString ("1"));

  action = tool_bar->addAction ("&Wider", graph, SLOT (enlarge_segment ()));
  action->setShortcut (QString ("2"));

  action = tool_bar->addAction ("&Narrower", graph, SLOT (shrink_segment ()));
  action->setShortcut (QString ("3"));

  action = tool_bar->addAction ("&n*2", graph, SLOT (enlarge_n ()));
  action->setShortcut (QString ("4"));

  action = tool_bar->addAction ("&n/2", graph, SLOT (shrink_n ()));
  action->setShortcut (QString ("5"));

  action = tool_bar->addAction ("f_n/2+", graph, SLOT (fluctuate_plus ()));
  action->setShortcut (QString ("6"));

  action = tool_bar->addAction ("f_n/2-", graph, SLOT (flustuate_minus ()));
  action->setShortcut (QString ("7"));

  action = tool_bar->addAction ("E&xit", window, SLOT (close ()));
  action->setShortcut (QString ("Ctrl+X"));
*/
    if (graph->parse_command_line(argc, argv)) {
        QMessageBox::warning(0, "Wrong input arguments!", "Wrong input arguments!");
        return -1;
    }

    tool_bar->setMaximumHeight(30);

    window->setMenuBar(tool_bar);
    window->setStatusBar(status_bar);
    window->setCentralWidget(graph);
    window->setWindowTitle("Graph");

    window->show();
    app.exec();
    delete window;
    return 0;
}
