#ifndef ADJUSTMENTMANAGER_H
#define ADJUSTMENTMANAGER_H

#include <QObject>
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>
#include <QImage>
#include <QColor>
#include "DrawingManager.h"
#include <QtConcurrent>
#include <QtConcurrent/QtConcurrentRun>
#include <QFuture>
#include <QFutureWatcher>

class AdjustmentManager : public QObject
{
    Q_OBJECT


public:

    explicit AdjustmentManager(QGraphicsScene *scene);
    explicit AdjustmentManager(QGraphicsScene *scene,DrawingManager *dm);
    // void hueSaturation(QGraphicsPixmapItem*,double,double);

    void adjustHue(QGraphicsPixmapItem *item,double hue);
    void adjustSaturation(QGraphicsPixmapItem *item, double saturation);
    void loadImage(QGraphicsPixmapItem *item);

    QImage adjustHueImage(const QImage &image,double hue);
    QImage adjustSaturationImage(const QImage&image,double saturation);
signals:
    void adjustmentApplied(QGraphicsItem *item);

private:
    QGraphicsScene *scene;
    DrawingManager *dm;


    QImage originalImage;
    QImage currentImage;

};

#endif // ADJUSTMENTMANAGER_H
