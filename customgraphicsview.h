#ifndef CUSTOMGRAPHICSVIEW_H
#define CUSTOMGRAPHICSVIEW_H

#include <QGraphicsView>
#include <QMouseEvent>

class CustomGraphicsView : public QGraphicsView
{
    Q_OBJECT
public:
    explicit CustomGraphicsView(QWidget *parent = 0);

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

signals:
    void viewMousePressed(QMouseEvent *event);
    void viewMouseMoved(QMouseEvent *event);
    void viewMouseReleased(QMouseEvent *event);
};

#endif // CUSTOMGRAPHICSVIEW_H
