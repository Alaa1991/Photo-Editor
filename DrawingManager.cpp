

#include "DrawingManager.h"
#include <QGraphicsLineItem>
#include <QGraphicsRectItem>
#include <QGraphicsEllipseItem>
#include <QGraphicsView>
#include <QGraphicsPathItem>
#include <QDebug>
#include <QGraphicsDropShadowEffect>
#include <QColor>
#include <QMessageBox>



QPointF initialPoint;

DrawingManager::DrawingManager(QGraphicsScene *scene):scene(scene),
    currentDrawMode(NoDraw)
{
    sysSz = 10;
    sysFillColor = Qt::blue;
    opacityVal = 100;

    dashedLineTimer = new QTimer(this);
    connect(dashedLineTimer, &QTimer::timeout,this, &DrawingManager::updateDashedLines);
    // Update every 100 ms
    dashedLineTimer->start(40);
    currPolygonItem = nullptr;

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
    KPathItem *krectItem = new KPathItem(rectPath);

    // rectItem->setBrush(sysFillColor);
    // QPen pen(sysBorderColor);
    // rectItem->setPen(pen);
    krectItem->setFillColor(sysFillColor);
    krectItem->setBorderColor(sysBorderColor);
    krectItem->setBorderSize(sysBorderSz);
    krectItem->setShapeType(ShapeType::Rectangle);
    krectItem->setTag(tr("Rectangle"));



    scene->addItem(krectItem);

    shapeTypeMap[krectItem] = ShapeType::Rectangle;

    selectedItems.append(krectItem);
    highlightItem(krectItem);



    emit addShapeCommand(krectItem);
}

void DrawingManager::drawCircle(const QPointF &start, const QPointF &end)
{

    QPainterPath circlePath;
    circlePath.addEllipse(QRectF(start, end));

    KPathItem *kCircleItem = new KPathItem(circlePath);

    kCircleItem->setFillColor(sysFillColor);
    kCircleItem->setBorderColor(sysBorderColor);
    kCircleItem->setBorderSize(sysBorderSz);
    kCircleItem->setShapeType(ShapeType::Circle);
    kCircleItem->setTag(tr("Circle"));

    // kCircleItem->setBrush(sysFillColor);
    // QPen pen(sysBorderColor);
    // kCircleItem->setPen(pen);
    scene->addItem(kCircleItem);
    shapeTypeMap[kCircleItem] = ShapeType::Circle;

    selectedItems.append(kCircleItem);
    highlightItem(kCircleItem);
    // kCircleItem->setPos(end.x(), end.y());

    emit addShapeCommand(kCircleItem);
}

