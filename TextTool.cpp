

#include "TextTool.h"
#include <QGraphicsTextItem>


TextTool::TextTool(QGraphicsScene *scene):_scene(scene)
{
    _sz = 30;
    _color = Qt::black;
    _font = QFont("Arial", _sz);
    drawingManager = new DrawingManager(_scene);
}

TextTool::~TextTool()
{
    delete drawingManager;
}

void TextTool::setTextSize(int sz)
{
    _sz = sz;
}

void TextTool::setTextColor(QColor color)
{
    _color = color;
}

void TextTool::setTextFont(QFont font)
{
    _font = font;
}


void TextTool::addText(const QString &str, const QPointF &scenePoint)
{
    if(!_scene) {
        qDebug() << "null scene";
        return;
    }

    QGraphicsTextItem *textItem = new QGraphicsTextItem(str);

//    textItem->setTextInteractionFlags(Qt::TextEditorInteraction);
    textItem->setDefaultTextColor(_color);
    textItem->setFont(_font);
    textItem->setPos(scenePoint);


    _scene->addItem(textItem);
}

void TextTool::setEditableStatus(bool op)
{
    qDebug() << "Toggling editable status to:" << op;
    int textItemCount = 0;
    for(auto *item : _scene->items()) {
        QGraphicsTextItem *textItem = dynamic_cast<QGraphicsTextItem*>(item);
        if(textItem) {
            qDebug() << "setEditable: textItem is not null ";
            textItemCount++;
            textItem->setTextInteractionFlags(op ? Qt::TextEditorInteraction :
                                                  Qt::NoTextInteraction);
        }
    }

    qDebug() << "Affected text items:" << textItemCount;
}

QFont TextTool::font() const
{
    return _font;
}

QColor TextTool::color() const
{
    return _color;
}

int TextTool::getTextSz() const
{
    return _sz;
}












