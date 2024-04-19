#ifndef DRAWINGMANAGER_H
#define DRAWINGMANAGER_H

#include <QObject>
#include <QGraphicsScene>
#include <QMouseEvent>
#include <QMap>
#include <QPainterPath>
#include <QTimer>
#include "EdgeEnum.h"
#include "ShapeType.h"
#include "KPathItem.h"

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
        DrawPolygon,
        DrawFreehand,
        Eraser,
        MoveTool,
        HandTool,
        FreehandTool,
        DrawArrow,
        DrawStar,
        DrawHeart,
        DrawBubble
    };

    struct CustomGroup {
        QList<QGraphicsItem*> items;

        bool operator==(const CustomGroup &other) const {
            if(items.size() != other.items.size())
                return false;

            for(int i = 0; i < items.size();++i) {
                if(items[i] != other.items[i])
                    return false;
            }
        }
    };




    void setDrawMode(DrawMode mode);
    DrawMode drawMode() const;
    void drawBrush(const QPointF &point);
    void drawRect(const QPointF &startPoint, const QPointF &endPoint);
    void drawCircle(const QPointF &startPoint, const QPointF &endPoint);
    void drawLine(const QPointF &start, const QPointF &end);

    void drawPloygonPoint(const QPointF &point);
    void finishPolygon();
    void stratFreehandDrawing(const QPointF &point);
    void continueFreehandDrawing(const QPointF &point);
    void finishFreehandDrawing();

    void drawFreehand(const QPointF &point);
    void drawArrow(const QPointF &start, const QPointF &end);

    void drawStar(const QPointF &center,double radius,
                  int points = 5);
    void drawHeart(const QPainterPath&);
    void drawSpeechBubble(const QPainterPath&);


    QPainterPath createStarPath(const QPointF &center,double radius,
                                int points);

    void drawHeart(const QPointF &heart, double size);


    void erase(const QPointF &point);
    void erasePart(const QPointF &point);
    void eraseShape(const QPointF &point);
    void resetCurrentPath();

    void selectItem(const QPointF &point);
    void selectItem(QGraphicsItem *item);

    void moveSelectedItem(const QPointF &point, bool isResizing);
    void deleteSelectedItems();
    void deleteSingleItem(QGraphicsItem *item);

    //DM Color setters/getters
    void setSysFillColor(QColor &color);
    QColor brushColor() const;
    void setBorderColor(QColor &color);
    QColor getBorderColor() const;
    void changeItemsFillColor(QColor &color);
    void chageItemsBorderColor(QColor &color);

    int getSysBorderSz() const;
    void setSysBorderSz(int size);

    void highlightItem(QGraphicsItem *item);
    void removeHighlightItem(QGraphicsItem *item);

    void deselectAllItems();

    void startRubberBandSelection(const QPointF &point);
    void updateRubberBandSelection(const QPointF &point);
    void endRubberBandSelection();

    bool isSelected();


    void keyPressEvent(QKeyEvent *event);
    void keyReleaseEvent(QKeyEvent *event);

    int getOpacityVal() const;

    void updateDashedLines();
    void resizeItem(const QPointF &point, const QPointF &startPoint, Edge edge);

    QList<QGraphicsItem*> getSelectedItems() const;
    QList<QGraphicsItem*> getCopiedItems() const;
    void appendSelectedItems(QGraphicsItem *item);
    int getToolSize() const;
    ShapeType getShapeType(KPathItem *pathItem) const;


    int getStarHeads() const {return starHeads;}
    bool getIsLocked() const;
    void setIsLocked(bool val);

    void changeShapeSz(qreal width, qreal height);
    void changeShapePos(qreal width, qreal height);

    void resizeArrow(QGraphicsPathItem *item,const QPointF &dleta,
                     Edge edge);

    void groupItems();
    void ungroupItems();
    void clearGroups();

    CustomGroup *findGroup(QGraphicsItem *item);

    bool isCopying = false;

    QPainterPath createHeartPath(const QPointF&,double);
    QPainterPath createBubblePath(const QRectF&);

    QPointF currentMousePos;

    ~DrawingManager();

public slots:

    void setToolSize(int newSz);
    void changeOpacityVal(int newVal);

    void changeBorderSz(int sz);
    void setHandBrushColor(QColor &color);
    void setStarHeads(const int& heads);
    QColor getHandBrushColor() const;

    void copyItems();
    void pasteItems();
    void deleteItem(QGraphicsItem *item);
    void selectAllItems();

signals:
    void itemSelected(int opacity,QGraphicsItem *item);
    void itemSelected(QGraphicsItem *item);
    void itemDeselected();
    void textSelected();
    void textDeselected();

    void addShapeCommand(QGraphicsItem *item);
    void removeShapeCommand(QGraphicsItem *item);
    void borderSzChanged(int sz);

    void itemUpdated(QGraphicsItem *item);
    void itemDeleted(QGraphicsItem *item);
    void itemCopied();



private:

    void moveGroup(CustomGroup &group,const QPointF &delta);

    void resizeHeartShape(KPathItem*,const QPointF&);



    QPainterPath createScaledBubblePath(const QRectF&);

     QGraphicsScene* scene;
     DrawMode currentDrawMode;
     QPointF startPoint;
     QPointF lastPoint;
     QGraphicsItem *currentShape;
     bool isDrawing;
     int sysSz;

     QColor sysFillColor;
     QColor sysBorderColor;
     QColor handBrushColor = Qt::blue;
     int sysBorderSz;
     int arrowWidth;

     KPathItem *kBrushPath = nullptr;
     QPainterPath currentPaintPath;
     QGraphicsItem *selectedItem;

     QList<QGraphicsItem*>  selectedItems;
     QMap<QGraphicsItem*, QPointF> startPoints;

     QMap<QGraphicsItem*, QPen> originalPens;
     QMap<QGraphicsItem*, QBrush> originalBrushes;

     bool ctrlPresed = false;

     QRectF rubberBandRect;
     bool isRubberBandActive;
     QGraphicsRectItem *tempRubberBand;
     bool isMoveWithoutRubberBand = false;

     float opacityVal;
     QMap<QGraphicsItem*, int> originalOpacities;

     int dashOffset = 0;
     QTimer *dashedLineTimer;
     QMap<KPathItem*,ShapeType> shapeTypeMap;



     QList<QGraphicsItem*> copiedItems;
     QMap<QGraphicsItem*, QGraphicsRectItem*> originalTextBackbrounds;

     QList<QPointF> polygonPoints;

     KPathItem *currPolygonItem = nullptr;

     std::unique_ptr<QPainterPath> currentFreehandPath;
     std::unique_ptr<KPathItem> currFreehandItm;

     int starHeads = 5;

     QList<CustomGroup*> customGroups;



 };

#endif // DRAWINGMANAGER_H