void DrawingManager::drawBrush(const QPointF &point)
{

    // Check if this is the beginning of a new brush stroke
    if(kBrushPath == nullptr) {
        // Initialize the QPainterPath with the starting point
        currentPaintPath = QPainterPath(point);

        // Create a new QGraphicsPathItem with the initialized QPainterPath
        kBrushPath = new KPathItem(currentPaintPath);
        kBrushPath->setShapeType(ShapeType::Brush);
        // kBrushPath->setBorderColor(handBrushColor);


//        qDebug() << "handbrush color is: " << handBrushColor;
        kBrushPath->setBrushSize(sysSz);
        // Create a QPen object and set its color and width
        QPen pen(handBrushColor);  // Use the current color set by the slider
        pen.setWidth(sysSz);  // Use the current size

        // Set the pen to the QGraphicsPathItem
        kBrushPath->setPen(pen);
        // currentPath->setBrush(currColor);
        // Add the QGraphicsPathItem to the scene
        scene->addItem(kBrushPath);
        shapeTypeMap[kBrushPath] = ShapeType::Brush;

        emit addShapeCommand(kBrushPath);
    }
    else {
        // If this is not the beginning of a new brush stroke,
        // extend the existing QPainterPath
        currentPaintPath.lineTo(point);

        // Update the QGraphicsPathItem with the extended QPainterPath
        kBrushPath->setPath(currentPaintPath);
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
    KPathItem *kLineItem = new KPathItem(linePath);

    // Create a QPen object to set the properties of the line, such as its width
    QPen pen(sysBorderColor);

    // Set the width of the pen to the current tool size
    // This will deterkLineItemmine how thick the line appears when drawn
    pen.setWidth(sysSz);

    // Apply the pen settings to the QGraphicsPathItem
    // This sets how the outline of the path will be drawn
    kLineItem->setPen(pen);
    // lineItem->setBrush(currColor);

    // Add the QGraphicsPathItem to the QGraphicsScene
    // This makes the line appear in the scene
    scene->addItem(kLineItem);
    kLineItem->setShapeType(ShapeType::Line);
    shapeTypeMap[kLineItem] = ShapeType::Line;

    emit addShapeCommand(kLineItem);
}

void DrawingManager::erasePart(const QPointF &point)
{
    QPainterPath erasePath;
    erasePath.addEllipse(point, sysSz/2, sysSz/2);
    // Find all the shapes that the eraser is touching
    QList<QGraphicsItem*> intersectingItems = scene->items(erasePath.boundingRect());
    // Loop through each shape that the eraser is touching
    for(QGraphicsItem *item : intersectingItems) {
        // Try to see if the shape is a path (like a line, rectangle, or circle)
        KPathItem *kPathItem = dynamic_cast<KPathItem*>(item);
        if(kPathItem) {
             // Get the existing shape of the path
            QPainterPath existingPath = kPathItem->path();
            // Remove the part of the shape that the eraser is touching
            QPainterPath newPath = existingPath.subtracted(erasePath);
             // Save the old color and thickness of the shape
            QPen oldPen = kPathItem->pen();
            // Save the old fill color of the shape
            QBrush oldBrush = kPathItem->brush();
            scene->removeItem(kPathItem);
            delete kPathItem;
            // Create a new shape that doesn't include the erased part
            QGraphicsPathItem *newItem = new QGraphicsPathItem(newPath);
            newItem->setPen(oldPen);
            newItem->setBrush(oldBrush);
            scene->addItem(newItem);
        }
    }
}

void DrawingManager::drawPloygonPoint(const QPointF &point)
{

//    qDebug() << __FUNCTION__;
   polygonPoints.append(point);

    if(!currPolygonItem) {
        QPainterPath polygonPath;
        polygonPath.moveTo(point);

        currPolygonItem = new KPathItem(polygonPath);
        currPolygonItem->setFillColor(sysFillColor);
        currPolygonItem->setBorderColor(sysBorderColor);
        currPolygonItem->setShapeType(ShapeType::Polygon);

        // currPolygonItem->setBrush(sysFillColor);
        // QPen pen(sysBorderColor);
        // currPolygonItem->setPen(pen);

        scene->addItem(currPolygonItem);
        shapeTypeMap[currPolygonItem] = ShapeType::Polygon;

        selectedItems.append(currPolygonItem);
        highlightItem(currPolygonItem);

        emit addShapeCommand(currPolygonItem);

    } else {
        QPainterPath polygonPath;
        polygonPath.addPolygon(QPolygonF(polygonPoints));
        currPolygonItem->setPath(polygonPath);
    }

}

void DrawingManager::finishPolygon()
{
//    qDebug() << __FUNCTION__;

    // Check if there is an active polygon being drawn
    if(currPolygonItem) {
        QPainterPath finalPath;
        finalPath.addPolygon(QPolygonF(polygonPoints));
        currPolygonItem->setPath(finalPath);

        polygonPoints.clear();
        currPolygonItem = nullptr;
    }
}

void DrawingManager::drawFreehand(const QPointF &point)
{
    // Check if this is the beginning of a new freehand drawing
    if(!currentFreehandPath) {
        currentFreehandPath = std::make_unique<QPainterPath>(point);

        currFreehandItm = std::make_unique<KPathItem>(*currentFreehandPath);
        currFreehandItm->setFillColor(sysFillColor);
        currFreehandItm->setBorderSize(sysSz);
        // QPen pen(sysFillColor);
        // pen.setWidth(sysSz);
        // currFreehandItm->setPen(pen);

        scene->addItem(currFreehandItm.get());
        shapeTypeMap[currFreehandItm.get()] = ShapeType::Freehand;

        emit addShapeCommand(currFreehandItm.get());
    } else {
        // If continuing a freehand drawing, extend the existing path
        currentFreehandPath->lineTo(point);
        currFreehandItm->setPath(*currentFreehandPath);
    }
}


void DrawingManager::drawArrow(const QPointF &start, const QPointF &end)
{/*
    qDebug() << __FUNCTION__;*/
    // Create a QPainterPath for the arrow
    QPainterPath arrowPath;
    arrowPath.moveTo(start);

    QLineF line(start, end);
    double angle = std::atan2(-line.dy(), line.dx());

    // Define the head of the arrow
    QPointF arrowP1 = line.p2() - QPointF(sin(angle + M_PI /3) * 20,
                                          cos(angle + M_PI/3) * 20);
    QPointF arrowP2 = line.p2() - QPointF(sin(angle + M_PI - M_PI /3) * 20,
                                          cos(angle + M_PI - M_PI/3) * 20);

    arrowPath.lineTo(end);
    arrowPath.moveTo(end);
    arrowPath.lineTo(arrowP1);
    arrowPath.moveTo(end);
    arrowPath.lineTo(arrowP2);

    KPathItem *arrowItem = new KPathItem(arrowPath);

    QPen pen(sysFillColor);
    pen.setWidth(sysSz);
    arrowItem->setPen(pen);
    arrowItem->setBrush(sysFillColor);
    arrowItem->setShapeType(ShapeType::Arrow);
    scene->addItem(arrowItem);
    shapeTypeMap[arrowItem] = ShapeType::Arrow;

    selectedItems.append(arrowItem);


    emit addShapeCommand(arrowItem);
}


void DrawingManager::drawStar(const QPointF &center,double radius,int points)
{
//    qDebug() << __FUNCTION__;
    if(points < 5)
        points = 5;

    QPainterPath starPath;
    double angleStep = M_PI / points;
    double startAngle = M_PI / 2;

    starPath.moveTo(center.x() + radius * cos(startAngle),
                    center.y() - radius * sin(startAngle));

    for(int i = 1; i <= points * 2; ++i) {
        double angle = startAngle - angleStep * i;
        double modRadius = i % 2 == 0? radius : radius /2;

        QPointF nextPoint(center.x() + modRadius * cos(angle),
                          center.y() - modRadius * sin(angle));
        starPath.lineTo(nextPoint);
    }

    // Close the path to complete the star
    starPath.closeSubpath();

    KPathItem *starItem = new KPathItem(starPath);

    QPen pen(sysBorderColor);
//    qDebug() << __FUNCTION__ << " border sz: " << sysBorderSz;
    pen.setWidth(sysBorderSz);
    starItem->setPen(pen);
    starItem->setBrush(sysFillColor);
    starItem->setShapeType(ShapeType::Star);

    scene->addItem(starItem);
    shapeTypeMap[starItem] = ShapeType::Star;

    selectedItems.append(starItem);
    highlightItem(starItem);

    emit addShapeCommand(starItem);
}

QPainterPath DrawingManager::createStarPath(const QPointF &center,
                                    double radius, int points)
{
    if(points < 5) {
        points = 5;
    }

    QPainterPath starPath;
    double angleStep = M_PI / points;
    double startAngle = M_PI / 2;


    // Move to the first point of the star
    starPath.moveTo(center.x() + radius * cos(startAngle),
                    center.y() - radius * sin(startAngle));

    // Create the star shape by connecting points
    for(int i = 1; i <= points * 2; ++i) {
        double angle = startAngle - angleStep * i;
        double modRadius = i % 2 == 0 ? radius : radius / 2;

        QPointF nextPoint(center.x() + modRadius *cos(angle),
                          center.y() - modRadius *sin(angle));

        starPath.lineTo(nextPoint);
    }

    // Close the path to complete the star shape
    starPath.closeSubpath();

    return starPath;
}


void DrawingManager::setToolSize(int newSz)
{
    sysSz = newSz;
}

int DrawingManager::getToolSize() const
{
    return sysSz;
}

void DrawingManager::resetCurrentPath()
{
    kBrushPath = nullptr;
}


void DrawingManager::selectItem(const QPointF &point)
{
    // Get the item at the clicked point on the scene.
    QGraphicsItem *clickedItem = scene->itemAt(point, QTransform());
    KPathItem *kpathItem = dynamic_cast<KPathItem*>(clickedItem);
    bool isLocked = (kpathItem != nullptr) && kpathItem->getLocked();
    //TODO
    if(clickedItem) {
        isMoveWithoutRubberBand = true; // Disable rubber band when an item is clicked
        // ... (rest of your selection code)
    } else {
        isMoveWithoutRubberBand = false; // Enable rubber band when no item is clicked
        startRubberBandSelection(point);
    }
    //in case the item belong to a group, selected all of the group items
    CustomGroup *group = findGroup(clickedItem);
    if(group) {
        for(auto *item : group->items) {
            if(selectedItems.contains(item)) {

                removeHighlightItem(item);
                selectedItems.removeOne(item);
            } else {
                highlightItem(item);
                selectedItems.append(item);
            }
        }
    }

    // Check if the Ctrl key is pressed and if an item was clicked.
    if(ctrlPresed && clickedItem) {
        if(isLocked) {

            return;
        }
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
            emit textDeselected();
        }

        // If an item was clicked and it's not already in the selected items list.
        if(clickedItem && !selectedItems.contains(clickedItem)) {
            // Highlight the clicked item.
            highlightItem(clickedItem);
            // Add the clicked item to the selected items list.
            selectedItems.append(clickedItem);


            // Get the opacity and border size of the clicked item.
            // KPathItem *kpathItem = dynamic_cast<KPathItem*>(clickedItem);
            QGraphicsTextItem *textItem = dynamic_cast<QGraphicsTextItem*>(clickedItem);
            if(kpathItem) {
                int opacity;
                int borderSz = kpathItem->pen().width();

                if(!originalOpacities.contains(clickedItem)) {
                    // Store original opacity
                    opacity = kpathItem->brush().color().alpha();
                    originalOpacities[clickedItem] = opacity;
                } else {
                     // Retrieve original opacity
                    opacity = originalOpacities[clickedItem];
                }

                emit itemSelected(opacity,clickedItem);
                emit itemSelected(clickedItem);
                emit borderSzChanged(borderSz);
            }

            if(textItem) {
                qDebug() << "selectedItem: textItem";
                emit textSelected();
            }
        }
    }


    // Update the initial point for future reference (used in moving items).
    initialPoint = point;
}

