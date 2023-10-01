#ifndef DRAWINGMANAGER_H
#define DRAWINGMANAGER_H

#include <QObject>
#include <QGraphicsScene>
#include <QMouseEvent>

class DrawingManager : public QObject
{
    Q_OBJECT

public:
    explicit DrawingManager(QGraphicsScene *scene);

    enum DrawMode {
        NoDraw,
        DrawRect,
        DrawCircle,
        DrawBrush,
        DrawLine,
        Eraser
    };

    void setDrawMode(DrawMode mode);
    DrawMode drawMode() const;
    void drawBrush(const QPointF &point,bool isPressing);
    void drawRect(const QPointF &startPoint, const QPointF &endPoint);
    void drawCircle(const QPointF &startPoint, const QPointF &endPoint);
    void drawLine(const QPointF &start, const QPointF &end);
    void erase(const QPointF &point);
    void erasePart(const QPointF &point);
    void eraseShape(const QPointF &point);


public slots:
    void setToolSize(int newSz);
    int getToolSize() const;

private:
     QGraphicsScene* scene;
     DrawMode currentDrawMode;
     QPointF startPoint;
     QPointF lastPoint;
     QGraphicsItem *currentShape;
     bool isDrawing;
     int currentSz;
     QGraphicsPathItem *currentPath = nullptr;
     QPainterPath currentPaintPath;

 };

#endif // DRAWINGMANAGER_H
