#ifndef PROPERTIESWINDOW_H
#define PROPERTIESWINDOW_H


#include <QDialog>
#include <QDoubleSpinBox>
#include <QFormLayout>
#include <QPushButton>
#include <QLabel>
#include <QGroupBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QColor>
#include "ShapeType.h"

class PropertiesWindow : public QDialog {
    Q_OBJECT

public:
    PropertiesWindow(QWidget *parent = nullptr);

    // qreal getWidth() const;
    // qreal getHeight() const;
    // QColor getFillColor() const;
    // QColor getBorderColor() const;
    // int getBorderSize() const;
    // qreal getOpacity() const;
    // bool isLocked() const;

    void setFillClr(const QColor &clr);
    void setBorderClor(const QColor &clr);
    void setPos(const QPointF &pos);
    void setCreationTime(const QString& ctime);
    void setLastMod(const QString &lastMod);
    void setLockState(bool isLocked);
    void setShapeType(ShapeType);

    void setWidth(double width);
    void setHeight(double height);




signals:
    void sizeChanged(qreal width,qreal height);
    void posChanged(qreal x, qreal y);


private:
    QDoubleSpinBox *widthSpinBox;
    QDoubleSpinBox *heightSpinBox;
    // QPushButton *btnApply;

    QDoubleSpinBox *xPosSpinBx;
    QDoubleSpinBox *yPosSpinBx;

    QLabel *fillClrLabel;
    QLabel *borderClrLabel;
    QLabel *positionLabel;
    QLabel *creationTimeLb;
    QLabel *lastModfiedLb;
    QLabel *lockStateLb;
    QLabel *shapeTypeLb;
    QFormLayout *propLayout;

    QVBoxLayout *mainLayout = new QVBoxLayout;

    void setupSizeLayout();
    void setupPropertiesLayout();
    void setupConnections();

};

#endif // PROPERTIESWINDOW_H