//OVERLOAED to handled the selection made by the qlistwidget
void DrawingManager::selectItem(QGraphicsItem *item)
{
    auto kpathItem = dynamic_cast<KPathItem*>(item);

    if(!kpathItem) {
        return;
    }

    bool isLocked = kpathItem->getLocked();
    if(isLocked) {
        return;
    }

    if(selectedItems.contains(kpathItem)) {
        removeHighlightItem(kpathItem);
        selectedItems.removeOne(kpathItem);
        emit itemDeselected();

    } else {
        highlightItem(kpathItem);
        selectedItems.append(kpathItem);


        int op = kpathItem->brush().color().alpha();
        int brSz = kpathItem->getBorderSize();

        emit itemSelected(op, kpathItem);
        emit borderSzChanged(brSz);
    }
}



void DrawingManager::selectAllItems()
{

    deselectAllItems();

    QList<QGraphicsItem*> allItems = scene->items();
    for(auto *item : allItems) {

        KPathItem *kpathItem = dynamic_cast<KPathItem*>(item);
        if(kpathItem) {
            if(!kpathItem->getLocked()) {
                highlightItem(item);
                selectedItems.append(item);
            }
        }
    }
}

void DrawingManager::deselectAllItems()
{
    for(auto *item : selectedItems) {
        removeHighlightItem(item);
    }
    selectedItems.clear();
}

