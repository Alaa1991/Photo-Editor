


#include "SceneInfoWind.h"

SceneInfoWindo::SceneInfoWindo(QGraphicsScene *scene, DrawingManager *dm,
                               QWidget *parent)
    :QDialog(parent), scene(scene),dm(dm),
    circleCount(0),rectCount(0),lineCount(0),brushCount(0),starCount(0),
    arrowCount(0),polygonCount(0),heartCount(0),bubbleCount(0)
{
    setWindowTitle(tr("Scene Items Info"));
    setupButtons();
    setupLayouts();
    updateInfo();
    setupConnections();



    listWidget->installEventFilter(this);
}


void SceneInfoWindo::updateInfo()
{
    // listWidget->clear();

    QList<QGraphicsItem*> items = scene->items();
    std::reverse(items.begin(),items.end());

    // Reset counts only if the map is empty (no items have been named yet)
    if (itemNames.isEmpty()) {
        circleCount = rectCount = lineCount = brushCount
            = starCount = arrowCount = polygonCount =  0;
    }

    for(auto *item : items) {
        KPathItem *kpathItem = dynamic_cast<KPathItem*>(item);

        if(kpathItem && !displayedItems.contains(kpathItem)) {

            // QString itemName;

            if(itemNames.contains(kpathItem)) {
                itemName = itemNames.value(kpathItem);

            } else {

                switch(kpathItem->getShapeType()) {

                case ShapeType::Circle:
                    itemName = QString("Circle%1").arg(++circleCount);
                    break;

                case ShapeType::Rectangle:
                    itemName = QString("Rectangle%1").arg(++rectCount);
                    break;

                case ShapeType::Arrow:
                    itemName = QString("Arrow%1").arg(++arrowCount);
                    break;

                case ShapeType::Brush:
                    itemName = QString("Brush%1").arg(++brushCount);
                    break;

                case ShapeType::Line:
                    itemName = QString("Line%1").arg(++lineCount);
                    break;

                case ShapeType::Star:
                    itemName = QString("Star%1").arg(++starCount);
                    break;

                case ShapeType::Polygon:
                    itemName = QString("Polygon%1").arg(++polygonCount);

                case ShapeType::Heart:
                    itemName = QString("Heart%1").arg(++heartCount);

                case ShapeType::BubbleSpeech:
                    itemName = QString("Bubble%1").arg(++bubbleCount);

                }

                itemNames[kpathItem] = itemName;
            }

            kpathItem->setName(itemName);

            infos = QString("X: %1  Y: %2").arg(kpathItem->pos().x()).arg(
                kpathItem->pos().y());

            itemName += "    " + infos;

            // QListWidgetItem *listItem = new QListWidgetItem(itemName, listWidget);


            QListWidgetItem *listWidgetItem = new QListWidgetItem(listWidget);
            listWidgetItem->setFlags(listWidgetItem->flags() | Qt::ItemIsEditable);

            QWidget *widget = new QWidget();
            layoutWidget = new QHBoxLayout(widget);
            // layoutWidget->setSpacing(1);


            checkVisible = new QCheckBox();
            checkVisible->setChecked(true);

            connect(checkVisible,&QCheckBox::toggled,
                    [kpathItem](bool checked){kpathItem->setVisible(checked);
                    });

            label = new QLabel(itemName);

            layoutWidget->addWidget(checkVisible, 0, Qt::AlignLeft);
            layoutWidget->addWidget(label, 0, Qt::AlignLeft);

            //This is to push the label to be near the checkbox
            layoutWidget->addStretch(4);

            // - The `setContentsMargins` method removes extra margins around
            // the widgets inside the QHBoxLayout
            layoutWidget->setContentsMargins(0, 0, 0, 0);
            // layout->addStretch(1);


            listWidgetItem->setSizeHint(widget->sizeHint());
            listWidget->setItemWidget(listWidgetItem, widget);

            listWidgetItem->setData(Qt::UserRole,QVariant::fromValue(
                                                      static_cast<void*>(kpathItem)));
            listWidgetItem->setData(Qt::UserRole + 1,QVariant::fromValue(
                                                          static_cast<void*>(label)));

            displayedItems.insert(kpathItem);
            qDebug() << "Added item to displayedItems. Total count:" <<
                displayedItems.size();
        }
    }
}

void SceneInfoWindo::clearListItemsSelc()
{
    listWidget->clearSelection();
}


void SceneInfoWindo::selectItem(QGraphicsItem *item)
{


    if(!item)
        return;

    auto *kpathItem = dynamic_cast<KPathItem*>(item);

    if(!kpathItem)
        return;

    for(int i = 0; i < listWidget->count(); ++i) {
        QListWidgetItem *listItem = listWidget->item(i);

        KPathItem *itemInList = static_cast<KPathItem*>(
            listItem->data(Qt::UserRole).value<void*>());

        if(itemInList == kpathItem) {
            listWidget->setCurrentItem(listItem);
            break;
        }
    }
}

