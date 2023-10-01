#include "customgraphicsview.h"

CustomGraphicsView::CustomGraphicsView(QWidget *parent) : QGraphicsView(parent)
{
}

void CustomGraphicsView::mousePressEvent(QMouseEvent *event)
{
    emit viewMousePressed(event);
    QGraphicsView::mousePressEvent(event);
}

void CustomGraphicsView::mouseMoveEvent(QMouseEvent *event)
{
    emit viewMouseMoved(event);
    QGraphicsView::mouseMoveEvent(event);
}

void CustomGraphicsView::mouseReleaseEvent(QMouseEvent *event)
{
    emit viewMouseReleased(event);
    QGraphicsView::mouseReleaseEvent(event);
}
