#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsScene>
#include <QPushButton>
#include <QMouseEvent>
#include "DrawingManager.h"
#include <QSlider>
#include "EdgeEnum.h"

class CustomGraphicsView;
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);

    // enum DrawMode {
    //     NoDraw,
    //     DrawRect,
    //     DrawCircle,
    //     DrawBrush
    // };


    ~MainWindow();

    void setProgrammaticChange(bool value)
    { isProgrammaticChange = value; }
    bool getProgrammaticChange() const
    { return isProgrammaticChange; }





private slots:
    void openImage();
    void convertToGrayscale();
    void setDrawRectMode();
    void setDrawCircleMode();
    void setDrawBrushMode();
    void setDrawLineMode();
    void setEraserMode();
    void setMoveToolMode();
    void setHandToolMode();

    void clearScene();
    void pickColor();

    void zoomIn();
    void zoomOut();

    void onSizeChanged(int newSz);
    void onViewMousePressed(QMouseEvent* event);
    void onViewMouseMoved(QMouseEvent* event);
    void onViewMouseReleased(QMouseEvent* event);

    void handleItemSelected(int opacity);
    void handleItemDeselected();


    void saveSettings();
    void loadSettings();

    void handToolEnabled();


protected:
    void mousePressEvent(QMouseEvent* event);
    void mouseMoveEvent(QMouseEvent* event);
    void mouseReleaseEvent(QMouseEvent* event);
    void keyPressEvent(QKeyEvent *event);
    void keyReleaseEvent(QKeyEvent *event);

private:

    //Functions
    void setupButtons();
    void setupToolbar();
    void setupLayouts();
    void setupStyles();
    void setupConnections();
    void createWidgets();
    void setupButtonGroups();

    void resetZoom();

    Edge isNearEdge(const QPointF &cursorPos, QGraphicsItem *item);


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
    QPushButton *btnMove;
    QSlider *szSlider;
    QSlider *opacitySlider;
    QGraphicsItem *selectedItem = nullptr;
    QPushButton *colorPickerBtn;
    QPushButton *btnLoad;
    QPushButton *btnZoomIn;
    QPushButton *btnZoomOut;
    QPushButton *btnHandTool;


    QButtonGroup *buttonGroup;

    bool isProgrammaticChange = false;

    bool isResizing = false;
    bool isMoving = false;

    Edge currentEdge = None;

    const double zoomFactor = 1.5;


//    QSlider *szSlider;

};

#endif // MAINWINDOW_H
