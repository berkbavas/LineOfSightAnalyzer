#include "Core/Controller.h"

#include <QApplication>

int main(int argc, char* argv[])
{
    using namespace LineOfSightAnalyzer;

    QApplication app(argc, argv);

    Controller controller;
    controller.Run();

    return app.exec();
}
