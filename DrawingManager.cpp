

#include "DrawingManager.h"
#include <QGraphicsLineItem>
#include <QGraphicsRectItem>
#include <QGraphicsEllipseItem>
#include <QGraphicsView>
#include <QGraphicsPathItem>
#include <QDebug>
#include <QGraphicsDropShadowEffect>
#include <QColor>



QPointF initialPoint;

DrawingManager::DrawingManager(QGraphicsScene *scene):scene(scene),
    currentDrawMode(NoDraw)
{
    currentSz = 10;
    currColor = Qt::blue;
    opacityVal = 100;

    dashedLineTimer = new QTimer(this);
    connect(dashedLineTimer, &QTimer::timeout,this, &DrawingManager::updateDashedLines);
    // Update every 100 ms
    dashedLineTimer->start(40);

}

DrawingManager::~DrawingManager()
{
    // delete mainWindow;
}

void DrawingManager::setDrawMode(DrawMode mode)
{
    currentDrawMode = mode;
}

DrawingManager::DrawMode DrawingManager::drawMode() const
{
    return currentDrawMode;
}

void DrawingManager::drawRect(const QPointF &start, const QPointF &end)
{
    QPainterPath rectPath;
    rectPath.addRect(QRectF(start, end));
    QGraphicsPathItem *rectItem = new QGraphicsPathItem(rectPath);
    rectItem->setBrush(currColor);
    scene->addItem(rectItem);
    shapeTypeMap[rectItem] = ShapeType::Rectangle;

    selectedItems.append(rectItem);
    highlightItem(rectItem);
}

void DrawingManager::drawCircle(const QPointF &start, const QPointF &end)
{
    QPainterPath circlePath;
    circlePath.addEllipse(QRectF(start, end));
    QGraphicsPathItem *circleItem = new QGraphicsPathItem(circlePath);
    circleItem->setBrush(currColor);
    scene->addItem(circleItem);
    shapeTypeMap[circleItem] = ShapeType::Circle;

    selectedItems.append(circleItem);
    highlightItem(circleItem);
}

void DrawingManager::drawBrush(const QPointF &point)
{
    // Check if this is the beginning of a new brush stroke
    if(currentPath == nullptr) {
        // Initialize the QPainterPath with the starting point
        currentPaintPath = QPainterPath(point);

        // Create a new QGraphicsPathItem with the initialized QPainterPath
        currentPath = new QGraphicsPathItem(currentPaintPath);

        // Create a QPen object and set its color and width
        QPen pen(currColor);  // Use the current color set by the slider
        pen.setWidth(currentSz);  // Use the current size

        // Set the pen to the QGraphicsPathItem
        currentPath->setPen(pen);

        // Add the QGraphicsPathItem to the scene
        scene->addItem(currentPath);
        shapeTypeMap[currentPath] = ShapeType::Brush;
    }
    else {
        // If this is not the beginning of a new brush stroke,
        // extend the existing QPainterPath
        currentPaintPath.lineTo(point);

        // Update the QGraphicsPathItem with the extended QPainterPath
        currentPath->setPath(currentPaintPath);
    }
}


void DrawingManager::drawLine(const QPointF &start, const QPointF &end)
{
    // Create a QPainterPath object to represent the geometric path of the line
    QPainterPath linePath;

    // Move the "pen" to the starting point of the line
    // This sets the initial point for the QPainterPath
    linePath.moveTo(start);

    // Draw a line in the QPainterPath from the starting point to the ending point
    // This extends the QPainterPath to include the line segment
    linePath.lineTo(end);

    // Create a QGraphicsPathItem using the QPainterPath
    // QGraphicsPathItem is a QGraphicsItem that can be added to a QGraphicsScene
    QGraphicsPathItem *lineItem = new QGraphicsPathItem(linePath);

    // Create a QPen object to set the properties of the line, such as its width
    QPen pen(currColor);

    // Set the width of the pen to the current tool size
    // This will determine how thick the line appears when drawn
    pen.setWidth(currentSz);

    // Apply the pen settings to the QGraphicsPathItem
    // This sets how the outline of the path will be drawn
    lineItem->setPen(pen);

    // Add the QGraphicsPathItem to the QGraphicsScene
    // This makes the line appear in the scene
    scene->addItem(lineItem);
    shapeTypeMap[lineItem] = ShapeType::Line;
}

