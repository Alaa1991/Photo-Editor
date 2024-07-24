#ifndef SCENEINFOWIND_H
#define SCENEINFOWIND_H

#include <QDialog>
#include <QGraphicsScene>
#include <QVBoxLayout>
#include <QListWidget>
#include "KPathItem.h"
#include "ShapeType.h"
#include <QEvent>
#include <QObject>
#include "DrawingManager.h"
#include <QCheckBox>
#include <QLabel>
#include <QPushButton>
#include <QInputDialog>
#include <QMap>
#include <QRegularExpression>
#include <QVariant>

class SceneInfoWindo : public QDialog {
    Q_OBJECT

public:
     SceneInfoWindo(QGraphicsScene *scene, DrawingManager *dm,
                   QWidget *parent =nullptr);

     void updateInfo();
     void clearListItemsSelc();

     void clearList();

 public slots:

     void deleteLstItem();
     void onItemDeleted(QGraphicsItem *item);
     void selectItem(QGraphicsItem *item);
     void itemRename(QListWidgetItem *item);
     void startRenaming();
     void onItemUpdated(QGraphicsItem *item);

 signals:
    void itemClicked(QGraphicsItem *item);
    void itemDoubleClicked(QGraphicsItem *item);
    void deleteItem(QGraphicsItem *item);

protected:
     bool eventFilter(QObject *watched, QEvent *event) override;

private:


    int circleCount ,rectCount ,lineCount,
        brushCount,starCount,arrowCount,polygonCount,
        heartCount,bubbleCount;

    QGraphicsScene *scene;
    QVBoxLayout *layout;

    QListWidget *listWidget;

    DrawingManager *dm;
    QCheckBox *checkVisible;
    QHBoxLayout *layoutWidget;
    QLabel *label;

    QPushButton *btnClrList;
    QPushButton *btnRemove;
    QPushButton *btnRename;

    QMap<QGraphicsItem*, QString> itemNames;
    QMap<ShapeType, QSet<int>> avialableNumbers;
    QSet<QGraphicsItem*> displayedItems;

    QString itemName;
    QString infos;

    void setupButtons();
    void setupLayouts();
    void setupConnections();

    int extractNumFromNames(const QString& name);
    QString getNewNameForShape(ShapeType type);
    void resetVars();
};

#endif // SCENEINFOWIND_H
