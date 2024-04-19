
#include "KGraphicsScene.h"
#include <QDebug>

KGraphicsScene::KGraphicsScene(QObject *parent )
    :QGraphicsScene(parent) {}


void KGraphicsScene::contextMenuEvent(QGraphicsSceneContextMenuEvent *event)
{
     qDebug() << __FUNCTION__;
    QGraphicsItem *item = itemAt(event->scenePos(), QTransform());

    emit contextmMenuRequested(event->scenePos(), item);
}
