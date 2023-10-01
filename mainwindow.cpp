#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QGraphicsPixmapItem>
#include <QImage>
#include <QDebug>
#include <QVBoxLayout>
#include <QGraphicsColorizeEffect>
#include <algorithm>
#include <QGraphicsLineItem>
#include "customgraphicsview.h"
#include <QDockWidget>
#include <QLabel>
#include <QButtonGroup>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //Buttons Setup
    setupButtons();
    setupLayouts();
    setupToolbar();

    scene = new QGraphicsScene(this);
    scene->setBackgroundBrush(Qt::transparent);

    QSize viewSize = ui->graphicsView->viewport()->size();
    initialSceneRect = QRectF(0, 0, viewSize.width(), viewSize.height());
    scene->setSceneRect(initialSceneRect);


    ui->graphicsView->setScene(scene);

    drawingManager = new DrawingManager(scene);

    // Enable anti-aliasing for smoother rendering
    ui->graphicsView->setRenderHint(QPainter::Antialiasing);
    ui->graphicsView->setRenderHint(QPainter::HighQualityAntialiasing);
    ui->graphicsView->setViewportUpdateMode(QGraphicsView::FullViewportUpdate);

    //Mouse
    this->setMouseTracking(true);
    ui->graphicsView->setMouseTracking(true);

    //Buttons
    QButtonGroup *buttonGroup = new QButtonGroup(this);
    buttonGroup->addButton(ui->btnCircle);
    buttonGroup->addButton(ui->btnRect);
    buttonGroup->addButton(btnBrush);
    buttonGroup->addButton(btnEraser);
    buttonGroup->addButton(btnLine);
    buttonGroup->setExclusive(true);


    connect(ui->graphicsView, SIGNAL(viewMousePressed(QMouseEvent*)), this, SLOT(onViewMousePressed(QMouseEvent*)));
    connect(ui->graphicsView, SIGNAL(viewMouseMoved(QMouseEvent*)), this, SLOT(onViewMouseMoved(QMouseEvent*)));
    connect(ui->graphicsView, SIGNAL(viewMouseReleased(QMouseEvent*)), this, SLOT(onViewMouseReleased(QMouseEvent*)));

    connect(ui->actionOpen, SIGNAL(triggered()), this, SLOT(openImage()));
    connect(ui->actionExit, SIGNAL(triggered()), this,SLOT(close()));

    connect(ui->btnGray, SIGNAL(clicked()), this, SLOT(convertToGrayscale()) );
    connect(ui->btnCircle, SIGNAL(clicked()), this, SLOT(setDrawCircleMode()));
    connect(ui->btnRect, SIGNAL(clicked()), this, SLOT(setDrawRectMode()));
    connect(ui->btnClear, SIGNAL(clicked()), this, SLOT(clearScene()));
    connect(btnBrush, SIGNAL(clicked()), this, SLOT(setDrawBrushMode()));
    connect(btnLine, SIGNAL(clicked()), this, SLOT(setDrawLineMode()));
    connect(btnEraser, SIGNAL(clicked()), this, SLOT(setEraserMode()));

    createWidgets();

}

MainWindow::~MainWindow()
{
    delete ui;
    delete drawingManager;
}

void MainWindow::openImage()
{


    QString fileName = QFileDialog::getOpenFileName(this, tr("Open Image"), "", tr("Images (*.png *.xpm *.jpg *.bmp)"));

    if(!fileName.isEmpty()) {
        QImage image(fileName);
        scene->clear();
        QGraphicsPixmapItem* background = new QGraphicsPixmapItem(QPixmap::fromImage(image));
        background->setZValue(-1);  // Ensure the background is always at the bottom
        scene->addItem(background);
        ui->graphicsView->fitInView(scene->sceneRect(),Qt::KeepAspectRatio);
    }
}

void MainWindow::convertToGrayscale()
{
    // Temporarily store the original background brush
       QBrush originalBackground = scene->backgroundBrush();
       // Set the background brush to transparent
       scene->setBackgroundBrush(Qt::transparent);

    for (QGraphicsItem* item : scene->items()) {
            QGraphicsPixmapItem* pixmapItem = dynamic_cast<QGraphicsPixmapItem*>(item);
            if (pixmapItem) {
                QImage image = pixmapItem->pixmap().toImage();

                for (int y = 0; y < image.height(); ++y) {
                    for (int x = 0; x < image.width(); ++x) {
                        QColor color(image.pixel(x, y));
                        int gray = qGray(color.rgb());
                        image.setPixel(x, y, qRgb(gray, gray, gray));
                    }
                }
                pixmapItem->setPixmap(QPixmap::fromImage(image));
            } else  {
                QGraphicsColorizeEffect* effect = new QGraphicsColorizeEffect();
                effect->setColor(Qt::gray);
                item->setGraphicsEffect(effect);
            }
        }
    // Restore the original background brush
    scene->setBackgroundBrush(originalBackground);
    // Update the graphics view
    ui->graphicsView->update();
}

