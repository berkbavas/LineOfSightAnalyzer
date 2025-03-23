#include "Core/Controller.h"

#include <QApplication>
#include <QImageReader>

int main(int argc, char* argv[])
{
    using namespace LineOfSightAnalyzer;

    QApplication app(argc, argv);

    QImageReader::setAllocationLimit(1024);

    Controller controller;
    controller.Run();

    return app.exec();
}
