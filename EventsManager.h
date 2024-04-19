#ifndef EVENTSMANAGER_H
#define EVENTSMANAGER_H

#include <QMouseEvent>
#include <QGraphicsItem>
#include <QGraphicsScene>
#include <QGraphicsView>
#include "DrawingManager.h"
#include <functional>
#include "EdgeEnum.h"

class EventsManager : public QObject {
    Q_OBJECT
public:

    EventsManager(QGraphicsScene *scene, DrawingManager *dm);
    ~EventsManager();

    void setStr(const QString& newStr);

    //CALLBACKS
    std::function<QPointF(const QMouseEvent*)> mapToSceneFunc;
    std::function<void(const QString&,const QPointF&)> txtToolFunc;
    std::function<Edge(const QPointF&, QGraphicsItem*)> isNearEdgeFunc;
    std::function<void(bool)> setCursorFunc;
    std::function<void(void)> setMoveToolFunc;
    std::function<bool(void)> checkBtnTextTool;

// public slots:
    void onViewMousePressed(QMouseEvent *event);
    void onViewMouseMoved(QMouseEvent *event);
    void onViewMouseReleased(QMouseEvent *event);
    void onDoubleClickEvent(QMouseEvent *event);

signals:
    void itemDoubleClicked(QGraphicsItem *item);

private:
    QPointF startPoint;
    QPointF rectStartPoint;
    QPointF circleStartPoint;
    QPointF lineStartPoint;
    QGraphicsRectItem *tempRect = nullptr;
    QGraphicsEllipseItem *tempCircle = nullptr;
    QPointF arrowStartPoint;
    QPointF starStartPoint;
    QGraphicsPathItem *tempArrow = nullptr;
    QGraphicsPathItem *tempStar = nullptr;

    QGraphicsScene *scene;
    DrawingManager *drawingManager;
    // QPointF scenePoint;
    QString str;
    Edge currentEdge = None;
    bool isResizing = false;

    QPointF lastScenePoint;

    QPointF heartStartPoint;
    QGraphicsPathItem *tempHeart = nullptr;
    QPointF bubbleStartPoint;
    QGraphicsPathItem *tempBubble = nullptr;

    QPointF initialMousePos; // Set this in the mousePressEvent
    qreal initialDistanceToCenter; // Calculate and set this in the mousePressEvent
};



#endif // EVENTSMANAGER_H
