#ifndef ADJUSTMENTWINDOW_H
#define ADJUSTMENTWINDOW_H

#include <QDialog>
#include <QSlider>
#include <QLabel>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QWidget>
#include <QObject>
#include <QTimer>

class AdjustmentWindow : public QDialog {

    Q_OBJECT

public:
    explicit AdjustmentWindow(QWidget *parent=nullptr);

    void setupUi();
    void setupConnections();
    void reset();

    void applyAdjustments();


signals:
    void hueChanged(double hue);
    void saturationChanged(double saturation);



private:
    QSlider *hueSlider;
    QSlider *saturationSlider;

    QTimer *updateTimer;



};

#endif // ADJUSTMENTWINDOW_H