void DrawingManager::moveSelectedItem(const QPointF &point,bool isResizing)
{
    if(isResizing) {
//        qDebug() << "moveSelectedItem: isResizing";
        return;
    }
    // Calculate the change in position (delta) from the initial point to the current point.
    QPointF delta = point - initialPoint;
    QSet<CustomGroup*> movedGroups;
    // Loop through each item in the selected items list.
    for(QGraphicsItem *item : selectedItems) {
        // Update the position of each selected item based on the calculated delta.
        // item->setPos(item->pos() + delta);
        KPathItem *kpathItem = dynamic_cast<KPathItem*>(item);
        QGraphicsTextItem *textItem = dynamic_cast<QGraphicsTextItem*>(item);
        CustomGroup *group = findGroup(item);

        if(group  && !movedGroups.contains(group)) {
            // qDebug() << "moving group";
            if(group->items.first() == item) {

                moveGroup(*group, delta);
                movedGroups.insert(group);
            }


        } else if(!group) {
            // qDebug() << "Moving individual item.";
            if(kpathItem) {
                if(!kpathItem->getLocked()) {
                    kpathItem->setPos(kpathItem->pos() + delta);         
                    emit itemUpdated(kpathItem);
                } else {
                    qDebug() << "Item is locked";
                    return;
                }
            }

            if(textItem) {
                textItem->setPos(textItem->pos() + delta);
            }
        }
    }
    // Update the initial point to the current point for the next move.
    initialPoint = point;
}


void DrawingManager::resizeItem(const QPointF &point, const QPointF &startPoint,
                                Edge edge)
{
    // qDebug() << __FUNCTION__;
    QPointF delta = point - startPoint;  // Calculate the change in position

    for(auto *item : selectedItems) {
        auto *pathItem = dynamic_cast<KPathItem*>(item);
        auto *textItem = dynamic_cast<QGraphicsTextItem*>(item);
        // CustomGroup *group = findGroup(item);

        if(pathItem) {
            if(pathItem->getLocked()) {
            qDebug() << "pathitem is locked";
            return;
            }

            QPainterPath existingPath = pathItem->path();
            QPainterPath newPath;
            ShapeType shapeType = shapeTypeMap[pathItem];


            // Create a new bounding rectangle based on which edge is being dragged
            QRectF rect = existingPath.boundingRect();
            if (edge == Left) rect.setLeft(rect.left() + delta.x());
            if (edge == Right) rect.setRight(rect.right() + delta.x());
            if (edge == Top) rect.setTop(rect.top() + delta.y());
            if (edge == Bottom) rect.setBottom(rect.bottom() + delta.y());


            // if(shapeType == ShapeType::Heart) {
            //     qDebug() << "Heart ";
            //     resizeHeartShape(pathItem, point);
            // }

            switch(shapeType) {

            case ShapeType::Rectangle:
                qDebug() << "Rect ";
                newPath.addRect(rect);
                break;

            case ShapeType::Circle:
                newPath.addEllipse(rect);
                break;

            case ShapeType::Heart:
                qDebug() << "Heart ";
                resizeHeartShape(pathItem, point );
                continue;

            case ShapeType::BubbleSpeech:
                newPath = createScaledBubblePath(rect);
                break;
            }

            pathItem->setPath(newPath);  // Update the item with the new path
            pathItem->setLastModifiedTime(QDateTime::currentDateTime());
            emit itemUpdated(pathItem);
        }

        else if (textItem) {
//            qDebug() << __FUNCTION__ << "textItem";
            QFont currentFont = textItem->font();
            int currentFontSz = currentFont.pointSize();

            int newFontSz = qMax(1, currentFontSz+static_cast<int>(delta.y()));
            currentFont.setPointSize(newFontSz);
            textItem->setFont(currentFont);
            emit textSelected();
//            qDebug() << __FUNCTION__ << "emit textSelected";
//            emit itemUpdated(item);
        }
    }
}


void DrawingManager::resizeHeartShape(KPathItem* heartItem,const QPointF &currentMousePos)


