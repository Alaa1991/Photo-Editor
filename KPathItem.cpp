
#include "KPathItem.h"


KPathItem::KPathItem(const QPainterPath &path):QGraphicsPathItem(path)
{
    initializeProp();
    originalPath = path;
}

KPathItem::KPathItem()
{
    initializeProp();
}

void KPathItem::initializeProp()
{
    isLocked = false;
    isSelected = false;
    isHighlighted = false;
    isResizeable = true;

    fillColor = Qt::blue;
    borderColor = Qt::black;

    creationTime = QDateTime::currentDateTime();
    lastModifiedTime = creationTime;
    itemOpacity = 1.0;
    uniqueId = QUuid::createUuid().toString();
    zIndex = 0;
}



bool KPathItem::getLocked() const
{
    return isLocked;
}

void KPathItem::setLocked(bool val)
{
    isLocked = val;
}

QColor KPathItem::getFillColor() const
{
    // return fillColor;
    return this->brush().color();
}

void KPathItem::setFillColor(const QColor &color)
{
    fillColor = color;

    this->setBrush(fillColor);
}

bool KPathItem::getIsSelected() const
{
    return isSelected;
}

void KPathItem::setIsSelected(bool val)
{
    isSelected = val;
}

void KPathItem::setBorderColor(const QColor &color)
{
    borderColor = color;
    QPen pen(borderColor);
    qDebug() << __FUNCTION__ << "color is " << borderColor;
    this->setPen(pen);
}

QColor KPathItem::getBorderColor() const
{
    return borderColor;
}


QDateTime KPathItem::getCreationTime() const
{
    return creationTime;
}

QDateTime KPathItem::getLastModifiedTime() const
{
    return lastModifiedTime;
}

qreal KPathItem::getBorderSize() const
{
    return borderSize;
}

void KPathItem::setBorderSize(qreal borderSz)
{
    borderSize = borderSz;
    QPen pen;
    pen.setWidth(borderSz);
    this->setPen(pen);
}


qreal KPathItem::getItemOpacity() const
{
    return itemOpacity;
}

void KPathItem::setItemOpacity(qreal opacity)
{
    itemOpacity = opacity;
    this->setOpacity(itemOpacity);
}


int KPathItem::getBrushSize() const
{
    return brushSize;
}

void KPathItem::setBrushSize(int sz)
{
    brushSize = sz;
    if(this->shapeType==ShapeType::Brush || this->shapeType==ShapeType::Line){

        QPen pen;
        pen.setWidth(sz);
        this->setPen(pen);
    }
}


bool KPathItem::getIsResizeable() const
{
    return isResizeable;
}

void KPathItem::setIsResizeable(bool val)
{
    isResizeable = val;
}

void KPathItem::setTag(QString str)
{
    tag = str;
}

QString KPathItem::getTag() const
{
    return tag;
}


void KPathItem::setShapeType(ShapeType type)
{
    shapeType = type;
}

ShapeType KPathItem::getShapeType() const
{
    return shapeType;
}

void KPathItem::setName(QString str)
{
    name = str;
}

QString KPathItem::getName() const
{
    return name;
}


void KPathItem::setSize(qreal width, qreal height)
{
    if(originalPath.isEmpty()) {
        originalPath = this->path();
    }

    QRectF boundingRect = originalPath.boundingRect();
    qreal scaleX = width / boundingRect.width();
    qreal scaleY = height / boundingRect.height();

    QTransform transform;
    transform.scale(scaleX, scaleY);

    QPainterPath scaledPath = transform.map(originalPath);

    this->setPath(scaledPath);
}

void KPathItem::setLastModifiedTime(QDateTime dt)
{
    lastModifiedTime = dt;
}





































