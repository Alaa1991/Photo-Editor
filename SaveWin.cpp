
#include "SaveWin.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QFileDialog>
#include <QPainter>
#include <QSettings>

SaveWin::SaveWin(QGraphicsScene *scene, QWidget *parent)
    :QDialog(parent), scene(scene)

{
    setupUI();
}

SaveWin::~SaveWin() {}

void SaveWin::setupUI()
{

    formatComboBox = new QComboBox(this);
    formatComboBox->addItem("PNG");
    formatComboBox->addItem("JPEG");
    formatComboBox->addItem("BMP");
    formatComboBox->addItem("GIF");

    widthSpinBox = new QSpinBox(this);
    widthSpinBox->setRange(1, 10000);
    widthSpinBox->setValue(800);
    heightSpinBox = new QSpinBox(this);
    heightSpinBox->setRange(1, 10000);
    heightSpinBox->setValue(600);

    qualitySpinBox = new QSpinBox(this);
    qualitySpinBox->setRange(1, 100);
    qualitySpinBox->setValue(100);
    qualitySpinBox->setEnabled(false);

     // Update quality SpinBox enabled state based on format
    connect(formatComboBox,&QComboBox::currentTextChanged,[this](const QString &format){
        qualitySpinBox->setEnabled(format.toLower().contains("jpeg"));
    });

    antialiasingCheckBox = new QCheckBox("High Quality Rendering (Antialiasing)", this);
    antialiasingCheckBox->setChecked(true);

    btnSave = new QPushButton("Save", this);
    btnCancel = new QPushButton("Cancel", this);

    progressBar = new QProgressBar(this);
    progressBar->setRange(0, 100);
    progressBar->setValue(0);

    // Layouts
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(new QLabel("Format:"));
    mainLayout->addWidget(formatComboBox);
    mainLayout->addWidget(new QLabel("Width:"));
    mainLayout->addWidget(widthSpinBox);
    mainLayout->addWidget(new QLabel("Height:"));
    mainLayout->addWidget(heightSpinBox);
    mainLayout->addWidget(new QLabel("Quality:"));
    mainLayout->addWidget(qualitySpinBox);
    mainLayout->addWidget(antialiasingCheckBox);

    QHBoxLayout* buttonLayout = new QHBoxLayout();
    buttonLayout->addWidget(btnSave);
    buttonLayout->addWidget(btnCancel);
    mainLayout->addLayout(buttonLayout);
    mainLayout->addWidget(progressBar);

    // Set dialog properties
    setLayout(mainLayout);

    setWindowTitle(tr("Save As"));

    connect(btnSave,&QPushButton::clicked,this,&SaveWin::onSaveClicked);
    connect(btnCancel,&QPushButton::clicked,this,&SaveWin::onCancelClicked);

}

void SaveWin::onSaveClicked()
{
    qDebug() << __FUNCTION__;

    QSettings settings;
    QString lastDir = settings.value("lastSaveDirectory", "").toString();

    QString selectedFormat = formatComboBox->currentText();
    QString defaultExtention = selectedFormat.toLower();

    if(defaultExtention == "jpeg") {
        defaultExtention = "jpg";
    }

    QString filter = QString("Images (*.%1)").arg(defaultExtention);

    QString filePath = QFileDialog::getSaveFileName(this,tr("Save Image"), lastDir, filter);

    if(!filePath.isEmpty()) {
        settings.setValue("lastSaveDirectory", QFileInfo(filePath).absolutePath());
        QFileInfo fileInfo(filePath);
        if(fileInfo.suffix().isEmpty()) {
            filePath += "." + defaultExtention;
        }

        renderScene();

        QByteArray formatByteArray = selectedFormat.toLatin1();
        const char *formatChar = formatByteArray.data();

        int quality = (selectedFormat.toLower() == "jpeg") ? qualitySpinBox->value(): -1;

        progressBar->setValue(50);

        if(renderedImage.save(filePath, formatChar, quality)) {
            qDebug() << "success";
            progressBar->setValue(100);
        } else {

            qDebug() << "failed";
            progressBar->setValue(0);
        }

        accept(); //close the dialog
    }
}

void SaveWin::onCancelClicked()
{
    reject(); //close the dialog
}

void SaveWin::renderScene()
{
    qDebug() << __FUNCTION__;
    if(!scene) {
        qDebug() << "null scene ";
        return;
    }

    QImage image(widthSpinBox->value(),heightSpinBox->value(),
                 QImage::Format_ARGB32_Premultiplied);

    image.fill(Qt::transparent);

    QPainter painter(&image);

    if(antialiasingCheckBox->isChecked()) {
        painter.setRenderHint(QPainter::Antialiasing);
        painter.setRenderHint(QPainter::SmoothPixmapTransform);
    }

    QRectF renderRect = scene->itemsBoundingRect();

    scene->render(&painter, QRectF(), renderRect);

    if(formatComboBox->currentText().toLower() == "jpeg" ||
        formatComboBox->currentText().toLower() == "bmp") {

        // Create a new image with a white background and re-draw the original image over it
        QImage nonTransparent(image.size(), QImage::Format_RGB32);
        nonTransparent.fill(Qt::white);
        QPainter painter(&nonTransparent);
        painter.drawImage(0, 0, image);
        renderedImage = nonTransparent;
    } else {

        renderedImage = image;
    }

}