{
    static QPointF previousMousePos = currentMousePos; // Initialize with current pos on first call

    // Calculate the movement vector since the last update
    QPointF movementVector = currentMousePos - previousMousePos;

    // Determine the incremental scale factor based on the movement vector
    // This factor controls how sensitively the shape responds to mouse movement
    qreal incrementalScaleFactor = 1.0 + (sqrt(pow(movementVector.x(), 2) + pow(movementVector.y(), 2)) / 100.0);

    // Determine the direction of resizing (enlarge or reduce)
    bool shouldEnlarge = (movementVector.x() > 0 && movementVector.y() > 0) || (movementVector.x() < 0 && movementVector.y() < 0);

    // Apply the scale factor conditionally based on the direction
    QRectF boundingRect = heartItem->path().boundingRect();
    qreal newSize = shouldEnlarge ? boundingRect.width() * incrementalScaleFactor : boundingRect.width() / incrementalScaleFactor;

    // Ensure newSize is within reasonable bounds
    newSize = std::max(10.0, std::min(newSize, 1000.0)); // Example bounds

    // Update the heart shape with the new size
    QPainterPath newPath = createHeartPath(boundingRect.center(), newSize);
    heartItem->setPath(newPath);

    // Update previousMousePos for the next iteration
    previousMousePos = currentMousePos;
}



QPainterPath DrawingManager::createScaledBubblePath(const QRectF &rect)
{
    QPainterPath path;



    return path;
}



void DrawingManager::highlightItem(QGraphicsItem *item)
{
//     qDebug() << __FUNCTION__;
    KPathItem *pathItem = dynamic_cast<KPathItem*>(item);

     if(pathItem) {
        // Store the original pen
        QPen pen = pathItem->pen();
        int penWidth = pathItem->pen().width();
        pen.setWidth(penWidth);

        originalPens[item] = pen;
        originalBrushes[item] = pathItem->brush();
        originalOpacities[item] = pathItem->brush().color().alpha();

        QPen dashedPen = pathItem->pen();
        // dashedPen.setWidth(penWidth);
        dashedPen.setStyle(Qt::DashLine);

        if(penWidth <= 1) {
            // dashedPen.setColor(Qt::black);
        }

         pathItem->setPen(dashedPen);


        // Update the opacity based on the current value
        QColor color = pathItem->brush().color();
        if(originalOpacities.contains(item)) {

            color.setAlpha(originalOpacities[item]);
        } else {
            color.setAlpha(opacityVal);
        }
        //TO solve selecting brush lines black shadows
        if(shapeTypeMap[pathItem] != ShapeType::Brush)
              pathItem->setBrush(color);
    }

    QGraphicsTextItem *textItem = dynamic_cast<QGraphicsTextItem*>(item);

    if(textItem) {

        auto *effect = new QGraphicsDropShadowEffect;
        effect->setBlurRadius(15);
        effect->setColor(Qt::blue);
        effect->setOffset(2, 2);
        textItem->setGraphicsEffect(effect);
    }
    emit itemSelected(originalOpacities[item], item);
}

