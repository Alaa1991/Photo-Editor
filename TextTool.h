#ifndef TEXTTOOL_H
#define TEXTTOOL_H

#include <QGraphicsScene>

class TextTool
{

    TextTool(QGraphicsScene *scene);
    ~TextTool();

    void setTextSize(int sz);
    void setTextColor(QColor color);
    void setTextStyle();
    void setTextFont(QFont font);

private:
    QString _text;
    QColor _color;
    QFont _font;
};

#endif // TEXTTOOL_H
