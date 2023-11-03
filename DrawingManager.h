#ifndef DRAWINGMANAGER_H
#define DRAWINGMANAGER_H

#include <QObject>
#include <QGraphicsScene>
#include <QMouseEvent>
#include <QMap>
#include <QPainterPath>
#include <QTimer>
#include "EdgeEnum.h"

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
        Eraser,
        MoveTool,
        HandTool,
    };

    enum class ShapeType {
        Rectangle,
        Circle,
        Line,
        Brush
    };

    void setDrawMode(DrawMode mode);
    DrawMode drawMode() const;
    void drawBrush(const QPointF &point);
    void drawRect(const QPointF &startPoint, const QPointF &endPoint);
    void drawCircle(const QPointF &startPoint, const QPointF &endPoint);
    void drawLine(const QPointF &start, const QPointF &end);
    void erase(const QPointF &point);
    void erasePart(const QPointF &point);
    void eraseShape(const QPointF &point);
    void resetCurrentPath();
    void selectItem(const QPointF &point);
    void moveSelectedItem(const QPointF &point, bool isResizing);
    void deleteSelectedItems();
    void setBrushColor(QColor &color);
    QColor brushColor() const;
    void highlightItem(QGraphicsItem *item);
    void removeHighlightItem(QGraphicsItem *item);
    void selectAllItems();
    void deselectAllItems();

    void startRubberBandSelection(const QPointF &point);
    void updateRubberBandSelection(const QPointF &point);
    void endRubberBandSelection();

    bool isSelected();
    void changeItemsColors(QColor color);

    void keyPressEvent(QKeyEvent *event);
    void keyReleaseEvent(QKeyEvent *event);

    int getOpacityVal() const;

    void updateDashedLines();
    void resizeItem(const QPointF &point, const QPointF &startPoint, Edge edge);

    QList<QGraphicsItem*> getSelectedItems() const;
    void setSelectedItems(QGraphicsItem *item);
    int getToolSize() const;
    DrawingManager::ShapeType getShapeType(QGraphicsPathItem *pathItem) const;

    void copyItems();
    void pasteItems();

    bool isCopying = false;

    ~DrawingManager();

public slots:
    void setToolSize(int newSz);

    void changeOpacityVal(int newVal);



signals:
    void itemSelected(int opacity);
    void itemDeselected();



private:
     QGraphicsScene* scene;
     DrawMode currentDrawMode;
     QPointF startPoint;
     QPointF lastPoint;
     QGraphicsItem *currentShape;
     bool isDrawing;
     int currentSz;
     QColor currColor;
     QGraphicsPathItem *currentPath = nullptr;
     QPainterPath currentPaintPath;
     QGraphicsItem *selectedItem;

     QList<QGraphicsItem*> selectedItems;
     QMap<QGraphicsItem*, QPointF> startPoints;
     QMap<QGraphicsItem*, QPen> originalPens;

     bool ctrlPresed = false;

     QRectF rubberBandRect;
     bool isRubberBandActive;
     QGraphicsRectItem *tempRubberBand;
     bool isMoveWithoutRubberBand = false;

     float opacityVal;
     QMap<QGraphicsItem*, int> originalOpacities;

     int dashOffset = 0;
     QTimer *dashedLineTimer;
     QMap<QGraphicsPathItem*,ShapeType> shapeTypeMap;



     QList<QGraphicsItem*> copiedItems;

 };

#endif // DRAWINGMANAGER_H
