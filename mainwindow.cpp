#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QGraphicsPixmapItem>
#include <QImage>
#include <QDebug>
#include <QVBoxLayout>
#include <QGraphicsColorizeEffect>
#include <QGraphicsLineItem>
// #include "customgraphicsview.h"
#include <QDockWidget>
#include <QLabel>
#include <QColorDialog>
#include <QButtonGroup>
#include <QSettings>
#include <QRegularExpression>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);


    //Buttons Setup
    setupButtons();
    setupLayouts();
    setupToolbar();
    setupButtonGroups();

    scene = new QGraphicsScene(this);
    scene->setBackgroundBrush(Qt::transparent);

    QSize viewSize = ui->graphicsView->viewport()->size();
    initialSceneRect = QRectF(0, 0, viewSize.width(), viewSize.height());
    scene->setSceneRect(initialSceneRect);


    ui->graphicsView->setScene(scene);

    drawingManager = new DrawingManager(scene);

    // Enable anti-aliasing for smoother rendering
    ui->graphicsView->setRenderHint(QPainter::Antialiasing);
    ui->graphicsView->setRenderHint(QPainter::Antialiasing);
    ui->graphicsView->setViewportUpdateMode(QGraphicsView::FullViewportUpdate);

    // ui->graphicsView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    // ui->graphicsView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);

    //Mouse
    this->setMouseTracking(true);
    ui->graphicsView->setMouseTracking(true);



    setupConnections();

    loadSettings();

    createWidgets();
    resize(1000, 800);

}

MainWindow::~MainWindow()
{
    saveSettings();

    delete ui;
    delete drawingManager;
}