void MainWindow::setDrawCircleMode()
{
    activateDrawButtons();
    drawingManager->setDrawMode(DrawingManager::DrawCircle);

}

void MainWindow::setDrawRectMode()
{
    activateDrawButtons();
   drawingManager->setDrawMode(DrawingManager::DrawRect);
}

void MainWindow::setDrawBrushMode()
{
    activateDrawButtons();
    drawingManager->setDrawMode(DrawingManager::DrawBrush);
//    szSlider->setEnabled(true);
}

void MainWindow::setDrawLineMode()
{
    activateDrawButtons();
    drawingManager->setDrawMode(DrawingManager::DrawLine);
//    szSlider->setEnabled(true);
}

void MainWindow::setEraserMode()
{
    activateDrawButtons();
    drawingManager->setDrawMode(DrawingManager::Eraser);
//    szSlider->setEnabled(true);
}

void MainWindow::mousePressEvent(QMouseEvent* event) {
    // Do nothing (or call the base class implementation if needed)
    QMainWindow::mousePressEvent(event);
}
void MainWindow::mouseMoveEvent(QMouseEvent* event) {
    // Do nothing (or call the base class implementation if needed)
    QMainWindow::mouseMoveEvent(event);
}
void MainWindow::mouseReleaseEvent(QMouseEvent* event) {
    // Do nothing (or call the base class implementation if needed)
    QMainWindow::mouseReleaseEvent(event);
}


void MainWindow::clearScene()
{
    scene->clear();
    drawingManager->setDrawMode(DrawingManager::NoDraw);
    ui->btnCircle->setCheckable(false);
    ui->btnRect->setCheckable(false);
    btnBrush->setCheckable(false);
    btnLine->setCheckable(false);
    btnEraser->setCheckable(false);
//    szSlider->setEnabled(false);
}

void MainWindow::activateDrawButtons()
{

    ui->btnCircle->setCheckable(true);
    ui->btnRect->setCheckable(true);
    btnBrush->setCheckable(true);
    btnLine->setCheckable(true);
    btnEraser->setCheckable(true);
}



void MainWindow::setupButtons()
{
    ui->btnGray->setFixedSize(50, 50);
    ui->btnRect->setFixedSize(50, 50);
    ui->btnCircle->setFixedSize(50, 50);
    ui->btnClear->setFixedSize (50, 50);

    btnBrush = new QPushButton("");
    btnBrush->setFixedSize(50, 50);
    btnBrush->setFlat(true);
    btnBrush->setToolTip("Brush");
    QIcon brushIcon(":/icons/illustration.png");
    btnBrush->setIcon(brushIcon);
    btnBrush->setIconSize(QSize(40, 40));

    btnLine = new QPushButton("");
    btnLine->setFlat(true);
    btnLine->setFixedSize(50, 50);
    btnLine->setToolTip("Line");
    QIcon lineIcon(":/icons/line.png");
    btnLine->setIcon(lineIcon);
    btnLine->setIconSize(QSize(40, 40));

    btnEraser = new QPushButton("");
    btnEraser->setFixedSize(50, 50);
    btnEraser->setFlat(true);
    btnEraser->setToolTip("Eraser");
    QIcon eraserIcon(":/icons/eraser.png");
    btnEraser->setIcon(eraserIcon);
    btnEraser->setIconSize(QSize(40, 40));

    //TODO choose better function cause this is not a button
//    szSlider = new QSlider(Qt::Horizontal);
//    szSlider->setMinimum(1);
//    szSlider->setMaximum(50);
//    szSlider->setValue(10);
//    szSlider->setEnabled(false);

    activateDrawButtons();
}

void MainWindow::setupLayouts()
{
    // Adjustments Layout (Horizontal)
        QHBoxLayout *adjustmentsLayout = new QHBoxLayout;
        adjustmentsLayout->addWidget(ui->btnGray);
        // Add other adjustment buttons here

        // Actions Layout (Horizontal)
        QHBoxLayout *actionsLayout = new QHBoxLayout;
        actionsLayout->addWidget(ui->btnClear);

        // Add other action buttons like save, exit, etc. here

        // Main Tools Layout: Combine the adjustments and actions layouts (Horizontal)
        QHBoxLayout *toolsLayout = new QHBoxLayout;
        toolsLayout->addLayout(adjustmentsLayout);
        toolsLayout->addSpacing(20);  // Space between adjustments and actions
        toolsLayout->addLayout(actionsLayout);

        // Main Layout (Vertical)
        QVBoxLayout *mainLayout = new QVBoxLayout;
        mainLayout->addLayout(toolsLayout);
        mainLayout->addWidget(ui->graphicsView);  // graphicsView

        QWidget* centralWidget = new QWidget;
        centralWidget->setLayout(mainLayout);
        setCentralWidget(centralWidget);
}


