
#include "EventsManager.h"


EventsManager::EventsManager(QGraphicsScene *scene, DrawingManager *dm):
    scene(scene), drawingManager(dm)
{
    // drawingManager = new DrawingManager(scene);
}

EventsManager::~EventsManager()
{

}

void EventsManager::setStr(const QString &newStr)
{
    str = newStr;
}

void EventsManager::onViewMousePressed(QMouseEvent *event)
{


    QPointF scenePoint;

    if(mapToSceneFunc) {

        // QPointF scenePoint = ui->graphicsView->mapToScene(event->pos());
         scenePoint = mapToSceneFunc(event);

    } else {
        qDebug() << "null function pointer";
    }

    startPoint = scenePoint;


    if(drawingManager->drawMode() == DrawingManager::DrawBrush) {
        drawingManager->drawBrush(scenePoint);
    }

     if(drawingManager->drawMode() == DrawingManager::DrawRect) {

        rectStartPoint = scenePoint;
        tempRect = new QGraphicsRectItem(QRectF(rectStartPoint,rectStartPoint));
        // tempRect->setBrush(drawingManager->brushColor());
        scene->addItem(tempRect);
    }

    else if(drawingManager->drawMode() == DrawingManager::DrawCircle) {

        circleStartPoint = scenePoint;
        tempCircle = new QGraphicsEllipseItem(QRectF(circleStartPoint,circleStartPoint));
        // tempCircle->setBrush(drawingManager->brushColor());
        scene->addItem(tempCircle);
    }

    else if(drawingManager->drawMode() == DrawingManager::DrawLine) {

        lineStartPoint = scenePoint;
    }

    else if(drawingManager->drawMode() == DrawingManager::Eraser) {
        drawingManager->erasePart(scenePoint);
    }

    else if(drawingManager->drawMode() == DrawingManager::MoveTool) {

        drawingManager->selectItem(scenePoint);
        // drawingManager->startRubberBandSelection(scenePoint);

    }

    else if(drawingManager->drawMode() == DrawingManager::DrawPolygon) {

        drawingManager->drawPloygonPoint(scenePoint);
    }

    else if(drawingManager->drawMode() == DrawingManager::DrawArrow) {

        arrowStartPoint = scenePoint;
        starStartPoint = scenePoint;
        QPainterPath tempPath;
        tempPath.moveTo(arrowStartPoint);
        tempPath.lineTo(arrowStartPoint);

        tempArrow = new QGraphicsPathItem(tempPath);
        tempArrow->setPen(QPen(drawingManager->brushColor(), drawingManager->getToolSize()));
        scene->addItem(tempArrow);
    }

    else if(drawingManager->drawMode() == DrawingManager::DrawStar) {

        tempStar = new QGraphicsPathItem();
        tempStar->setPen(QPen(drawingManager->brushColor(), drawingManager->getToolSize()));
        tempStar->setBrush(drawingManager->brushColor());
        scene->addItem(tempStar);
    }

    else if(drawingManager->drawMode() == DrawingManager::DrawHeart) {
        heartStartPoint = scenePoint;
        tempHeart = new QGraphicsPathItem();
        scene->addItem(tempHeart);
    }

    else if(drawingManager->drawMode() == DrawingManager::DrawBubble) {
        bubbleStartPoint = scenePoint;
        tempBubble = new QGraphicsPathItem();
        scene->addItem(tempBubble);
    }

    // if(btnTextTool->isChecked()) {
    //     if(ok) {
    //         textTool->addText(str, scenePoint);
    //     }
    // }

    if(txtToolFunc) {
        txtToolFunc(str, scenePoint);
    } else {
        qDebug() << "null function pointer";
    }

    lastScenePoint = scenePoint;
}

