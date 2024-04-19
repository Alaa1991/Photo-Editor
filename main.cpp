#include "mainwindow.h"
#include <QApplication>
// #include <QObject>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    // QCoreApplication::setApplicationName(tr("Kditor"));

    return a.exec();
}