void MainWindow::setupToolbar()
{
    drawingsToolbar = new QToolBar("Drawing Tools", this);
    drawingsToolbar->addWidget(ui->btnRect);
    drawingsToolbar->addWidget(ui->btnCircle);
    drawingsToolbar->addWidget(btnBrush);
    drawingsToolbar->addWidget(btnLine);
    drawingsToolbar->addWidget(btnEraser);
    drawingsToolbar->setOrientation(Qt::Vertical);
    addToolBar(Qt::LeftToolBarArea, drawingsToolbar);

    adjustmentsToolbar = new QToolBar("Adjustments Tools", this);
    adjustmentsToolbar->addWidget(ui->btnGray);
    adjustmentsToolbar->setOrientation(Qt::Horizontal);
    addToolBar(Qt::TopToolBarArea, adjustmentsToolbar);
}

void MainWindow::onViewMousePressed(QMouseEvent *event)
{
    QPointF scenePoint = ui->graphicsView->mapToScene(event->pos());
    startPoint = scenePoint;

    if(drawingManager->drawMode() == DrawingManager::DrawBrush) {
        drawingManager->drawBrush(scenePoint, true);
    }

    else if(drawingManager->drawMode() == DrawingManager::DrawRect) {
       rectStartPoint = scenePoint;
       tempRect = new QGraphicsRectItem(QRectF(rectStartPoint,rectStartPoint));
       tempRect->setBrush(Qt::blue);
       scene->addItem(tempRect);
    }

    else if(drawingManager->drawMode() == DrawingManager::DrawCircle) {
        circleStartPoint = scenePoint;
        tempCircle = new QGraphicsEllipseItem(QRectF(circleStartPoint,circleStartPoint));
        tempCircle->setBrush(Qt::red);
        scene->addItem(tempCircle);
    }

    else if(drawingManager->drawMode() == DrawingManager::DrawLine) {
        lineStartPoint = scenePoint;
    }

    else if(drawingManager->drawMode() == DrawingManager::Eraser) {
        drawingManager->erasePart(scenePoint);
    }
}

void MainWindow::onViewMouseMoved(QMouseEvent *event)
{
    QPointF scenePoint = ui->graphicsView->mapToScene(event->pos());
    if(event->buttons() & Qt::LeftButton) {
        if(drawingManager->drawMode() == DrawingManager::DrawBrush) {
            drawingManager->drawBrush(scenePoint, true);
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
    }
}

void MainWindow::onViewMouseReleased(QMouseEvent *event)
{
    QPointF scenePoint = ui->graphicsView->mapToScene(event->pos());
    if(drawingManager->drawMode() == DrawingManager::DrawBrush) {
        drawingManager->drawBrush(scenePoint, false);
    }

    else if(drawingManager->drawMode() == DrawingManager::DrawRect) {
        drawingManager->drawRect(rectStartPoint, scenePoint);
        if(tempRect) {
            scene->removeItem(tempRect);
            delete tempRect;
            tempRect = nullptr;
        }
    }

    else if(drawingManager->drawMode() == DrawingManager::DrawCircle) {
        drawingManager->drawCircle(circleStartPoint, scenePoint);
        if(tempCircle) {
            scene->removeItem(tempCircle);
            delete tempCircle;
            tempCircle = nullptr;
        }
    }

    else if(drawingManager->drawMode() == DrawingManager::DrawLine) {
        drawingManager->drawLine(lineStartPoint, scenePoint);
    }

}


//void MainWindow::setupConnections()
//{

//}

void MainWindow::onSizeChanged(int newSz)
{
    drawingManager->setToolSize(newSz);
}


void MainWindow::createWidgets()
{
    QDockWidget *sliderDock = new QDockWidget("Size Slider", this);
    sliderDock->setAllowedAreas(Qt::AllDockWidgetAreas);

    QSlider *szSlider = new QSlider(Qt::Horizontal);
    szSlider->setMinimum(1);
    szSlider->setMaximum(200);
    szSlider->setValue(10);

    QLabel *szLabel = new QLabel;
    szLabel->setText(QString("%1").arg(drawingManager->getToolSize()));

    QWidget *sliderWindow = new QWidget;

    QHBoxLayout *layout = new QHBoxLayout;
    layout->addWidget(szSlider);
    layout->addWidget(szLabel);
    sliderWindow->setLayout(layout);

    sliderDock->setWidget(sliderWindow);
    addDockWidget(Qt::RightDockWidgetArea, sliderDock);

    connect(szSlider, SIGNAL(valueChanged(int)), drawingManager,SLOT(setToolSize(int)));
    connect(szSlider, &QSlider::valueChanged,[=](int value) {
        szLabel->setText(QString("%1").arg(value));
        drawingManager->setToolSize(value);
    });
}