void DrawingManager::removeHighlightItem(QGraphicsItem *item)
{
//     qDebug() << __FUNCTION__;
    KPathItem *pathItem = dynamic_cast<KPathItem*>(item);
    auto *textItem = dynamic_cast<QGraphicsTextItem*>(item);

    if(pathItem) {
        // Restore the original pen
        if(originalPens.contains(item)) {
//            qDebug() << __FUNCTION__ << " pen width " << pathItem->pen().width();
            pathItem->setPen(originalPens[item]);
            originalPens.remove(item);
        }

        if(originalBrushes.contains(item)) {
            if(shapeTypeMap[pathItem] != ShapeType::Brush)
                pathItem->setBrush(originalBrushes[item]);
            originalBrushes.remove(item);
        }

        if(originalOpacities.contains(item)) {
            QColor color = pathItem->brush().color();
            color.setAlpha(originalOpacities[item]);
            if(shapeTypeMap[pathItem] != ShapeType::Brush)
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

        if(shapeTypeMap[pathItem] != ShapeType::Brush)
            pathItem->setBrush(color);
    }

    if(textItem && textItem->graphicsEffect()) {
        delete textItem->graphicsEffect();
        textItem->setGraphicsEffect(nullptr);
    }
}


void DrawingManager::updateDashedLines()
{

//     qDebug() << __FUNCTION__;
    dashOffset = (dashOffset + 2) % 30;

    for(auto item : selectedItems) {
        KPathItem *pathItem = dynamic_cast<KPathItem*>(item);
        if(pathItem) {
            QPen pen = pathItem->pen();
            int penWidth = pathItem->pen().width();

            if(penWidth <= 1) {
                pen.setDashOffset(dashOffset);
            }
            else {
                pen.setDashOffset(0);
            }
            pathItem->setPen(pen);
        }
    }

}



void DrawingManager::deleteSelectedItems()
{
//    qDebug() << __FUNCTION__;
    if(selectedItems.isEmpty())
        return;


    for(QGraphicsItem *item : selectedItems) {
        // emit removeShapeCommand(item);
//        KPathItem *kpathItem = dynamic_cast<KPathItem*>(item);
//        if(kpathItem && kpathItem->getLocked()) {
//            continue;
//        }
//        emit itemDeleted(item);

//        scene->removeItem(item);
//        delete item;
//        selectedItems.removeOne(item);
        deleteItem(item);

    }

    // selectedItems.clear();
}

void DrawingManager::deleteItem(QGraphicsItem *item)
{
//     qDebug() << __FUNCTION__;

//    KPathItem *kpathItem = dynamic_cast<KPathItem*>(item);

//    if(kpathItem && !kpathItem->getLocked()) {

//        emit itemDeleted(item);
//        scene->removeItem(item);
//        delete item;
//        selectedItems.removeOne(item);
//    }

     CustomGroup *group = findGroup(item);

     if(group) {
        int ret = QMessageBox::question(nullptr,"Delete Group",
        "Do you want to delte the whole group?",QMessageBox::Yes | QMessageBox::No);


        if(ret == QMessageBox::Yes) {
            // Delete the whole group
            for(auto *groupItem : group->items) {
                deleteSingleItem(groupItem);
            }
            // Remove the group from the list
            customGroups.removeOne(group);
            delete group;
        } else {
            // Delete only the selected item
            deleteSingleItem(item);
            // Remove item from the group
            group->items.removeOne(item);
        }

     } else {
        // Handle non-grouped items
        deleteSingleItem(item);
     }
}

void DrawingManager::deleteSingleItem(QGraphicsItem *item)
{
//     qDebug() << __FUNCTION__;
     KPathItem *kpathItem = dynamic_cast<KPathItem*>(item);
     QGraphicsTextItem *textItem = dynamic_cast<QGraphicsTextItem*>(item);

     if(kpathItem && !kpathItem->getLocked()) {
        emit itemDeleted(item);
        scene->removeItem(item);
        delete item;
        selectedItems.removeOne(item);
     } else if(textItem) {
        emit itemDeleted(item);
        scene->removeItem(item);
        delete item;
        selectedItems.removeOne(item);
     }
}

void DrawingManager::setSysFillColor(QColor &color)
{
    sysFillColor = color;
}

void DrawingManager::setBorderColor(QColor &color)
{
    sysBorderColor = color;
}



// bool DrawingManager::isSelected()
// {
//     return !selectedItems.empty();
// }


QColor DrawingManager::brushColor() const
{
    return sysFillColor;
}

QColor DrawingManager::getBorderColor() const
{
    return sysBorderColor;
}

void DrawingManager::changeItemsFillColor(QColor &color)
{

    for(QGraphicsItem *item : selectedItems) {
        KPathItem *pathItem = dynamic_cast<KPathItem*>(item);
        if(pathItem && !pathItem->getLocked()) {
            // pathItem->setBrush(color);
            pathItem->setFillColor(color);
            pathItem->setLastModifiedTime(QDateTime::currentDateTime());
            originalBrushes[pathItem] = color;
        }
    }
}


void DrawingManager::chageItemsBorderColor(QColor &color)
{

    for(auto *item : selectedItems) {
        auto *pathItem = dynamic_cast<KPathItem*>(item);
        if(pathItem && !pathItem->getLocked()) {
            // QPen pen(color);
            // pen.setWidth(pathItem->pen().width());
            // pathItem->setPen(pen);
            pathItem->setBorderColor(color);
            originalPens[pathItem] = pathItem->pen();
            pathItem->setLastModifiedTime(QDateTime::currentDateTime());
            emit itemSelected(originalOpacities[item], item);

        }
    }
}

void DrawingManager::changeBorderSz(int sz)
{

    for(auto *item : selectedItems) {
        auto *pathItem = dynamic_cast<KPathItem*>(item);
        if(pathItem && !pathItem->getLocked()) {
            QPen pen(pathItem->pen().color());
            pen.setWidth(sz);
            pathItem->setPen(pen);
            // pathItem->setBorderSize(sz);
            originalPens[pathItem] = pen;
            pathItem->setLastModifiedTime(QDateTime::currentDateTime());
            // emit borderSzChanged(sz);
        }
    }
}

int DrawingManager::getSysBorderSz() const
{
    return sysBorderSz;
}

void DrawingManager::setSysBorderSz(int size)
{
    sysBorderSz = size;
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
        KPathItem *pathItem = dynamic_cast<KPathItem*>(item);
        if(pathItem && !pathItem->getLocked()) {
            QColor color = pathItem->brush().color();
            color.setAlpha(opacityVal);
            // pathItem->setItemOpacity(opacityVal);
            if(shapeTypeMap[pathItem] != ShapeType::Brush) {
                qDebug() << "not brush so won't spoint it";
                pathItem->setBrush(color);
                pathItem->setLastModifiedTime(QDateTime::currentDateTime());

            } else {

                QMessageBox::warning(nullptr, "Opacity Slider Warning",
    "Opacity Slider Not Applicable to Brush and othe Shapes that uses line path");
            }

            originalOpacities[item] = opacityVal;
        }
    }


}