bool SceneInfoWindo::eventFilter(QObject *watched, QEvent *event)
{
    // qDebug() << "I am the delete of event filter called first?\n";
    if(watched == listWidget && event->type() == QEvent::KeyPress) {

        QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
        QListWidgetItem *lstItem = listWidget->currentItem();

        if(keyEvent->key() == Qt::Key_Delete && lstItem != nullptr) {

            // KPathItem *kpathItem = static_cast<KPathItem*>(lstItem->data(
                                                    // Qt::UserRole).value<void*>());
            QVariant itemData = lstItem->data(Qt::UserRole);


            if(!itemData.canConvert<KPathItem*>()) {
                qDebug() << "Cannot convert item data to KpathItem*";
                return false;
            }
            KPathItem *kpathItem = qvariant_cast<KPathItem*>(itemData);
            if(!scene->items().contains(kpathItem)) {
                qDebug() << "kpathitem  Not there in the scene\n";
            }

            if(kpathItem) {
                qDebug() << "Suprised I am 10\n";
                deleteLstItem();
                emit deleteItem(kpathItem);
                return true;
            }
        }

        if(keyEvent->key() == Qt::Key_F2) {
            startRenaming();
            return true;
        }
    }

    return QDialog::eventFilter(watched, event);
}


void SceneInfoWindo::deleteLstItem()
{
    qDebug() << __FUNCTION__ << " inside SCeneInfoClass\n";
    QListWidgetItem *selectedItem = listWidget->currentItem();

    if(!selectedItem) {
        qDebug() << "No item selected or item already deleted.";
        return;
    }

    // QGraphicsItem *item = static_cast<QGraphicsItem*>(selectedItem->data(
    //              Qt::UserRole).value<void*>());

    QVariant itemData = selectedItem->data(Qt::UserRole);
    if(!itemData.canConvert<KPathItem*>()) {
        qDebug() << "Stored item data is not a KPathItem*";
        return;
    }

    KPathItem *item = qvariant_cast<KPathItem*>(itemData);
    if(!item) {
        qDebug() << "Item already deleted of invalid\n";
        return;
    }


    emit deleteItem(item);

    int row = listWidget->row(selectedItem);
    listWidget->takeItem(row);
    delete selectedItem;
    itemNames.remove(item);


    // if(kpathItem) {
    //     // ShapeType type = kpathItem->getShapeType();
    //     // int number = extractNumFromNames(itemNames.value(kpathItem));
    //     //  // Add the number back to the available numbers for reuse
    //     // avialableNumbers[type].insert(number);
    //     itemNames.remove(kpathItem);
    // }


}


void SceneInfoWindo::onItemDeleted(QGraphicsItem *item)
{
    qDebug() << __FUNCTION__ << " in SceneInfoWindow\n";
    if(!item) {
        return;
    }

    auto kpathItem = dynamic_cast<KPathItem*>(item);
    if(!kpathItem) return;

    for(int i = 0; i < listWidget->count(); ++i) {

        QListWidgetItem *lstItem = listWidget->item(i);

        auto itmInLst = static_cast<KPathItem*>(lstItem->data(
                                          Qt::UserRole).value<void*>());

        if(itmInLst == kpathItem) {

            int row = listWidget->row(lstItem);
            listWidget->takeItem(row);          

            itemNames.remove(itmInLst);
            delete lstItem;
            break;
        }
    }
}

void SceneInfoWindo::clearList()
{
    QList<QGraphicsItem*> sceneItems = scene->items();

    for(auto *item : sceneItems) {
        emit deleteItem(item);
    }

    itemNames.clear();
    listWidget->clear();
    displayedItems.clear();

}


void SceneInfoWindo::setupButtons()
{
    btnClrList = new QPushButton(tr("Clear List"));
    btnClrList->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);

    btnRemove = new QPushButton(tr("Remove"));
    btnRemove->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);

    btnRename = new QPushButton(tr("Rename"));
    btnRename->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
}


void SceneInfoWindo::setupLayouts()
{
    layout = new QVBoxLayout(this);
    listWidget = new QListWidget(this);

    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->addWidget(btnClrList);
    // buttonLayout->addStretch(1);
    buttonLayout->addWidget(btnRemove);
    buttonLayout->addStretch(5);
    buttonLayout->addWidget(btnRename);

    layout->addWidget(listWidget);
    layout->addLayout(buttonLayout);

}


