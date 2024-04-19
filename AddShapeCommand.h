#ifndef ADDSHAPECOMMAND_H
#define ADDSHAPECOMMAND_H


#include <QUndoCommand>
#include <QGraphicsItem>
#include <QGraphicsScene>

class AddShapeCommand : public QUndoCommand {
public:
    AddShapeCommand(QGraphicsItem *item, QGraphicsScene *scene,
                    QUndoCommand *parent = nullptr):QUndoCommand(parent),myItem(item),
        myScene(scene)
    {

    }

    void undo() override
    {
        myScene->removeItem(myItem);
        myItem->setVisible(false);
    }

    void redo() override
    {
        myScene->addItem(myItem);
        myItem->setVisible(true);
    }


private:
    QGraphicsItem *myItem;
    QGraphicsScene *myScene;
};


#endif // ADDSHAPECOMMAND_H
