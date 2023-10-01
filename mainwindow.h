#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsScene>
#include <QPushButton>
#include <QMouseEvent>
#include "DrawingManager.h"
#include <QSlider>

class CustomGraphicsView;
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);

    enum DrawMode {
        NoDraw,
        DrawRect,
        DrawCircle,
        DrawBrush
    };


    ~MainWindow();

public slots:

private slots:
    void openImage();
    void convertToGrayscale();
    void setDrawRectMode();
    void setDrawCircleMode();
    void setDrawBrushMode();
    void setDrawLineMode();
    void setEraserMode();
    void clearScene();

    void onSizeChanged(int newSz);
    void onViewMousePressed(QMouseEvent* event);
    void onViewMouseMoved(QMouseEvent* event);
    void onViewMouseReleased(QMouseEvent* event);

protected:
    void mousePressEvent(QMouseEvent* event);
    void mouseMoveEvent(QMouseEvent* event);
    void mouseReleaseEvent(QMouseEvent* event);

private:

    //Functions
    void setupButtons();
    void setupToolbar();
    void setupLayouts();
    void setupStyles();
    void setupConnections();
    void createWidgets();


    void activateDrawButtons();

    QToolBar *drawingsToolbar;
    QToolBar *adjustmentsToolbar;

    QPushButton *btnBrush;
    QPointF lastPoint;

    Ui::MainWindow *ui;
    QGraphicsScene* scene;
    DrawingManager *drawingManager;
    CustomGraphicsView *customView;
    QPointF startPoint;
    QGraphicsItem *currentShape;

    QRectF initialSceneRect;
    QGraphicsRectItem *transparentRect;

    QPointF rectStartPoint;
    QPointF circleStartPoint;
    QPointF lineStartPoint;
    QGraphicsRectItem *tempRect = nullptr;
    QGraphicsEllipseItem *tempCircle = nullptr;
    QPushButton *btnLine;
    QPushButton *btnEraser;

    bool isPressing;
//    QSlider *szSlider;
};

#endif // MAINWINDOW_H