void MainWindow::openImage()
{


    QString fileName = QFileDialog::getOpenFileName(this, tr("Open Image"), "", tr("Images (*.png *.xpm *.jpg *.bmp)"));

    if(!fileName.isEmpty()) {
        QImage image(fileName);
        drawingManager->deleteSelectedItems();
        scene->clear();
        QGraphicsPixmapItem* background = new QGraphicsPixmapItem(QPixmap::fromImage(image));
        // background->setZValue(-1);  // Ensure the background is always at the bottom
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
    drawingManager->deselectAllItems();
    ui->btnCircle->setChecked(true);

}

void MainWindow::setDrawRectMode()
{
    activateDrawButtons();
    drawingManager->setDrawMode(DrawingManager::DrawRect);
    drawingManager->deselectAllItems();
    ui->btnRect->setChecked(true);

}

void MainWindow::setDrawBrushMode()
{
    activateDrawButtons();
    drawingManager->setDrawMode(DrawingManager::DrawBrush);
    szSlider->setMaximum(100);
    drawingManager->deselectAllItems();
    btnBrush->setChecked(true);
}

void MainWindow::setDrawLineMode()
{
    activateDrawButtons();
    drawingManager->setDrawMode(DrawingManager::DrawLine);
    szSlider->setMaximum(100);
    drawingManager->deselectAllItems();
    btnLine->setChecked(true);
}

void MainWindow::setEraserMode()
{
    activateDrawButtons();
    drawingManager->setDrawMode(DrawingManager::Eraser);
    szSlider->setMaximum(200);
    drawingManager->deselectAllItems();
    btnEraser->setChecked(true);
}

void MainWindow::setMoveToolMode()
{
    activateDrawButtons();
    drawingManager->setDrawMode(DrawingManager::MoveTool);
    btnMove->setChecked(true);
}

void MainWindow::setHandToolMode()
{
     qDebug() << "setHandToolMode called";
    activateDrawButtons();
    drawingManager->setDrawMode(DrawingManager::HandTool);
    handToolEnabled();
    btnHandTool->setChecked(true);
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
    qDebug() << "clearSCene";

    drawingManager->deleteSelectedItems();
    scene->clear();

    drawingManager->setDrawMode(DrawingManager::NoDraw);

    ui->btnCircle->setCheckable(false);
    ui->btnRect->setCheckable(false);
    btnBrush->setCheckable(false);
    btnLine->setCheckable(false);
    btnEraser->setCheckable(false);
    btnMove->setCheckable(false);\

   // szSlider->setEnabled(false);
}

void MainWindow::activateDrawButtons()
{
    qDebug() << "activateDrawButtons";
    ui->graphicsView->setDragMode(QGraphicsView::NoDrag);
    ui->graphicsView->setCursor(Qt::ArrowCursor);

    ui->btnCircle->setCheckable(true);
    ui->btnRect->setCheckable(true);
    btnBrush->setCheckable(true);
    btnLine->setCheckable(true);
    btnEraser->setCheckable(true);
    btnMove->setCheckable(true);
    btnHandTool->setCheckable(true);
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
    btnBrush->setIconSize(QSize(30, 30));

    btnLine = new QPushButton("");
    btnLine->setFlat(true);
    btnLine->setFixedSize(50, 50);
    btnLine->setToolTip("Line");
    QIcon lineIcon(":/icons/line.png");
    btnLine->setIcon(lineIcon);
    btnLine->setIconSize(QSize(30, 30));

    btnEraser = new QPushButton("");
    btnEraser->setFixedSize(50, 50);
    btnEraser->setFlat(true);
    btnEraser->setToolTip("Eraser");
    QIcon eraserIcon(":/icons/eraser.png");
    btnEraser->setIcon(eraserIcon);
    btnEraser->setIconSize(QSize(30, 30));

    btnMove = new QPushButton("");
    btnMove->setFixedSize(50, 50);
    btnMove->setFlat(true);
    btnMove->setToolTip("Move");
    QIcon moveIcon(":/icons/move.png");
    btnMove->setIcon(moveIcon);
    btnMove->setIconSize(QSize(30, 30));

    btnLoad = new QPushButton("");
    btnLoad->setFixedSize(50, 50);
    btnLoad->setFlat(true);
    btnLoad->setToolTip("Load an image");
    QIcon loadIcon(":/icons/load.png");
    btnLoad->setIcon(loadIcon);
    btnLoad->setIconSize(QSize(30, 30));

    btnZoomIn = new QPushButton("");
    btnZoomIn->setFixedSize(50, 50);
    btnZoomIn->setFlat(true);
    btnZoomIn->setToolTip("Zoom In");
    QIcon zoomInIcon(":/icons/zoom-in.png");
    btnZoomIn->setIcon(zoomInIcon);
    btnZoomIn->setIconSize(QSize(30, 30));

    btnZoomOut = new QPushButton("");
    btnZoomOut->setFixedSize(50, 50);
    btnZoomOut->setFlat(true);
    btnZoomOut->setToolTip("Zoom Out");
    QIcon zoomOutIcon(":/icons/zoom-out.png");
    btnZoomOut->setIcon(zoomOutIcon);
    btnZoomOut->setIconSize(QSize(30, 30));


    btnHandTool = new QPushButton("");
    btnHandTool->setFixedSize(50, 50);
    btnHandTool->setFlat(true);
    btnHandTool->setToolTip("Hand Tool");
    QIcon handToolIcon(":/icons/swipe-left.png");
    btnHandTool->setIcon(handToolIcon);
    btnHandTool->setIconSize(QSize(30, 30));


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
        actionsLayout->addWidget(btnLoad);

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

    drawingsToolbar->addWidget(btnHandTool);
    drawingsToolbar->addWidget(btnMove);
    drawingsToolbar->addWidget(ui->btnRect);
    drawingsToolbar->addWidget(ui->btnCircle);
    drawingsToolbar->addWidget(btnBrush);
    drawingsToolbar->addWidget(btnLine);
    drawingsToolbar->addWidget(btnEraser);
    drawingsToolbar->addWidget(btnZoomIn);
    drawingsToolbar->addWidget(btnZoomOut);
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
        drawingManager->drawBrush(scenePoint);
    }

    else if(drawingManager->drawMode() == DrawingManager::DrawRect) {
       rectStartPoint = scenePoint;
       tempRect = new QGraphicsRectItem(QRectF(rectStartPoint,rectStartPoint));
       tempRect->setBrush(drawingManager->brushColor());
       scene->addItem(tempRect);
    }

    else if(drawingManager->drawMode() == DrawingManager::DrawCircle) {
        circleStartPoint = scenePoint;
        tempCircle = new QGraphicsEllipseItem(QRectF(circleStartPoint,circleStartPoint));
        tempCircle->setBrush(drawingManager->brushColor());
        scene->addItem(tempCircle);
    }

    else if(drawingManager->drawMode() == DrawingManager::DrawLine) {
        lineStartPoint = scenePoint;
    }

    else if(drawingManager->drawMode() == DrawingManager::Eraser) {
        drawingManager->erasePart(scenePoint);
    }

    else if(drawingManager->drawMode() == DrawingManager::MoveTool) {
        drawingManager->selectItem(scenePoint);
        drawingManager->startRubberBandSelection(scenePoint);

    }


}

