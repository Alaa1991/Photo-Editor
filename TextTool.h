#ifndef TEXTTOOL_H
#define TEXTTOOL_H

#include <QGraphicsScene>
#include "DrawingManager.h"
class TextTool
{

public:

    TextTool(QGraphicsScene *scene);
    ~TextTool();

    void setTextSize(int sz);
    void setTextColor(QColor color);
    void setTextStyle();
    void setTextFont(QFont font);
    void addText(const QString &str,const QPointF &point);
    int getTextSz() const;
    QFont font() const;
    QColor color() const;
    void setEditableStatus(bool op);

private:
    QString _text;
    QColor _color;
    QFont _font;
    int _sz;

    QGraphicsScene *_scene;

    DrawingManager *drawingManager;

};

#endif // TEXTTOOL_H