void EventsManager::onViewMouseMoved(QMouseEvent *event)
{
    // qDebug() << "EM: " << __FUNCTION__;
    // QPointF scenePoint = ui->graphicsView->mapToScene(event->pos());
    QPointF scenePoint;
    drawingManager->currentMousePos = event->pos();

    if(mapToSceneFunc) {
        scenePoint = mapToSceneFunc(event);
    } else {
        qDebug() << "null function pointer";
    }

    // Check for resizing when in MoveTool mode and not actively drawing/moving
    if(drawingManager->drawMode() == DrawingManager::MoveTool) {
        if(!drawingManager->getSelectedItems().empty()) {
            QList<QGraphicsItem*> selectedItems = drawingManager->getSelectedItems();
            QGraphicsItem *firstSelected = selectedItems.first();
            if(!firstSelected) {
                return;
            }

            if(isNearEdgeFunc) {
                currentEdge = isNearEdgeFunc(scenePoint,firstSelected);
            } else {
                qDebug() << "null function pointer";
            }


            // currentEdge = isNearEdge(scenePoint, firstSelected);  // Set isResizing here
            isResizing = (currentEdge != None);
            if(setCursorFunc) {
                setCursorFunc(isResizing);
            }
            // if(isResizing) {
            //     ui->graphicsView->setCursor(Qt::SizeAllCursor);
            // } else {
            //     ui->graphicsView->unsetCursor();
            // }
        }
    }

    if(event->buttons() & Qt::LeftButton) {
        if(drawingManager->drawMode() == DrawingManager::DrawBrush) {
            drawingManager->drawBrush(scenePoint);
        }

        else if(drawingManager->drawMode() == DrawingManager::DrawRect) {
            if(tempRect) {
                tempRect->setRect(QRectF(rectStartPoint, scenePoint).normalized());
            }
        }

        else if(drawingManager->drawMode() == DrawingManager::DrawCircle) {
            if(tempCircle) {
                tempCircle->setRect(QRectF(circleStartPoint, scenePoint).normalized());
            }
        }

        else if(drawingManager->drawMode() == DrawingManager::Eraser) {
            drawingManager->erasePart(scenePoint);
        }

        else if(drawingManager->drawMode() == DrawingManager::MoveTool) {
            if(isResizing) {

                if(setCursorFunc) {
                    setCursorFunc(true);
                } else {
                    qDebug() << "null function pointer";
                }
                drawingManager->resizeItem(scenePoint,startPoint,currentEdge);
                startPoint = scenePoint;
            } else {
                setCursorFunc(false);
                drawingManager->moveSelectedItem(scenePoint, isResizing);
            }
            // if(setCursorFunc) {
            //     setCursorFunc(isResizing);
            // } else {
            //     qDebug() << "null function pointer";
            // }
            // if(isResizing) {
            //     ui->graphicsView->setCursor(Qt::SizeAllCursor);
            //     drawingManager->resizeItem(scenePoint, startPoint, currentEdge);
            //     startPoint = scenePoint;
            // }

            // else {
            //     ui->graphicsView->unsetCursor();
            //     drawingManager->moveSelectedItem(scenePoint, isResizing);
            //     drawingManager->updateRubberBandSelection(scenePoint);
            // }

        }

        else if(drawingManager->drawMode() == DrawingManager::DrawArrow) {
            qDebug() << __FUNCTION__ << "drawMoe == drawArrow";
            QPainterPath tempPath;
            tempPath.moveTo(arrowStartPoint);
            tempPath.lineTo(scenePoint);
            tempArrow->setPath(tempPath);
        }

        else if(drawingManager->drawMode() == DrawingManager::DrawStar && tempStar) {
            double radius = QLineF(startPoint, scenePoint).length();

            QPainterPath starPath = drawingManager->createStarPath(
                startPoint, radius, drawingManager->getStarHeads());

            tempStar->setPath(starPath);

        }

        else if(drawingManager->drawMode() == DrawingManager::DrawHeart && tempHeart) {

            double size = QLineF(heartStartPoint, scenePoint).length();

            QPainterPath path = drawingManager->createHeartPath(heartStartPoint, size);

            tempHeart->setPath(path);
        }

        else if(drawingManager->drawMode() == DrawingManager::DrawBubble &&tempBubble) {
            QRectF bubbleRect(QRectF(bubbleStartPoint,scenePoint).normalized());
            QPainterPath path = drawingManager->createBubblePath(bubbleRect);
            tempBubble->setPath(path);
        }

    }
}