void SceneInfoWindo::setupConnections()
{
    connect(listWidget,&QListWidget::itemClicked,[this](QListWidgetItem *listItem) {
        if(!listItem)
            return;

        KPathItem *kpathItem = static_cast<KPathItem*>(listItem->data(
                                                                    Qt::UserRole).value<void*>());

        emit itemClicked(kpathItem);
    } );

    connect(listWidget,&QListWidget::currentItemChanged,[this](
                                                              QListWidgetItem *current, QListWidgetItem */*prev*/) {
        if(!current) return;

        KPathItem *kpathItem = static_cast<KPathItem*>(current->data(
                                                                   Qt::UserRole).value<void*>());

        emit itemClicked(kpathItem);
    });

    connect(listWidget,&QListWidget::itemDoubleClicked,[this](QListWidgetItem *lsItm) {
        if(!lsItm) return;

        KPathItem *kpathItem = static_cast<KPathItem*>(lsItm->data(
                                                 Qt::UserRole).value<void*>());

        emit itemDoubleClicked(kpathItem);
    });

    connect(dm,&DrawingManager::itemDeleted,this,&SceneInfoWindo::onItemDeleted);
    //    connect(dm,&DrawingManager::itemSelected,this,&SceneInfoWindo::selectItem);

    connect(dm,static_cast<void (DrawingManager::*)(QGraphicsItem*)>(
                    &DrawingManager::itemSelected), this,
            &SceneInfoWindo::selectItem);

    connect(btnClrList,&QPushButton::clicked,this,&SceneInfoWindo::clearList);
    connect(btnRemove,&QPushButton::clicked,this,&SceneInfoWindo::deleteLstItem);
    connect(listWidget,&QListWidget::itemChanged,this,&SceneInfoWindo::itemRename);
    connect(btnRename,&QPushButton::clicked,this, &SceneInfoWindo::startRenaming);

    connect(dm,&DrawingManager::itemUpdated,this,&SceneInfoWindo::onItemUpdated);
}


void SceneInfoWindo::itemRename(QListWidgetItem *item)
{

    if(!item) return;

    QString newName = item->text();

    auto *kpathItem = static_cast<KPathItem*>(item->data(
                                 Qt::UserRole).value<void*>());

    if(kpathItem) {
        kpathItem->setName(newName);
    }
}


void SceneInfoWindo::startRenaming()
{

    QListWidgetItem *selectedItem = listWidget->currentItem();

    if(selectedItem) {

        KPathItem *kpathItem = static_cast<KPathItem*>(selectedItem->data(
                                    Qt::UserRole).value<void*>());

        QLabel *label = static_cast<QLabel*>(selectedItem->data(
                                        Qt::UserRole + 1).value<void*>());



        if(kpathItem && label) {

            bool ok;

            QString newName = QInputDialog::getText(this,tr("Rename Item"),
                            tr("New name:"),QLineEdit::Normal,label->text(), &ok);

            if(ok && !newName.isEmpty()) {

                kpathItem->setName(newName);
                label->setText(newName);

                itemNames[kpathItem] = newName;
            }
        }
    }
}

void SceneInfoWindo::onItemUpdated(QGraphicsItem *item)
{
    QListWidgetItem *selectedItem = listWidget->currentItem();

    infos = infos = QString("X: %1  Y: %2").arg(item->pos().x()).arg(
        item->pos().y());

    if(!selectedItem || !item)
        return;



    QLabel *label = static_cast<QLabel*>(selectedItem->data(
                                    Qt::UserRole + 1).value<void*>());

    if(label) {
        QString originalName = itemNames.value(static_cast<KPathItem*>(item));
        QString positionInfo = QString("X: %1  Y: %2").arg(item->pos().x()).arg(
            item->pos().y());

        QString fullInfo = originalName + "          " + positionInfo;


        label->setText(fullInfo);
    }


}

// int SceneInfoWindo::extractNumFromNames(const QString &name)
// {
//     QRegularExpression re("\\d+");
//     QRegularExpressionMatch match = re.match(name);

//     if(match.hasMatch()) {
//         return match.captured(0).toInt();
//     }

//     return 0;
// }

// QString SceneInfoWindo::getNewNameForShape(ShapeType type)
// {
//     QSet<int> &numbers = avialableNumbers[type];

//     int number;

//     if(!numbers.isEmpty()) {
//         // If there are available numbers, use the smallest one
//         number = *numbers.begin();
//         numbers.remove(number);
//     } else {
//         // Otherwise, use the next number based on the count
//         switch(type) {
//         case ShapeType::Circle:
//             number = ++circleCount;
//             break;
//         }
//     }

//     switch(type) {
//         case ShapeType::Circle:
//             return QString("Circle%1").arg(number);
//         case ShapeType::Rectangle:
//             return QString("Rectangle%1").arg(number);
//         case ShapeType::Brush:
//             return QString("Brush%1").arg(number);
//         case ShapeType::Line:
//             return QString("Line%1").arg(number);
//         case ShapeType::Arrow:
//             return QString("Arrow%1").arg(number);
//         case ShapeType::Star:
//             return QString("Star%1").arg(number);
//         case ShapeType::Polygon:
//             return QString("Polygon%1").arg(number);
//     }
// }

// void SceneInfoWindo::resetVars()
// {
//     circleCount = rectCount = lineCount = brushCount = starCount =
//         arrowCount = polygonCount = 0;
// }