//TODO remove it, now it's handled in KPathItem class
int DrawingManager::getOpacityVal() const
{
    return opacityVal;
}


QList<QGraphicsItem*> DrawingManager::getSelectedItems() const
{
    return selectedItems;
}

void DrawingManager::appendSelectedItems(QGraphicsItem *item)
{
    selectedItems.append(item);
}

ShapeType DrawingManager::getShapeType(KPathItem *pathItem) const
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
            emit itemCopied();
        }
    }

//    qDebug() << "numbers of copied items: " << copiedItems.size();
}


void DrawingManager::pasteItems()
{
    //TODO Handle Shape type
    deselectAllItems();

//    qDebug() << "pasteItems: ";
    for(auto *item : copiedItems) {
        if(item) {
            KPathItem *pathItem = dynamic_cast<KPathItem*>(item);
            //TODO
            QGraphicsTextItem *textItem = dynamic_cast<QGraphicsTextItem*>(item);

            if(pathItem) {
                KPathItem *newItem = new KPathItem(pathItem->path());
                newItem->setBrush(pathItem->brush());
                newItem->setPen(pathItem->pen());
                newItem->setItemOpacity(pathItem->getItemOpacity());
                shapeTypeMap[newItem] = shapeTypeMap[pathItem];
                // Position the new item slightly below the original item
                newItem->setPos(pathItem->pos() + QPointF(0, 10));

                scene->addItem(newItem);
                emit addShapeCommand(newItem);
                selectedItems.append(newItem);
                highlightItem(newItem);
            }

            if(textItem) {
                qDebug() << __FUNCTION__ << " textItem not null";
                QGraphicsTextItem *newTextItem = new QGraphicsTextItem(
                    textItem->toPlainText());

                newTextItem->setFont(textItem->font());

                newTextItem->setDefaultTextColor(textItem->defaultTextColor());

                newTextItem->setPos(textItem->pos() + QPointF(0, 10));
                scene->addItem(newTextItem);
                selectedItems.append(newTextItem);
                highlightItem(newTextItem);
            }
        }
    }

    copiedItems.clear();
    isCopying = false;
    emit itemCopied();
}