void EventsManager::onViewMouseReleased(QMouseEvent *event)
{


    // QPointF scenePoint = ui->graphicsView->mapToScene(event->pos());
    QPointF scenePoint;

    if(mapToSceneFunc) {

        // QPointF scenePoint = ui->graphicsView->mapToScene(event->pos());
        scenePoint = mapToSceneFunc(event);

    } else {
        qDebug() << "null function pointer";
    }
    if(drawingManager->drawMode() == DrawingManager::DrawBrush) {
        //        drawingManager->drawBrush(scenePoint, false);

        drawingManager->resetCurrentPath();

    }

    else if(drawingManager->drawMode() == DrawingManager::DrawRect) {
        drawingManager->drawRect(rectStartPoint, scenePoint);
        if(tempRect) {
            scene->removeItem(tempRect);
            delete tempRect;
            tempRect = nullptr;
        }

        setMoveToolFunc();
    }

    else if(drawingManager->drawMode() == DrawingManager::DrawCircle) {
        drawingManager->drawCircle(circleStartPoint, scenePoint);
        if(tempCircle) {
            scene->removeItem(tempCircle);
            delete tempCircle;
            tempCircle = nullptr;
        }
        // drawingManager->setDrawMode(DrawingManager::NoDraw);
        // setMoveToolMode();
        setMoveToolFunc();

    }

    else if(drawingManager->drawMode() == DrawingManager::DrawLine) {
        drawingManager->drawLine(lineStartPoint, scenePoint);
    }

    else if(drawingManager->drawMode() == DrawingManager::MoveTool) {
        // drawingManager->endRubberBandSelection();

        // ui->graphicsView->unsetCursor();
        if(setCursorFunc) {
            setCursorFunc(false);
        }
    }

    else if(drawingManager->drawMode() == DrawingManager::DrawArrow) {
        if(tempArrow) {
            scene->removeItem(tempArrow);
            delete tempArrow;
            tempArrow = nullptr;
        }

        drawingManager->drawArrow(arrowStartPoint, scenePoint);
        setMoveToolFunc();
    }

    else if(drawingManager->drawMode() == DrawingManager::DrawStar && tempStar) {
        double radius = QLineF(startPoint, scenePoint).length();
        drawingManager->drawStar(startPoint, radius, drawingManager->getStarHeads());

        scene->removeItem(tempStar);
        delete tempStar;
        tempStar = nullptr;

        setMoveToolFunc();
    }

    else if(drawingManager->drawMode() == DrawingManager::DrawHeart) {
        if(tempHeart) {
            drawingManager->drawHeart(tempHeart->path());
            scene->removeItem(tempHeart);
            delete tempHeart;
            tempHeart = nullptr;
            setMoveToolFunc();
        }
    }

    else if(drawingManager->drawMode() == DrawingManager::DrawBubble) {
        if(tempBubble) {
            drawingManager->drawSpeechBubble(tempBubble->path());
            scene->removeItem(tempBubble);
            delete tempBubble;
            tempBubble = nullptr;
            setMoveToolFunc();
        }
    }

    // if(btnTextTool->isChecked()) {
    //     ui->graphicsView->unsetCursor();
    //     setMoveToolMode();
    // }

    if(checkBtnTextTool) {
        if(checkBtnTextTool()) {
            setCursorFunc(false);
            setMoveToolFunc();
        }
    } else {
        qDebug() << "null function pointer";
    }

    isResizing = false;

    lastScenePoint = scenePoint;
}


void EventsManager::onDoubleClickEvent(QMouseEvent* /*event*/)
{

    QPointF scenePoint = lastScenePoint;


    QGraphicsItem *clickedItem = scene->itemAt(scenePoint, QTransform());

    if(clickedItem) {

        emit itemDoubleClicked(clickedItem);
    }

    if(drawingManager->drawMode() == DrawingManager::DrawPolygon) {
        drawingManager->finishPolygon();
    }

}

