void DrawingManager::erasePart(const QPointF &point)
{
    QPainterPath erasePath;
    erasePath.addEllipse(point, currentSz/2, currentSz/2);
    // Find all the shapes that the eraser is touching
    QList<QGraphicsItem*> intersectingItems = scene->items(erasePath.boundingRect());
    // Loop through each shape that the eraser is touching
    for(QGraphicsItem *item : intersectingItems) {
        // Try to see if the shape is a path (like a line, rectangle, or circle)
        QGraphicsPathItem *pathItem = dynamic_cast<QGraphicsPathItem*>(item);
        if(pathItem) {
             // Get the existing shape of the path
            QPainterPath existingPath = pathItem->path();
            // Remove the part of the shape that the eraser is touching
            QPainterPath newPath = existingPath.subtracted(erasePath);
             // Save the old color and thickness of the shape
            QPen oldPen = pathItem->pen();
            // Save the old fill color of the shape
            QBrush oldBrush = pathItem->brush();
            scene->removeItem(pathItem);
            delete pathItem;
            // Create a new shape that doesn't include the erased part
            QGraphicsPathItem *newItem = new QGraphicsPathItem(newPath);
            newItem->setPen(oldPen);
            newItem->setBrush(oldBrush);
            scene->addItem(newItem);
        }
    }
}

void DrawingManager::setToolSize(int newSz)
{
    currentSz = newSz;
}

int DrawingManager::getToolSize() const
{
    return currentSz;
}

void DrawingManager::resetCurrentPath()
{
    currentPath = nullptr;
}


void DrawingManager::selectItem(const QPointF &point)
{
    // Get the item at the clicked point on the scene.
    QGraphicsItem *clickedItem = scene->itemAt(point, QTransform());
    //TODO
    if(clickedItem) {
        isMoveWithoutRubberBand = true; // Disable rubber band when an item is clicked
        // ... (rest of your selection code)
    } else {
        isMoveWithoutRubberBand = false; // Enable rubber band when no item is clicked
        startRubberBandSelection(point);
    }

    // Check if the Ctrl key is pressed and if an item was clicked.
    if(ctrlPresed && clickedItem) {
        // If the clicked item is already in the selected items list.
        if(selectedItems.contains(clickedItem)) {
            // Remove the highlight from the clicked item.
            removeHighlightItem(clickedItem);
            // Remove the clicked item from the selected items list.
            selectedItems.removeOne(clickedItem);
        } else {
            // Highlight the clicked item.
            highlightItem(clickedItem);
            // Add the clicked item to the selected items list.
            selectedItems.append(clickedItem);
        }
    } else {
        // If the clicked item is not already in the selected items list, clear the selection.
        if(!selectedItems.contains(clickedItem)) {
            // Remove the highlight from all currently selected items.
            for(QGraphicsItem *item : selectedItems) {
                removeHighlightItem(item);
            }
            // Clear the selected items list.
            selectedItems.clear();
            emit itemDeselected();
        }

        // If an item was clicked and it's not already in the selected items list.
        if(clickedItem && !selectedItems.contains(clickedItem)) {
            // Highlight the clicked item.
            highlightItem(clickedItem);
            // Add the clicked item to the selected items list.
            selectedItems.append(clickedItem);

            // Get the opacity of the clicked item.
            QGraphicsPathItem *pathItem = dynamic_cast<QGraphicsPathItem*>(clickedItem);
            if(pathItem) {
                int opacity;
                if(!originalOpacities.contains(clickedItem)) {
                    // Store original opacity
                    opacity = pathItem->brush().color().alpha();
                    originalOpacities[clickedItem] = opacity;
                } else {
                     // Retrieve original opacity
                    opacity = originalOpacities[clickedItem];
                }

                emit itemSelected(opacity);

            }
        }
    }

    // Update the initial point for future reference (used in moving items).
    initialPoint = point;
}


void DrawingManager::selectAllItems()
{
    deselectAllItems();

    QList<QGraphicsItem*> allItems = scene->items();
    for(auto *item : allItems) {
        highlightItem(item);
        selectedItems.append(item);
    }
}

void DrawingManager::deselectAllItems()
{
    for(auto *item : selectedItems) {
        removeHighlightItem(item);
    }
    selectedItems.clear();
}