void DrawingManager::keyPressEvent(QKeyEvent *event)
{
    if(event->key() == Qt::Key_Control) {
        ctrlPresed = true;
        isMoveWithoutRubberBand = false;
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


void DrawingManager::setHandBrushColor(QColor &color)
{
//    qDebug() << __FUNCTION__;
    handBrushColor = color;
}

void DrawingManager::setStarHeads(const int& heads)
{
    starHeads = heads;
}


QColor DrawingManager::getHandBrushColor() const
{
    return handBrushColor;
}



void DrawingManager::setIsLocked(bool val)
{
    for(auto *item : selectedItems) {
        KPathItem *kpathItem = dynamic_cast<KPathItem*>(item);
        if(kpathItem) {
            kpathItem->setLocked(val);
        }
    }
}


void DrawingManager::changeShapeSz(qreal width, qreal height)
{
    for(auto *item : selectedItems) {
        auto *kpathItem = dynamic_cast<KPathItem*>(item);
        if(kpathItem) {
            kpathItem->setSize(width, height);
        }
    }

}

void DrawingManager::changeShapePos(qreal x, qreal y)
{
    if(!selectedItems.empty()) {

        auto *selectedItem = selectedItems.first();

        auto kpathItem = dynamic_cast<KPathItem*>(selectedItem);

        if(kpathItem) {
            kpathItem->setPos(x, y);
        }
    }
}


void DrawingManager::resizeArrow(QGraphicsPathItem *item, const QPointF &delta, Edge edge) {
//    qDebug() << "Inside resizeArrow";
    QPainterPath path = item->path();
    QPointF start = path.elementAt(0);  // Assuming the start point is the first element
    QPointF end = path.elementAt(1);    // Assuming the end point is the second element

    // Calculate new end point based on the edge being resized
    if (edge == Right) {
        end += QPointF(delta.x(), 0);  // Horizontal resize
    } else if (edge == Bottom) {
        end += QPointF(0, delta.y());  // Vertical resize
    }

    // Rebuild the arrow path with new dimensions
    QPainterPath newPath;
    newPath.moveTo(start);
    newPath.lineTo(end);
    // Add logic to resize the arrowhead here if needed

    item->setPath(newPath);  // Update the item with the new path
}

QList<QGraphicsItem*> DrawingManager::getCopiedItems() const
{
    return copiedItems;
}

void DrawingManager::groupItems()
{
    if(selectedItems.size() < 2)
        return;

    CustomGroup *existingGroup = nullptr;

    for(auto *item : selectedItems) {
        existingGroup = findGroup(item);
        if(existingGroup)
            break;
    }

    if(existingGroup) {
        // Add new items to the existing group
        for(auto *item : selectedItems) {
            if(!existingGroup->items.contains(item)) {
                existingGroup->items.append(item);
            }
            removeHighlightItem(item);
        }
    } else {
        // Create a new group
        CustomGroup *newGroup = new CustomGroup;
        for(auto *item : selectedItems) {
//            newGroup.items.append(item);
            newGroup->items.append(item);
            removeHighlightItem(item);
        }

        customGroups.append(newGroup);
    }

    selectedItems.clear();

}


void DrawingManager::ungroupItems()
{

//    if(customGroups.isEmpty())
//        return;

    if(selectedItems.isEmpty())
        return;

    QSet<CustomGroup*> groupsToUngroup;
    // Identify groups that need to be ungrouped
    for(auto *item : selectedItems) {
        CustomGroup *group = findGroup(item);
        if(group)
            groupsToUngroup.insert(group);
    }

    // Ungroup the identified groups
    for(auto *group : groupsToUngroup) {
        qDebug() << "Ungrouping items from group:" << group;
        for(auto *item : group->items) {
            qDebug() << "Removing item from group:" << item;
            item->setData(Qt::UserRole, QVariant());
            removeHighlightItem(item);
            // Add item back to selectedItems if it's still selected

                selectedItems.append(item);
                qDebug() << "Adding item back to selectedItems:" << item;

        }

        // Remove the group from customGroups
        customGroups.removeOne(group);
        delete group;
        qDebug() << "Removing group from customGroups:" << group;
        qDebug() << "number of groups inside customeGroups " << customGroups.count();

    }

//    CustomGroup ungroup = customGroups.takeLast();

//    for(auto *item : ungroup.items) {
//        selectedItems.append(item);
//        removeHighlightItem(item);
//    }
}

DrawingManager::CustomGroup *DrawingManager::findGroup(QGraphicsItem *item)
{
    for(auto &group : customGroups) {
        if(group->items.contains(item))
            return group;
    }
    return nullptr;
}

void DrawingManager::moveGroup(CustomGroup &group, const QPointF &delta)
{
    for(auto *item : group.items) {
        item->setPos(item->pos() + delta);
    }
}

void DrawingManager::clearGroups()
{
    customGroups.clear();
}


void DrawingManager::drawHeart(const QPainterPath &heartPath)
{


    KPathItem *heartItem = new KPathItem(heartPath);
    QPen pen(sysBorderColor);
    pen.setWidth(sysBorderSz);
    heartItem->setPen(pen);
    heartItem->setBrush(sysFillColor);
    heartItem->setShapeType(ShapeType::Heart);

    shapeTypeMap[heartItem] = ShapeType::Heart;
    scene->addItem(heartItem);

    selectedItems.append(heartItem);
    highlightItem(heartItem);

    emit addShapeCommand(heartItem);
}

void DrawingManager::drawSpeechBubble(const QPainterPath &bubblePath)
{

    KPathItem *bubbleItem = new KPathItem(bubblePath);
    QPen pen(sysBorderColor);
    pen.setWidth(sysBorderSz);
    bubbleItem->setPen(pen);
    bubbleItem->setBrush(sysFillColor);
    bubbleItem->setShapeType(ShapeType::BubbleSpeech);
    shapeTypeMap[bubbleItem] = ShapeType::BubbleSpeech;

    scene->addItem(bubbleItem);

    selectedItems.append(bubbleItem);
    highlightItem(bubbleItem);

    emit addShapeCommand(bubbleItem);
}

QPainterPath DrawingManager::createHeartPath(const QPointF &center, double size)
{
    QPainterPath path;

    // Start point for the top left of the heart
    QPointF startPointLeft(center.x(), center.y() - size / 4);

    // Control points and end point for the left half of the heart
    QPointF leftControlPoint1(center.x() - size / 2, center.y() - size / 2);
    QPointF leftControlPoint2(center.x() - size / 2, center.y() + size / 4);
    QPointF bottomPoint(center.x(), center.y() + size / 2);

    // Control points for the right half of the heart
    QPointF rightControlPoint1(center.x() + size / 2, center.y() + size / 4);
    QPointF rightControlPoint2(center.x() + size / 2, center.y() - size / 2);

    // Draw the left half
    path.moveTo(startPointLeft);
    path.cubicTo(leftControlPoint1, leftControlPoint2, bottomPoint);

    // Draw the right half
    path.moveTo(startPointLeft);
    path.cubicTo(rightControlPoint2, rightControlPoint1, bottomPoint);

    return path;
}

QPainterPath DrawingManager::createBubblePath(const QRectF &rect)
{
    QPainterPath path;
    path.addRoundedRect(rect,20,20);

    // Triangle for the "tail" of the bubble
    QPointF triangleBaseCenter(rect.center().x(), rect.bottom());
    QPointF trianglePoint1 = triangleBaseCenter + QPointF(-20, 0);
    QPointF trianglePoint2 = triangleBaseCenter + QPointF(20, 0);
    QPointF trianglePoint3 = triangleBaseCenter + QPointF(0, 30);

    path.moveTo(trianglePoint1);
    path.lineTo(trianglePoint3);
    path.lineTo(trianglePoint2);
    path.lineTo(trianglePoint1);

    return path;
}



















