#include "mainwindow.h"
#include <QApplication>
// #include <QObject>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QCoreApplication::setOrganizationName("K&K");
    QCoreApplication::setApplicationName("KDitor");
    MainWindow w;
    w.show();

    // QCoreApplication::setApplicationName(tr("Kditor"));

    return a.exec();
}
