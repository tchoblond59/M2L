#include <QCoreApplication>
#include "controller.h"
int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    simpleqtlogger::ENABLE_LOG_SINK_CONSOLE = true;
    simpleqtlogger::SimpleQtLogger::createInstance(qApp)->setLogFileName("/var/log/MySensors/M2L.log", 10*10000, 10);
    Controller *controller = new Controller();

    return a.exec();
}
