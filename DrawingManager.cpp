

#include "DrawingManager.h"
#include <QGraphicsLineItem>
#include <QGraphicsRectItem>
#include <QGraphicsEllipseItem>
#include <QGraphicsView>
#include <QDebug>

DrawingManager::DrawingManager(QGraphicsScene *scene):scene(scene),
    currentDrawMode(NoDraw)
{
    currentSz = 10;
}

void DrawingManager::setDrawMode(DrawMode mode)
{
    currentDrawMode = mode;
}

DrawingManager::DrawMode DrawingManager::drawMode() const
{
    return currentDrawMode;
}

void DrawingManager::drawRect(const QPointF &start, const QPointF &end)
{
    QGraphicsRectItem *rect = new QGraphicsRectItem(QRectF(start,end));
    rect->setBrush(Qt::blue);
    scene->addItem(rect);
}

void DrawingManager::drawCircle(const QPointF &start, const QPointF &end)
{
   QGraphicsEllipseItem *ellipse = new QGraphicsEllipseItem(QRectF(start,end));
   ellipse->setBrush(Qt::red);
   scene->addItem(ellipse);
}




void DrawingManager::drawBrush(const QPointF &point, bool isPressing)
{
    if(isPressing) {
        if(currentPath == nullptr) {
            currentPaintPath = QPainterPath(point);
            currentPath = new QGraphicsPathItem(currentPaintPath);
            scene->addItem(currentPath);

            QPen pen;
            pen.setWidth(currentSz);
            currentPath->setPen(pen);

        } else {
            currentPaintPath.lineTo(point);
            currentPath->setPath(currentPaintPath);
        }
    }

    else {
        currentPath = nullptr;
    }
}

void DrawingManager::drawLine(const QPointF &start, const QPointF &end)
{
    QPen pen;
    pen.setWidth(currentSz);
    QGraphicsLineItem *line = new QGraphicsLineItem(QLineF(start,end));
    line->setPen(pen);
    scene->addItem(line);
}


//void DrawingManager::erase(const QPointF &point)
//{
//    QGraphicsPathItem *path = new QGraphicsPathItem();
//    QPainterPath erasePath;
//    erasePath.addEllipse(point, 5, 5);
//    path->setPath(erasePath);
//    path->setBrush(Qt::white);
//    scene->addItem(path);
//}



//void DrawingManager::erasePart(const QPointF &point)
//{

//    QGraphicsPathItem *path = new QGraphicsPathItem();
//    QPainterPath erasePath;
//    erasePath.addEllipse(point,currentSz/2,currentSz/2);
//    path->setPath(erasePath);
//    if(scene->backgroundBrush().color().alpha() == 0){
//        path->setBrush(Qt::white);
//        path->setPen(Qt::NoPen);
//    }
//    else {
//        QColor backgroundColor = scene->backgroundBrush().color();
//        path->setBrush(backgroundColor);
//        path->setPen(QPen(backgroundColor));
//    }
//   scene->addItem(path);

//}

void DrawingManager::erasePart(const QPointF &point)
{
    QGraphicsItem *item = scene->itemAt(point, QTransform());
}

void DrawingManager::setToolSize(int newSz)
{
    currentSz = newSz;
}

int DrawingManager::getToolSize() const
{
    return currentSz;
}









