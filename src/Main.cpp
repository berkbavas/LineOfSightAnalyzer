
#include "Window.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    LineOfSightAnalyzer::Window w;
    w.resize(1600, 900);
    w.show();
    return app.exec();
}
