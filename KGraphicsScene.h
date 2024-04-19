#ifndef KGRAPHICSSCENE_H
#define KGRAPHICSSCENE_H

#include <QGraphicsScene>
#include <QGraphicsSceneContextMenuEvent>

class KGraphicsScene : public QGraphicsScene {
    Q_OBJECT

public:

    KGraphicsScene(QObject *parent = nullptr);

protected:
    void contextMenuEvent(QGraphicsSceneContextMenuEvent *event) override;


signals:
    void contextmMenuRequested(QPointF scenePos,QGraphicsItem *item);


};

#endif // KGRAPHICSSCENE_H
