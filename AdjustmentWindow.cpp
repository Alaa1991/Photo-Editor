

#include "AdjustmentWindow.h"


AdjustmentWindow::AdjustmentWindow(QWidget *parent) :QDialog(parent)
{
    updateTimer = new QTimer(this);
    updateTimer->setInterval(200);
    updateTimer->setSingleShot(true);

    setupUi();
    setupConnections();


}


void AdjustmentWindow::setupUi()
{
    hueSlider = new QSlider(Qt::Horizontal);
    hueSlider->setRange(-100, 100);
    hueSlider->setValue(0);

    saturationSlider = new QSlider(Qt::Horizontal);
    saturationSlider->setRange(0, 200);
    saturationSlider->setValue(100);

    QLabel *huelb = new QLabel("Hue:");
    QLabel *saturationlb = new QLabel("Saturation:");


    QHBoxLayout *hueLayout = new QHBoxLayout();
    hueLayout->addWidget(huelb);
    hueLayout->addWidget(hueSlider);


    QHBoxLayout *saturationLayout = new QHBoxLayout();
    saturationLayout->addWidget(saturationlb);
    saturationLayout->addWidget(saturationSlider);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addLayout(hueLayout);
    mainLayout->addLayout(saturationLayout);
}


void AdjustmentWindow::setupConnections()
{
    connect(hueSlider,&QSlider::valueChanged,this,[this](int value) {
        updateTimer->start();

    });

    connect(saturationSlider,&QSlider::valueChanged,this,[this](int value) {
        updateTimer->start();

    });

    connect(updateTimer,&QTimer::timeout,this,&AdjustmentWindow::applyAdjustments);
}

void AdjustmentWindow::applyAdjustments()
{
    emit hueChanged(hueSlider->value() / 100.0);
    emit saturationChanged(saturationSlider->value() / 100.0);
}

void AdjustmentWindow::reset()
{
    hueSlider->setValue(0);
    saturationSlider->setValue(100);
}










