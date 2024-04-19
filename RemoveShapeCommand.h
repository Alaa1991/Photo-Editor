#ifndef REMOVESHAPECOMMAND_H
#define REMOVESHAPECOMMAND_H


#include <QGraphicsItem>
#include <QGraphicsScene>
#include <QUndoCommand>
//TODO this class not act correctly for undo functionality

class RemoveShapeCommand : public QUndoCommand {

public:

    RemoveShapeCommand(QGraphicsItem *item, QGraphicsScene *scene,
                       QUndoCommand *parent=nullptr):QUndoCommand(parent),
        myItem(item), myScene(scene)
    {}

    ~RemoveShapeCommand()
    {
        if(myItem && myItem->scene() == nullptr) {
            delete myItem;
        }
    }

    // Add the item back to the scene
    void undo() override {
        if(myItem && myScene->items().contains(myItem)) {
            myItem->setVisible(true);
            myScene->addItem(myItem);

        }
    }

    // Remove the item from the scene
    void redo() override {
        if(myItem && myScene->items().contains(myItem)) {
            myItem->setVisible(false);
            myScene->removeItem(myItem);
        }
    }

private:
    QGraphicsItem *myItem;
    QGraphicsScene *myScene;
};

#endif // REMOVESHAPECOMMAND_H
