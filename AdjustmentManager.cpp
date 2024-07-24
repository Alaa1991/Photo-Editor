

#include "AdjustmentManager.h"

AdjustmentManager::AdjustmentManager(QGraphicsScene *scene) :scene(scene)
{

}

AdjustmentManager::AdjustmentManager(QGraphicsScene *scene,DrawingManager *dm):
    scene(scene),dm(dm)
{

}

void AdjustmentManager::adjustHue(QGraphicsPixmapItem *item, double hue)
{
    if(!item || originalImage.isNull()) {
        return;
    }


    QFuture<QImage> future = QtConcurrent::run([this, hue]() {
        return adjustHueImage(originalImage, hue);
    });

    auto *watcher = new QFutureWatcher<QImage>();

    connect(watcher,&QFutureWatcher<QImage>::finished,this,[this,item,watcher](){
        QImage adjusted = watcher->result();
        item->setPixmap(QPixmap::fromImage(adjusted));
        emit adjustmentApplied(static_cast<QGraphicsItem*>(item));
        watcher->deleteLater();
    });

    watcher->setFuture(future);

    // QImage adjusted = adjustHueImage(originalImage, hue);

    // item->setPixmap(QPixmap::fromImage(adjusted));

    // emit adjustmentApplied(static_cast<QGraphicsItem*>(item));
}

void AdjustmentManager::adjustSaturation(QGraphicsPixmapItem *item,double saturation)
{
    if(!item || originalImage.isNull()) {
        qDebug() << "Item is null or original image is null, skipping adjustment";
        return;
    }
     qDebug() << "Starting saturation adjustment";
    QFuture<QImage> future = QtConcurrent::run([this, saturation]() {
        // return adjustSaturationImage(originalImage, saturation);
        qDebug() << "Running adjustSaturationImage in thread";
        QImage result = adjustSaturationImage(originalImage, saturation);
        qDebug() << "Completed adjustSaturationImage in thread";
        return result;
        // return adjustSaturationImage(currentImage, saturation);
    });

    auto *watcher = new QFutureWatcher<QImage>();

    connect(watcher,&QFutureWatcher<QImage>::finished,this,[this,item,watcher](){

        //this is problamatic cause it makes destructive modification
        // QImage image = item->pixmap().toImage();
        // QImage adjusted = adjustSaturationImage(originalImage, saturation);

        qDebug() << "FutureWatcher finished, updating pixmap";
        QImage adjusted = watcher->result();
        currentImage = adjusted;
         qDebug() << "Setting adjusted image to pixmap";
        item->setPixmap(QPixmap::fromImage(adjusted));

        emit adjustmentApplied(static_cast<QGraphicsItem*>(item));
        qDebug() << "Saturation adjustment applied and signal emitted";
        watcher->deleteLater();
    });

    watcher->setFuture(future);


}

// void AdjustmentManager::adjustSaturation(QGraphicsPixmapItem *item, double saturation) {
//     if (!item || originalImage.isNull())
//         return;
//     QImage image = item->pixmap().toImage();
//     // QImage adjusted = adjustSaturationImage(originalImage, saturation);
//     image = adjustSaturationImage(image, saturation);
//     item->setPixmap(QPixmap::fromImage(image));
//     emit adjustmentApplied(static_cast<QGraphicsItem*>(item));
// }

QImage AdjustmentManager::adjustHueImage(const QImage &image, double hue)
{
    QImage adjusted(image.size(), image.format());
    QColor oldColor;

    for(int x = 0; x < image.width();++x) {
        for(int y = 0; y < image.height(); ++y) {
            oldColor = QColor(image.pixel(x, y));

            float h,s,v;
            oldColor.getHsvF(&h,&s,&v);

            h += hue;
            while(h > 1)
                h -= 1;
            while(h < 0)
                h += 1;

            oldColor.setHsvF(h,s,v);

            adjusted.setPixel(x, y, oldColor.rgb());
        }
    }

    return adjusted;
}

QImage AdjustmentManager::adjustSaturationImage(const QImage &image, double saturation)
{
    QImage adjusted(image.size(), image.format());
    QColor oldColor;

    for(int x = 0; x < image.width(); ++x) {
        for(int y = 0; y < image.height(); ++y) {
            oldColor = QColor(image.pixel(x, y));
            float h,s,v;

            oldColor.getHsvF(&h,&s,&v);
            // Log original values
            qDebug() << "Original HSV:" << h << s << v;

            s *= saturation;

            s = qBound(0.0f, s, 1.0f);

            // Log adjusted values
            qDebug() << "Adjusted HSV:" << h << s << v;

            oldColor.setHsvF(h, s, v);
            adjusted.setPixel(x, y, oldColor.rgb());
        }
    }
    qDebug() << "Saturation adjustment complete";
    return adjusted;
}


void AdjustmentManager::loadImage(QGraphicsPixmapItem *item)
{
    if(item) {
        // Store the unmodified image
        originalImage = item->pixmap().toImage();
        // currentImage = originalImage;

    }
}



