void DrawingManager::moveSelectedItem(const QPointF &point, bool isResizing)
{
    qDebug() << "moveSelectedItem: ";

    if(isResizing) {
        qDebug() << "moveSelectedItem: isResizing";
        return;
    }

    if(selectedItems.empty()) {
        isMoveWithoutRubberBand = false;
        updateRubberBandSelection(point);
        return;
    }


    // Calculate the change in position (delta) from the initial point to the current point.
    QPointF delta = point - initialPoint;

    // Loop through each item in the selected items list.
    for(QGraphicsItem *item : selectedItems) {
        // Update the position of each selected item based on the calculated delta.
        item->setPos(item->pos() + delta);
    }

    // Update the initial point to the current point for the next move.
    initialPoint = point;
}


void DrawingManager::resizeItem(const QPointF &point, const QPointF &startPoint,
                                 Edge edge)
{

    QPointF delta = point - startPoint;  // Calculate the change in position

    for(auto *item : selectedItems) {
        auto *pathItem = dynamic_cast<QGraphicsPathItem*>(item);
        if(pathItem) {
            QPainterPath existingPath = pathItem->path();
            QPainterPath newPath;
            ShapeType shapeType = shapeTypeMap[pathItem];

            // Create a new bounding rectangle based on which edge is being dragged
            QRectF rect = existingPath.boundingRect();
            if (edge == Left) rect.setLeft(rect.left() + delta.x());
            if (edge == Right) rect.setRight(rect.right() + delta.x());
            if (edge == Top) rect.setTop(rect.top() + delta.y());
            if (edge == Bottom) rect.setBottom(rect.bottom() + delta.y());

            // Create a new path based on the new bounding rectangle
            if(shapeType == DrawingManager::ShapeType::Rectangle) {
                newPath.addRect(rect);
            } else if(shapeType == DrawingManager::ShapeType::Circle) {
                newPath.addEllipse(rect);
            }

            pathItem->setPath(newPath);  // Update the item with the new path
        }
    }
}


void DrawingManager::highlightItem(QGraphicsItem *item)
{
    QGraphicsPathItem *pathItem = dynamic_cast<QGraphicsPathItem*>(item);
    if(pathItem) {
        // Store the original pen
        originalPens[item] = pathItem->pen();
        originalOpacities[item] = pathItem->brush().color().alpha();

        QPen dashedPen = pathItem->pen();
        dashedPen.setStyle(Qt::DashLine);
        dashedPen.setWidth(1);
        dashedPen.setColor(Qt::black);
        pathItem->setPen(dashedPen);



        // Update the opacity based on the current value
        QColor color = pathItem->brush().color();
        if(originalOpacities.contains(item)) {

            color.setAlpha(originalOpacities[item]);
        } else {
            color.setAlpha(opacityVal);
        }
        pathItem->setBrush(color);
    }

    emit itemSelected(originalOpacities[item]);
}

void DrawingManager::removeHighlightItem(QGraphicsItem *item)
{
    QGraphicsPathItem *pathItem = dynamic_cast<QGraphicsPathItem*>(item);
    if(pathItem) {
        // Restore the original pen
        if(originalPens.contains(item)) {
            pathItem->setPen(originalPens[item]);
            originalPens.remove(item);
        }

        if(originalOpacities.contains(item)) {
            QColor color = pathItem->brush().color();
            color.setAlpha(originalOpacities[item]);
            pathItem->setBrush(color);
            // originalOpacities.remove(item);
        }


        // Remove any additional graphics effects
        pathItem->setGraphicsEffect(nullptr);

        // Update the opacity based on the current value
        QColor color = pathItem->brush().color();
        if(originalOpacities.contains(item)) {

            color.setAlpha(originalOpacities[item]);
        } else {
            color.setAlpha(opacityVal);
        }

        pathItem->setBrush(color);
    }
}


void DrawingManager::updateDashedLines()
{
    dashOffset = (dashOffset + 2) % 30;
    for(auto item : selectedItems) {
        QGraphicsPathItem *pathItem = dynamic_cast<QGraphicsPathItem*>(item);
        if(pathItem) {
            QPen pen = pathItem->pen();
            pen.setDashOffset(dashOffset);
            pathItem->setPen(pen);
        }
    }

}



void DrawingManager::deleteSelectedItems()
{

    for(QGraphicsItem *item : selectedItems) {

        scene->removeItem(item);
        delete item;
    }

    selectedItems.clear();
}

void DrawingManager::setBrushColor(QColor &color)
{
    currColor = color;
}


QColor DrawingManager::brushColor() const
{
    return currColor;
}

bool DrawingManager::isSelected()
{
    return !selectedItems.empty();
}

void DrawingManager::changeItemsColors(QColor color)
{

    for(QGraphicsItem *item : selectedItems) {
        QGraphicsPathItem *pathItem = dynamic_cast<QGraphicsPathItem*>(item);
        if(pathItem) {
            pathItem->setBrush(color);
        }
    }
}

