#ifndef KPATHITEM_H
#define KPATHITEM_H


#include <QGraphicsItem>
#include <QGraphicsPathItem>
#include <QDateTime>
#include "ShapeType.h"
#include <QUuid>
#include <QList>
#include <QBrush>
#include <QPen>
#include <QString>

class KPathItem : public QGraphicsPathItem
{

public:

    KPathItem(const QPainterPath &path);
    KPathItem();

    virtual ~KPathItem() {}

    //GETTERS AND SETTERS
    void initializeProp();

    void setSize(qreal width,qreal height);

    int type() const override { return KPathItemType;}

    bool getLocked() const;
    void setLocked(bool val);
    // bool getColor() const;
    void setFillColor(const QColor &color);
    QColor getFillColor() const;

    void setIsSelected(bool val);
    bool getIsSelected() const;

    QColor getBorderColor() const;
    void setBorderColor(const QColor &clr);

    QDateTime getCreationTime() const;
    QDateTime getLastModifiedTime() const;
    void setLastModifiedTime(QDateTime dt);

    qreal getItemOpacity() const;
    void setItemOpacity(qreal opacity);

    void setShapeType(ShapeType shapeType);
    ShapeType getShapeType() const;

    qreal getBorderSize() const;
    void setBorderSize(qreal border);

    void setBrushSize(int sz);
    int getBrushSize() const;

    bool getIsResizeable() const;
    void setIsResizeable(bool isResizeable);

    QString getTag() const;
    void setTag(QString str);

    void setName(QString str);
    QString getName() const;


private:
    bool isLocked;
    QColor fillColor;
    QColor borderColor;
    QColor brushColor;
    int borderWidth;

    bool isSelected;
    bool isHighlighted;

    QDateTime creationTime;
    QDateTime lastModifiedTime;
    qreal itemOpacity;
    ShapeType shapeType;
    QString uniqueId;
    int zIndex;
    QString tag;
    qreal borderSize;
    qreal itemSize;
    int brushSize;
    bool isResizeable;

    // QList<KPathItem*> kpathArr;

    const int KPathItemType = QGraphicsItem::UserType+1;
    QPainterPath originalPath;

    QString name;
};

#endif // KPATHITEM_H
