/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 6.7.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QIcon>
#include <QtWidgets/QApplication>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QWidget>
#include "customgraphicsview.h"

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QAction *actionOpen;
    QAction *actionExit;
    QWidget *centralWidget;
    QPushButton *btnGray;
    QPushButton *btnCircle;
    QPushButton *btnRect;
    QPushButton *btnClear;
    CustomGraphicsView *graphicsView;
    QMenuBar *menuBar;
    QMenu *menuFile;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName("MainWindow");
        MainWindow->resize(1083, 704);
        actionOpen = new QAction(MainWindow);
        actionOpen->setObjectName("actionOpen");
        actionExit = new QAction(MainWindow);
        actionExit->setObjectName("actionExit");
        centralWidget = new QWidget(MainWindow);
        centralWidget->setObjectName("centralWidget");
        btnGray = new QPushButton(centralWidget);
        btnGray->setObjectName("btnGray");
        btnGray->setGeometry(QRect(990, 60, 91, 61));
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/icons/greyscale.png"), QSize(), QIcon::Normal, QIcon::Off);
        btnGray->setIcon(icon);
        btnGray->setIconSize(QSize(40, 40));
        btnGray->setFlat(true);
        btnCircle = new QPushButton(centralWidget);
        btnCircle->setObjectName("btnCircle");
        btnCircle->setGeometry(QRect(990, 140, 91, 51));
        QIcon icon1;
        icon1.addFile(QString::fromUtf8(":/icons/circle.png"), QSize(), QIcon::Normal, QIcon::Off);
        btnCircle->setIcon(icon1);
        btnCircle->setIconSize(QSize(40, 40));
        btnCircle->setFlat(true);
        btnRect = new QPushButton(centralWidget);
        btnRect->setObjectName("btnRect");
        btnRect->setGeometry(QRect(1000, 200, 71, 41));
        QIcon icon2;
        icon2.addFile(QString::fromUtf8(":/icons/rounded-rectangle.png"), QSize(), QIcon::Normal, QIcon::Off);
        btnRect->setIcon(icon2);
        btnRect->setIconSize(QSize(40, 40));
        btnRect->setFlat(true);
        btnClear = new QPushButton(centralWidget);
        btnClear->setObjectName("btnClear");
        btnClear->setGeometry(QRect(1010, 260, 75, 41));
        QIcon icon3;
        icon3.addFile(QString::fromUtf8(":/icons/cleaning.png"), QSize(), QIcon::Normal, QIcon::Off);
        btnClear->setIcon(icon3);
        btnClear->setIconSize(QSize(40, 40));
        btnClear->setFlat(true);
        graphicsView = new CustomGraphicsView(centralWidget);
        graphicsView->setObjectName("graphicsView");
        graphicsView->setGeometry(QRect(0, -30, 981, 631));
        MainWindow->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(MainWindow);
        menuBar->setObjectName("menuBar");
        menuBar->setGeometry(QRect(0, 0, 1083, 21));
        menuFile = new QMenu(menuBar);
        menuFile->setObjectName("menuFile");
        MainWindow->setMenuBar(menuBar);
        mainToolBar = new QToolBar(MainWindow);
        mainToolBar->setObjectName("mainToolBar");
        MainWindow->addToolBar(Qt::ToolBarArea::TopToolBarArea, mainToolBar);
        statusBar = new QStatusBar(MainWindow);
        statusBar->setObjectName("statusBar");
        MainWindow->setStatusBar(statusBar);

        menuBar->addAction(menuFile->menuAction());
        menuFile->addAction(actionOpen);
        menuFile->addAction(actionExit);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "MainWindow", nullptr));
        actionOpen->setText(QCoreApplication::translate("MainWindow", "Open", nullptr));
        actionExit->setText(QCoreApplication::translate("MainWindow", "Exit", nullptr));
        btnGray->setText(QString());
        btnCircle->setText(QString());
        btnRect->setText(QString());
#if QT_CONFIG(tooltip)
        btnClear->setToolTip(QCoreApplication::translate("MainWindow", "Clear Scene", nullptr));
#endif // QT_CONFIG(tooltip)
        btnClear->setText(QString());
        menuFile->setTitle(QCoreApplication::translate("MainWindow", "File", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
