QT += widgets

HEADERS       = approximations.h \
                functions.h \
                graph.h \
                msr.h \
                thread_related.h

SOURCES       = approximation.cpp \
                approximation_evaluation.cpp \
                controller.cpp \
                functions.cpp \
                graph.cpp \
                graph_slots.cpp \
                initializations.cpp \
                main.cpp \
                msr.cpp \
                precision_checks.cpp \
                preconditioner.cpp \
                solve.cpp \
                thread_related.cpp

TARGET = a.out