void DrawingManager::startRubberBandSelection(const QPointF &point)
{
    if(!isMoveWithoutRubberBand) {

        rubberBandRect.setTopLeft(point);
        rubberBandRect.setBottomRight(point);
        isRubberBandActive = true;

        tempRubberBand = new QGraphicsRectItem(rubberBandRect);
        QPen pen(Qt::DashLine);
        pen.setColor(Qt::blue);
        tempRubberBand->setPen(pen);
        scene->addItem(tempRubberBand);
    }
}

void DrawingManager::updateRubberBandSelection(const QPointF &point)
{
    if(!isMoveWithoutRubberBand) {
        rubberBandRect.setBottomRight(point);
        tempRubberBand->setRect(rubberBandRect.normalized());

         // Clear the current selection
        for(QGraphicsItem *item : selectedItems) {
            removeHighlightItem(item);
        }
        selectedItems.clear();

         // Find and select all items that intersect with the rubber band rectangle
        QList<QGraphicsItem*> intersectingItems = scene->items(rubberBandRect);

        for(QGraphicsItem *item : intersectingItems) {
            highlightItem(item);
            selectedItems.append(item);
        }
    }
}

void DrawingManager::endRubberBandSelection()
{
    if(!isMoveWithoutRubberBand) {

        isRubberBandActive = false;

        if(tempRubberBand) {
            scene->removeItem(tempRubberBand);
        }

        rubberBandRect = QRectF();
    }
}


void DrawingManager::changeOpacityVal(int newVal)
{
    opacityVal = (newVal * 255) / 100;

    for(QGraphicsItem *item : selectedItems) {
        QGraphicsPathItem *pathItem = dynamic_cast<QGraphicsPathItem*>(item);
        if(pathItem) {
            QColor color = pathItem->brush().color();
            color.setAlpha(opacityVal);
            pathItem->setBrush(color);

            originalOpacities[item] = opacityVal;
        }
    }


}

int DrawingManager::getOpacityVal() const
{
    return opacityVal;
}


QList<QGraphicsItem*> DrawingManager::getSelectedItems() const
{
    return selectedItems;
}

void DrawingManager::setSelectedItems(QGraphicsItem *item)
{
    selectedItems.append(item);
}

DrawingManager::ShapeType DrawingManager::getShapeType(QGraphicsPathItem *pathItem) const
{

    ShapeType shapeType = shapeTypeMap[pathItem];
    return shapeType;
}



void DrawingManager::keyReleaseEvent(QKeyEvent *event)
{
    if(event->key() == Qt::Key_Control) {
        ctrlPresed = false;
        if(selectedItems.empty()) {
            isMoveWithoutRubberBand = true;
        }
    }
}


void DrawingManager::copyItems()
{
    for(auto *item : selectedItems) {
        if(!copiedItems.contains(item)) {
            copiedItems.append(item);
        }
    }

    qDebug() << "numbers of copied items: " << copiedItems.size();
}


void DrawingManager::pasteItems()
{
    //TODO Handle Shape type
    deselectAllItems();

    qDebug() << "pasteItems: ";
    for(auto *item : copiedItems) {
        if(item) {
            QGraphicsPathItem *pathItem = dynamic_cast<QGraphicsPathItem*>(item);
            if(pathItem) {
                QGraphicsPathItem *newItem = new QGraphicsPathItem(pathItem->path());
                newItem->setBrush(pathItem->brush());
                shapeTypeMap[newItem] = shapeTypeMap[pathItem];
                // Position the new item slightly below the original item
                newItem->setPos(pathItem->pos() + QPointF(0, 10));

                scene->addItem(newItem);

                selectedItems.append(newItem);
                highlightItem(newItem);
            }
        }
    }

    copiedItems.clear();
    isCopying = false;
}



void DrawingManager::keyPressEvent(QKeyEvent *event)
{
    if(event->key() == Qt::Key_Control) {
        ctrlPresed = true;
        isMoveWithoutRubberBand = false;
    }

    if(ctrlPresed && event->key() == Qt::Key_A) {
        selectAllItems();
    }

    if(ctrlPresed && event->key() == Qt::Key_D) {
        deselectAllItems();
    }

    if(ctrlPresed && event->key() == Qt::Key_V) {
        pasteItems();
    }

    if(ctrlPresed && event->key() == Qt::Key_C) {
        copyItems();
    }
}



