void MainWindow::onViewMouseMoved(QMouseEvent *event)
{
    QPointF scenePoint = ui->graphicsView->mapToScene(event->pos());

    // Check for resizing when in MoveTool mode and not actively drawing/moving
    if(drawingManager->drawMode() == DrawingManager::MoveTool) {
        if(!drawingManager->getSelectedItems().empty()) {
            QGraphicsItem *firstSelected = drawingManager->getSelectedItems().first();
            if(!firstSelected) {
                return;
            }
             currentEdge = isNearEdge(scenePoint, firstSelected);  // Set isResizing here
             isResizing = (currentEdge != None);
            if(isResizing) {
                ui->graphicsView->setCursor(Qt::SizeAllCursor);
            } else {
                ui->graphicsView->unsetCursor();
            }
        }
    }

    if(event->buttons() & Qt::LeftButton) {
        if(drawingManager->drawMode() == DrawingManager::DrawBrush) {
            drawingManager->drawBrush(scenePoint);
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

        else if(drawingManager->drawMode() == DrawingManager::MoveTool) {

            if(isResizing) {
                 ui->graphicsView->setCursor(Qt::SizeAllCursor);
                drawingManager->resizeItem(scenePoint, startPoint, currentEdge);
                startPoint = scenePoint;
            }

             else {
                ui->graphicsView->unsetCursor();
                drawingManager->moveSelectedItem(scenePoint, isResizing);
                drawingManager->updateRubberBandSelection(scenePoint);
            }

        }

    }
}

void MainWindow::onViewMouseReleased(QMouseEvent *event)
{
    QPointF scenePoint = ui->graphicsView->mapToScene(event->pos());
    if(drawingManager->drawMode() == DrawingManager::DrawBrush) {
//        drawingManager->drawBrush(scenePoint, false);
        drawingManager->resetCurrentPath();

    }

    else if(drawingManager->drawMode() == DrawingManager::DrawRect) {
        drawingManager->drawRect(rectStartPoint, scenePoint);
        if(tempRect) {
            scene->removeItem(tempRect);
            delete tempRect;
            tempRect = nullptr;
        }

         setMoveToolMode();
    }

    else if(drawingManager->drawMode() == DrawingManager::DrawCircle) {
        drawingManager->drawCircle(circleStartPoint, scenePoint);
        if(tempCircle) {
            scene->removeItem(tempCircle);
            delete tempCircle;
            tempCircle = nullptr;
        }
        // drawingManager->setDrawMode(DrawingManager::NoDraw);
        setMoveToolMode();

    }

    else if(drawingManager->drawMode() == DrawingManager::DrawLine) {
        drawingManager->drawLine(lineStartPoint, scenePoint);
    }

    else if(drawingManager->drawMode() == DrawingManager::MoveTool) {
        drawingManager->endRubberBandSelection();

        ui->graphicsView->unsetCursor();
    }

    isResizing = false;
    // ui->graphicsView->unsetCursor();
}

Edge MainWindow::isNearEdge(const QPointF &cursorPos, QGraphicsItem *item)
{   
    const qreal proximityThershold = 10.0;
    QRectF itemRect = item->boundingRect();
    QPainterPath itemPath = item->shape();
    itemRect = itemPath.boundingRect();

    itemRect = item->mapRectToScene(itemRect);

    // Check if cursor is near any of the edges
    bool nearLeft = qAbs(cursorPos.x() - itemRect.left()) < proximityThershold;
    bool nearRight = qAbs(cursorPos.x() - itemRect.right()) < proximityThershold;
    bool nearTop = qAbs(cursorPos.y() - itemRect.top()) < proximityThershold;
    bool nearBottom = qAbs(cursorPos.y() - itemRect.bottom()) < proximityThershold;

    // return nearLeft || nearRight || nearTop || nearBottom;

    if (nearLeft) {
        return Left;
    } else if (nearRight) {
        return Right;
    } else if (nearTop) {
        return Top;
    } else if (nearBottom) {
        return Bottom;
    } else {
        return None;
    }

}

void MainWindow::setupConnections()
{
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
    connect(btnMove, &QPushButton::clicked, this, &MainWindow::setMoveToolMode);
    connect(btnLoad, &QPushButton::clicked, this, &MainWindow::openImage);
    connect(btnZoomIn, &QPushButton::clicked, this, &MainWindow::zoomIn);
    connect(btnZoomOut, &QPushButton::clicked, this, &MainWindow::zoomOut);
    connect(btnHandTool, &QPushButton::clicked, this, &MainWindow::setHandToolMode);
}

void MainWindow::onSizeChanged(int newSz)
{
    drawingManager->setToolSize(newSz);
}


void MainWindow::createWidgets()
{
    QDockWidget *utilityWindow = new QDockWidget("Utilities", this);
    utilityWindow->setAllowedAreas(Qt::AllDockWidgetAreas);

    szSlider = new QSlider(Qt::Horizontal);
    szSlider->setMinimum(1);
    szSlider->setMaximum(100);
    szSlider->setValue(10);

    QLabel *szLabel = new QLabel;
    szLabel->setText(QString("%1").arg(drawingManager->getToolSize()));

    colorPickerBtn = new QPushButton("Pick Color");
    connect(colorPickerBtn, &QPushButton::clicked,this,&MainWindow::pickColor);

    opacitySlider = new QSlider(Qt::Horizontal);
    opacitySlider->setRange(0, 100);
    opacitySlider->setValue(100);
    opacitySlider->setEnabled(false);

    QLabel *opacityLabel = new QLabel;
    opacityLabel->setText(QString("%1").arg(drawingManager->getOpacityVal()));

    connect(opacitySlider, &QSlider::valueChanged,[=](int value) {
        opacityLabel->setText(QString("%1").arg(value));
        if(!isProgrammaticChange) {  //here we solve the decrementing problem
            drawingManager->changeOpacityVal(value);
        }
    });


    QWidget *sliderWindow = new QWidget;

    QHBoxLayout *sliderLayout = new QHBoxLayout;
    sliderLayout->addWidget(szSlider);
    sliderLayout->addWidget(szLabel);

    QHBoxLayout *opacityLayout = new QHBoxLayout;
    opacityLayout->addWidget(opacitySlider);
    opacityLayout->addWidget(opacityLabel);


    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addSpacing(20);
    mainLayout->addWidget(colorPickerBtn);
    mainLayout->addSpacing(20);
    mainLayout->addLayout(sliderLayout);
    mainLayout->addSpacing(20);
    mainLayout->addLayout(opacityLayout);
    mainLayout->addStretch(1);



    sliderWindow->setLayout(mainLayout);

    utilityWindow->setWidget(sliderWindow);
    addDockWidget(Qt::RightDockWidgetArea, utilityWindow);

    connect(szSlider, SIGNAL(valueChanged(int)), drawingManager,SLOT(setToolSize(int)));
    connect(szSlider, &QSlider::valueChanged,[=](int value) {
        szLabel->setText(QString("%1").arg(value));
        drawingManager->setToolSize(value);
    });

    connect(drawingManager, SIGNAL(itemSelected(int)), this,SLOT(handleItemSelected(int)));
    connect(drawingManager, SIGNAL(itemDeselected()), this,SLOT(handleItemDeselected()));
}


void MainWindow::handleItemSelected(int opacity)
{

    isProgrammaticChange = true;
    opacitySlider->setValue((opacity * 100) / 255);
    isProgrammaticChange = false;
    opacitySlider->setEnabled(true);  
}

void MainWindow::handleItemDeselected()
{
    isProgrammaticChange = true;
    opacitySlider->setValue(100);
    isProgrammaticChange = false;
    opacitySlider->setEnabled(false);
}

void MainWindow::pickColor()
{
    QColor color = QColorDialog::getColor(Qt::blue, this, "Choose Color");
    if(color.isValid()) {
        drawingManager->setBrushColor(color);
    }

    if(drawingManager->isSelected()) {         
          drawingManager->changeItemsColors(color);
    }
}



void MainWindow::keyReleaseEvent(QKeyEvent *event)
{
    drawingManager->keyReleaseEvent(event);
}

void MainWindow::setupButtonGroups()
{
    buttonGroup = new QButtonGroup(this);

    buttonGroup->addButton(ui->btnCircle);
    buttonGroup->addButton(ui->btnRect);
    buttonGroup->addButton(btnBrush);
    buttonGroup->addButton(btnEraser);
    buttonGroup->addButton(btnLine);
    buttonGroup->addButton(btnMove);
    buttonGroup->addButton(btnHandTool);

    buttonGroup->setExclusive(true);
}


void MainWindow::saveSettings()
{
    QSettings settings("K&K", "KDITOR");

    QColor lastColor = drawingManager->brushColor();
    QString colorStr = QString("rgba(%1, %2, %3, %4").arg(
                            lastColor.red()).arg(lastColor.green()).arg(
                            lastColor.blue()).arg(lastColor.alpha());

    settings.setValue("lastColor", colorStr);

    int brushThickness = drawingManager->getToolSize();
    settings.setValue("brushThickness", brushThickness);

}

void MainWindow::loadSettings()
{

    QSettings settings("K&K", "KDITOR");


    // Load the saved color string, default to black if not found
    QString colorStr = settings.value("lastColor", "rgba(0, 0, 0, 255)").toString();

    // Split the string to extract individual color components
    QStringList colorParts = colorStr.replace("rgba(", "").replace(")", "").split(",");

    if (colorParts.size() == 4) {
        // Convert string components to integers
        int red = colorParts[0].toInt();
        int green = colorParts[1].toInt();
        int blue = colorParts[2].toInt();
        int alpha = colorParts[3].toInt();

        // Create a QColor object
        QColor lastColor(red, green, blue, alpha);

        // Set the brush color
        drawingManager->setBrushColor(lastColor);
    }

    // Load the saved line thickness, default to 1 if not found
    int lineThickness = settings.value("brushThickness", 1).toInt();

    // Set the line thickness
    drawingManager->setToolSize(lineThickness);

}


void MainWindow::zoomIn()
{
    // Scale the view by the zoom factor to zoom in
    ui->graphicsView->scale(zoomFactor, zoomFactor);
}

void MainWindow::zoomOut()
{
    // Scale the view by the inverse of the zoom factor to zoom out
    ui->graphicsView->scale(1.0 / zoomFactor, 1.0 / zoomFactor);
}


void MainWindow::resetZoom()
{
    // Reset the transformation matrix to identity, which removes all zoom
    ui->graphicsView->setTransform(QTransform());
}


void MainWindow::keyPressEvent(QKeyEvent *event)
{
    drawingManager->keyPressEvent(event);

    if(event->modifiers() & Qt::ControlModifier) {

        if(event->key() == Qt::Key_Plus || event->key() == Qt::Key_Equal) {
            zoomIn();
        }

        if(event->key() == Qt::Key_Minus) {
            zoomOut();
        }

        if(event->key() == Qt::Key_0) {
            resetZoom();
        }
    }

    if(event->key() == Qt::Key_BracketRight) {
        szSlider->setValue(szSlider->value()+1);
    }

    else if(event->key() == Qt::Key_BracketLeft) {
        szSlider->setValue(szSlider->value()-1);
    }

    if(event->key() == Qt::Key_Delete) {
        drawingManager->deleteSelectedItems();
    }

    if(event->key() == Qt::Key_M) {

        setMoveToolMode();
    }

    if(event->key() == Qt::Key_C && event->modifiers() == Qt::NoModifier) {


        setDrawCircleMode();
    }

    if(event->key() == Qt::Key_R) {

        setDrawRectMode();
    }
}


void MainWindow::handToolEnabled()
{
    qDebug() << "handtoolEnabled: ";
    if(ui->graphicsView->dragMode() == QGraphicsView::NoDrag) {
        ui->graphicsView->setInteractive(true);
        ui->graphicsView->setDragMode(QGraphicsView::ScrollHandDrag);
        qDebug() << "Drag mode after setting: " << ui->graphicsView->dragMode();
        ui->graphicsView->setCursor(Qt::OpenHandCursor);

    }

    qDebug() << "Is Interactive: " << ui->graphicsView->isInteractive();
}






