

#include "PropertiesWindow.h"

PropertiesWindow::PropertiesWindow(QWidget *parent):
    QDialog(parent)
{
    mainLayout = new QVBoxLayout(this);

    setupPropertiesLayout();
    setupSizeLayout();
    setupConnections();

    setLayout(mainLayout);

    setWindowTitle(tr("Properties"));
}

void PropertiesWindow::setupSizeLayout()
{

    widthSpinBox = new QDoubleSpinBox(this);
    heightSpinBox = new QDoubleSpinBox(this);

    widthSpinBox->setRange(0.0, 10000);
    heightSpinBox->setRange(0.0,10000);
    // widthSpinBox->setSingleStep(3.0);
    // heightSpinBox->setSingleStep(3.0);


    // btnApply = new QPushButton(tr("Apply"), this);

    xPosSpinBx = new QDoubleSpinBox(this);
    yPosSpinBx = new QDoubleSpinBox(this);
    xPosSpinBx->setRange(-100000.0, 100000.0);
    yPosSpinBx->setRange(-100000.0, 100000.0);

    QFormLayout *sizeLayout = new QFormLayout;
    sizeLayout->addRow(tr("Width"), widthSpinBox);
    sizeLayout->addRow(tr("Height"), heightSpinBox);
    // sizeLayout->addWidget(btnApply);


    QFormLayout *posLayout = new QFormLayout;
    posLayout->addRow(tr("X Position"), xPosSpinBx);
    posLayout->addRow(tr("Y Position"), yPosSpinBx);

    QVBoxLayout *posSzLayout = new QVBoxLayout;
    posSzLayout->addLayout(sizeLayout);
    posSzLayout->addLayout(posLayout);


    QGroupBox *sizeGroupBox = new QGroupBox(tr("Size"), this);
    sizeGroupBox->setLayout(posSzLayout);

    // QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(sizeGroupBox);
    // setLayout(mainLayout);
}


void PropertiesWindow::setupPropertiesLayout()
{
     qDebug() << __FUNCTION__;
    fillClrLabel = new QLabel(this);
    borderClrLabel = new QLabel(this);
    positionLabel = new QLabel(this);
    creationTimeLb = new QLabel(this);
    lastModfiedLb = new QLabel(this);
    lockStateLb = new QLabel(this);
    shapeTypeLb = new QLabel(this);

    propLayout = new QFormLayout;
    propLayout->addRow(tr("Fill Color"), fillClrLabel);
    propLayout->addRow(tr("Border Color"), borderClrLabel);
    propLayout->addRow(tr("Position"), positionLabel);
    propLayout->addRow(tr("Creation Time"), creationTimeLb);
    propLayout->addRow(tr("Last Modified"), lastModfiedLb);
    propLayout->addRow(tr("Lock State"), lockStateLb);
    propLayout->addRow(tr("Shape Type"), shapeTypeLb);


    QGroupBox *propGroupBox = new QGroupBox(tr("Properties"), this);
    propGroupBox->setLayout(propLayout);
    // layout()->addWidget(propGroupBox);
    mainLayout->addWidget(propGroupBox);

}


void PropertiesWindow::setupConnections()
{

    connect(widthSpinBox,&QDoubleSpinBox::valueChanged,[this]() {
        qreal currentWidth = widthSpinBox->value();
        qreal currentHeight = heightSpinBox->value();

        emit sizeChanged(currentWidth, currentHeight);
    });

    connect(heightSpinBox,&QDoubleSpinBox::valueChanged,[this]() {
        qreal currentWidth = widthSpinBox->value();
        qreal currentHeight = heightSpinBox->value();

        emit sizeChanged(currentWidth,currentHeight);
    });

    connect(xPosSpinBx,&QDoubleSpinBox::valueChanged,[this]() {
        qreal currentX = xPosSpinBx->value();
        qreal currentY = yPosSpinBx->value();

        emit posChanged(currentX,currentY);
    });

    connect(yPosSpinBx,&QDoubleSpinBox::valueChanged,[this]() {
        qreal currentX = xPosSpinBx->value();
        qreal currentY = yPosSpinBx->value();

        emit posChanged(currentX,currentY);
    });
}

void PropertiesWindow::setFillClr(const QColor &clr)
{
    qDebug() << __FUNCTION__;
    fillClrLabel->setText(clr.name());
}

void PropertiesWindow::setBorderClor(const QColor &clr)
{
    borderClrLabel->setText(clr.name());
}

void PropertiesWindow::setPos(const QPointF &pos)
{
    qreal xPos = pos.x();
    qreal yPos = pos.y();

    positionLabel->setText(QString("(%1, %2)").arg(pos.x())
                               .arg(pos.y()));

    xPosSpinBx->setValue(xPos);
    yPosSpinBx->setValue(yPos);
}


void PropertiesWindow::setCreationTime(const QString &str)
{
    creationTimeLb->setText(str);
}

void PropertiesWindow::setLastMod(const QString &str)
{
    lastModfiedLb->setText(str);
}

void PropertiesWindow::setShapeType(ShapeType sh)
{
    switch(sh) {

    case ShapeType::Rectangle:
        shapeTypeLb->setText(getShapeTypeStr(sh));
        break;

    case ShapeType::Arrow:
        shapeTypeLb->setText(getShapeTypeStr(sh));
        break;

    case ShapeType::Brush:
        shapeTypeLb->setText(getShapeTypeStr(sh));
        break;

    case ShapeType::BubbleSpeech:
        shapeTypeLb->setText(getShapeTypeStr(sh));
        break;

    case ShapeType::Circle:
        shapeTypeLb->setText(getShapeTypeStr(sh));
        break;

    case ShapeType::Freehand:
        shapeTypeLb->setText(getShapeTypeStr(sh));
        break;

     case ShapeType::Heart:
        shapeTypeLb->setText(getShapeTypeStr(sh));
        break;

    case ShapeType::Line:
        shapeTypeLb->setText(getShapeTypeStr(sh));
        break;

    case ShapeType::Polygon:
        shapeTypeLb->setText(getShapeTypeStr(sh));
        break;

    case ShapeType::Star:
            shapeTypeLb->setText(getShapeTypeStr(sh));


    }
}

void PropertiesWindow::setLockState(bool isLocked)
{
    if(isLocked) {
        lockStateLb->setText(tr("Item is locked"));
    } else {
        lockStateLb->setText(tr("Item is not locked"));
    }
}

void PropertiesWindow::setWidth(double width)
{
    widthSpinBox->setValue(width);
}

void PropertiesWindow::setHeight(double h)
{
    heightSpinBox->setValue(h);
}














