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
#include "AddShapeCommand.h"
#include "RemoveShapeCommand.h"
#include <QAction>
#include "customgraphicsview.h"
#include "KPathItem.h"


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    scene = new KGraphicsScene(this);
    scene->setBackgroundBrush(Qt::transparent);
    ui->graphicsView->setScene(scene);

    undoStack = new QUndoStack(this);
    drawingManager = new DrawingManager(scene);
    eventsManager = new EventsManager(scene, drawingManager);
    textTool = new TextTool(scene);
    propWin = new PropertiesWindow(this);
    infoWin = new SceneInfoWindo(scene, drawingManager, this);

    //Buttons Setup
    setupButtons();
    setupLayouts();
    setupToolbar();

    setupButtonGroups();

    setupActions();

    qDebug() << "after setupActions";
    setupMenues();
    qDebug() << "after setuMenue";


    QSize viewSize = ui->graphicsView->viewport()->size();
    initialSceneRect = QRectF(0, 0, viewSize.width(), viewSize.height());
    scene->setSceneRect(initialSceneRect);



    // Enable anti-aliasing for smoother rendering
    ui->graphicsView->setRenderHint(QPainter::Antialiasing);
    ui->graphicsView->setRenderHint(QPainter::Antialiasing);
    ui->graphicsView->setViewportUpdateMode(QGraphicsView::FullViewportUpdate);

    // ui->graphicsView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    // ui->graphicsView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);

    //Mouse
    this->setMouseTracking(true);
    ui->graphicsView->setMouseTracking(true);

    ui->graphicsView->setBackgroundBrush(Qt::white);


    setupConnections();
    setupFunctionCallback();
    loadSettings();

    createUtilityWindow();

    infoWin->show();
    infoWinDock();
    createTextWindow();
    // alignInfoWindow();
    resize(1000, 800);

    setWindowTitle(tr("KDitor"));
}

MainWindow::~MainWindow()
{
    saveSettings();

    delete ui;
    delete drawingManager;
    delete eventsManager;
    delete textTool;
    delete propWin;
    delete infoWin;
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
    qDebug() << __FUNCTION__;
    activateDrawButtons();
    drawingManager->setDrawMode(DrawingManager::DrawCircle);
    drawingManager->deselectAllItems();
    ui->btnCircle->setChecked(true);

}

void MainWindow::setDrawRectMode()
{
    qDebug() << __FUNCTION__;
    activateDrawButtons();
    drawingManager->setDrawMode(DrawingManager::DrawRect);
    drawingManager->deselectAllItems();
    ui->btnRect->setChecked(true);

}

void MainWindow::setDrawBrushMode()
{
    qDebug() << __FUNCTION__;
    activateDrawButtons();
    drawingManager->setDrawMode(DrawingManager::DrawBrush);
    szSlider->setMaximum(100);
    drawingManager->deselectAllItems();
    btnBrush->setChecked(true);
    szSlider->setEnabled(true);
    btnBrushColor->setEnabled(true);
    opacitySlider->setEnabled(false);
}

void MainWindow::setDrawLineMode()
{
    activateDrawButtons();
    drawingManager->setDrawMode(DrawingManager::DrawLine);
    szSlider->setMaximum(100);
    drawingManager->deselectAllItems();
    btnLine->setChecked(true);
    szSlider->setEnabled(true);
}

void MainWindow::setEraserMode()
{
    activateDrawButtons();
    drawingManager->setDrawMode(DrawingManager::Eraser);
    szSlider->setMaximum(200);
    szSlider->setEnabled(true);
    drawingManager->deselectAllItems();
    btnEraser->setChecked(true);
}

void MainWindow::setMoveToolMode()
{
    qDebug() << __FUNCTION__;
    activateDrawButtons();
    drawingManager->setDrawMode(DrawingManager::MoveTool);
    btnMove->setChecked(true);
    btnFinishPolygon->setEnabled(false);
}

void MainWindow::setHandToolMode()
{

    activateDrawButtons();
    drawingManager->setDrawMode(DrawingManager::HandTool);
    handToolEnabled();
    btnHandTool->setChecked(true);
}

void MainWindow::setTextToolMode()
{
    activateDrawButtons();
    drawingManager->deselectAllItems();
    btnTextTool->setChecked(true);

    str = QInputDialog::getText(this, tr("Enter Text"),tr("Text: "),
                                QLineEdit::Normal,QString(), &ok);

    if(ok) {
        eventsManager->setStr(str);
    }

    ui->graphicsView->setCursor(Qt::IBeamCursor);
}

void MainWindow::setDrawPolygonMode()
{
    activateDrawButtons();
    drawingManager->deselectAllItems();
    drawingManager->setDrawMode(DrawingManager::DrawPolygon);
    btnPolygon->setChecked(true);
    isDrawingPolygon = true;
    btnFinishPolygon->setEnabled(true);
}

void MainWindow::setDrawArrowMode()
{

    activateDrawButtons();
    drawingManager->deselectAllItems();
    drawingManager->setDrawMode(DrawingManager::DrawArrow);
    btnArrow->setEnabled(true);
    szSlider->setEnabled(true);
    btnArrow->setChecked(true);
}

void MainWindow::setDrawStarMode()
{
     qDebug() << __FUNCTION__;
    activateDrawButtons();
    drawingManager->deselectAllItems();
    drawingManager->setDrawMode(DrawingManager::DrawStar);
    btnStar->setEnabled(true);
    szSlider->setEnabled(true);
    starHeadsDial->setEnabled(true);
    btnStar->setChecked(true);
}

void MainWindow::setDrawHeartMode()
{
    activateDrawButtons();
    drawingManager->deselectAllItems();
    drawingManager->setDrawMode(DrawingManager::DrawHeart);
    btnHeart->setEnabled(true);
    btnHeart->setChecked(true);
}

void MainWindow::setDrawBubbleMode()
{
    activateDrawButtons();
    drawingManager->deselectAllItems();
    drawingManager->setDrawMode(DrawingManager::DrawBubble);
    btnBubble->setEnabled(true);
    btnBubble->setChecked(true);
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
    unlock();
//    ungroupItems();
    drawingManager->clearGroups();
    drawingManager->deleteSelectedItems();
    infoWin->clearList();
    scene->clear();

    drawingManager->setDrawMode(DrawingManager::NoDraw);

    // infoWin->updateInfo();
    ui->btnCircle->setCheckable(false);
    ui->btnRect->setCheckable(false);
    btnBrush->setCheckable(false);
    btnLine->setCheckable(false);
    btnEraser->setCheckable(false);
    btnMove->setCheckable(false);
    btnTextTool->setCheckable(false);
}

void MainWindow::activateDrawButtons()
{

    ui->graphicsView->setDragMode(QGraphicsView::NoDrag);
    ui->graphicsView->setCursor(Qt::ArrowCursor);

    ui->btnCircle->setCheckable(true);
    ui->btnRect->setCheckable(true);
    btnBrush->setCheckable(true);
    btnLine->setCheckable(true);
    btnEraser->setCheckable(true);
    btnMove->setCheckable(true);
    btnHandTool->setCheckable(true);
    btnTextTool->setCheckable(true);
    btnPolygon->setCheckable(true);
    btnArrow->setCheckable(true);
    btnStar->setCheckable(true);
    btnHeart->setCheckable(true);
    btnBubble->setCheckable(true);
}


void MainWindow::setupButtons()
{
    ui->btnGray->setFixedSize(50, 50);
    ui->btnGray->setIcon(QIcon(":/icons/icons/greyscale.png"));
    ui->btnGray->setIconSize(QSize(20, 20));
    ui->btnRect->setFixedSize(50, 50);
    ui->btnRect->setIconSize(QSize(20, 20));
    ui->btnRect->setIcon(QIcon(":/icons/icons/rounded-rectangle.png"));
    ui->btnCircle->setFixedSize(50, 50);
    ui->btnCircle->setIconSize(QSize(20,20));
    ui->btnCircle->setIcon(QIcon(":/icons/icons/circle.png"));
    ui->btnClear->setFixedSize (50, 50);
    ui->btnClear->setIcon(QIcon(":/icons/icons/cleaning.png"));


    btnBrush = new QPushButton("");
    btnBrush->setFixedSize(50, 50);
    btnBrush->setFlat(true);
    btnBrush->setToolTip("Brush");
    QIcon brushIcon(":/icons/icons/illustration.png");
    btnBrush->setIcon(brushIcon);
    btnBrush->setIconSize(QSize(20, 20));

    btnLine = new QPushButton("");
    btnLine->setFlat(true);
    btnLine->setFixedSize(50, 50);
    btnLine->setToolTip("Line");
    QIcon lineIcon(":/icons/icons/line.png");
    btnLine->setIcon(lineIcon);
    btnLine->setIconSize(QSize(20, 20));

    btnEraser = new QPushButton("");
    btnEraser->setFixedSize(50, 50);
    btnEraser->setFlat(true);
    btnEraser->setToolTip("Eraser");
    QIcon eraserIcon(":/icons/icons/eraser.png");
    btnEraser->setIcon(eraserIcon);
    btnEraser->setIconSize(QSize(20, 20));

    btnMove = new QPushButton("");
    btnMove->setFixedSize(50, 50);
    btnMove->setFlat(true);
    btnMove->setToolTip("Move");
    QIcon moveIcon(":/icons/icons/move.png");
    btnMove->setIcon(moveIcon);
    btnMove->setIconSize(QSize(20, 20));

    btnLoad = new QPushButton("");
    btnLoad->setFixedSize(50, 50);
    btnLoad->setFlat(true);
    btnLoad->setToolTip("Load an image");
    QIcon loadIcon(":/icons/icons/load.png");
    btnLoad->setIcon(loadIcon);
    btnLoad->setIconSize(QSize(20, 20));

    btnZoomIn = new QPushButton("");
    btnZoomIn->setFixedSize(50, 50);
    btnZoomIn->setFlat(true);
    btnZoomIn->setToolTip("Zoom In");
    QIcon zoomInIcon(":/icons/icons/zoom-in.png");
    btnZoomIn->setIcon(zoomInIcon);
    btnZoomIn->setIconSize(QSize(20, 20));
    // :/icons/icons/zoom-in.png
    btnZoomOut = new QPushButton("");
    btnZoomOut->setFixedSize(50, 50);
    btnZoomOut->setFlat(true);
    btnZoomOut->setToolTip("Zoom Out");
    QIcon zoomOutIcon(":/icons/icons/zoom-out.png");
    btnZoomOut->setIcon(zoomOutIcon);
    btnZoomOut->setIconSize(QSize(20, 20));


    btnHandTool = new QPushButton("");
    btnHandTool->setFixedSize(50, 50);
    btnHandTool->setFlat(true);
    btnHandTool->setToolTip("Hand Tool");
    QIcon handToolIcon(":/icons/icons/swipe-left.png");
    btnHandTool->setIcon(handToolIcon);
    btnHandTool->setIconSize(QSize(20, 20));

    btnTextTool = new QPushButton("");
    btnTextTool->setFixedSize(50, 50);
    btnTextTool->setFlat(true);
    btnTextTool->setToolTip("Text Tool");
    QIcon textToolIcon(":/icons/icons/text.png");
    btnTextTool->setIcon(textToolIcon);
    btnTextTool->setIconSize(QSize(20, 20));


    btnPolygon = new QPushButton("");
    btnPolygon->setFixedSize(50, 50);
    btnPolygon->setFlat(true);
    btnPolygon->setToolTip("Text Tool");
    QIcon polygonIcon(":/icons/icons/trapezium.png");
    btnPolygon->setIcon(polygonIcon);
    btnPolygon->setIconSize(QSize(20, 20));


    btnUndo = new QPushButton("");
    btnUndo->setEnabled(undoStack->canUndo());
    btnUndo->setFixedSize(50, 50);
    btnUndo->setFlat(true);
    btnUndo->setToolTip("Undo");
    QIcon undoIcon(":/icons/icons/undo.png");
    btnUndo->setIcon(undoIcon);
    btnUndo->setIconSize(QSize(20, 20));



    btnRedo = new QPushButton("");
    btnRedo->setEnabled(undoStack->canRedo());
    btnRedo->setFixedSize(50, 50);
    btnRedo->setFlat(true);
    btnRedo->setToolTip("Redo");
    QIcon redoIcon(":/icons/icons/circular-arrow.png");
    btnRedo->setIcon(redoIcon);
    btnRedo->setIconSize(QSize(20, 20));

    btnArrow = new QPushButton("");
    btnArrow->setFixedSize(50, 50);
    btnArrow->setFlat(true);
    btnArrow->setToolTip("Arrow Tool");
    QIcon arrowIcon(":/icons/icons/arrow-right.png");
    btnArrow->setIcon(arrowIcon);
    btnArrow->setIconSize(QSize(20, 20));

    btnStar = new QPushButton("");
    btnStar->setFixedSize(50, 50);
    btnStar->setFlat(true);
    btnStar->setToolTip("Arrow Tool");
    QIcon starIcon(":/icons/icons/star.png");
    btnStar->setIcon(starIcon);
    btnStar->setIconSize(QSize(20, 20));

    btnBrushColor = new QPushButton("");
    btnBrushColor->setFixedSize(40, 40);
    btnBrushColor->setFlat(true);
    btnBrushColor->setToolTip("Arrow Tool");
    QIcon brushColorIcon(":/icons/icons/color-palette.png");
    btnBrushColor->setIcon(brushColorIcon);
    btnBrushColor->setIconSize(QSize(30, 30));
    btnBrushColor->setEnabled(false);

    //CreateWidgets buttons
    btnLocked = new QPushButton;
    btnLocked->setFlat(true);
    btnLocked->setFixedSize(35, 35);
    btnLocked->setToolTip("Lock Shape");
    QIcon iconLocked(":/icons/icons/lock.png");
    btnLocked->setIcon(iconLocked);
    btnLocked->setIconSize(QSize(25, 25));
    btnLocked->setCheckable(true);

    btnUnlocked = new QPushButton;
    btnUnlocked->setFlat(true);
    btnUnlocked->setFixedSize(40, 40);
    btnUnlocked->setToolTip("Lock Shape");
    QIcon iconUnlocked(":/icons/icons/unlocked.png");
    btnUnlocked->setIcon(iconUnlocked);
    btnUnlocked->setIconSize(QSize(30, 30));
    btnUnlocked->setCheckable(true);

    btnSendFront = new QPushButton;
    btnSendFront->setFlat(true);
    btnSendFront->setFixedSize(40, 40);
    btnSendFront->setToolTip("Send To The Front");
    QIcon iconSendFront(":/icons/icons/sendFront.png");
    btnSendFront->setIcon(iconSendFront);
    btnSendFront->setIconSize(QSize(30, 30));


    btnSendBack = new QPushButton;
    btnSendBack->setFlat(true);
    btnSendBack->setFixedSize(35, 35);
    btnSendBack->setToolTip("Send To The Back");
    QIcon iconSendBack(":/icons/icons/sendBack.png");
    btnSendBack->setIcon(iconSendBack);
    btnSendBack->setIconSize(QSize(25, 25));


    btnSendForward = new QPushButton;
    btnSendForward->setFlat(true);
    btnSendForward->setFixedSize(40, 40);
    btnSendForward->setToolTip("Send Forward");
    QIcon iconSendForward(":/icons/icons/sendForward.png");
    btnSendForward->setIcon(iconSendForward);
    btnSendForward->setIconSize(QSize(30, 30));

    btnSendBackward = new QPushButton;
    btnSendBackward->setFlat(true);
    btnSendBackward->setFixedSize(40, 40);
    btnSendBackward->setToolTip("Send Backward");
    QIcon iconSendBackward(":/icons/icons/sendBackward.png");
    btnSendBackward->setIcon(iconSendBackward);
    btnSendBackward->setIconSize(QSize(30, 30));

    btnGroup = new QPushButton;
    btnGroup->setFlat(true);
    btnGroup->setFixedSize(40, 40);
    btnGroup->setToolTip("Group Items");
    QIcon iconGroup(":/icons/icons/group.png");
    btnGroup->setIcon(iconGroup);
    btnGroup->setIconSize(QSize(30, 30));
    btnGroup->setCheckable(true);

    btnUngroup = new QPushButton;
    btnUngroup->setFlat(true);
    btnUngroup->setFixedSize(40, 40);
    btnUngroup->setToolTip("Ungroup Items");
    QIcon iconUngroup(":/icons/icons/ungroup.png");
    btnUngroup->setIcon(iconUngroup);
    btnUngroup->setIconSize(QSize(30, 30));

    btnSaveAs = new QPushButton;
    btnSaveAs->setFlat(true);
    btnSaveAs->setFixedSize(40, 40);
    btnSaveAs->setToolTip("Save As");
    QIcon iconSaveAs(":/icons/icons/saveAs.png");
    btnSaveAs->setIcon(iconSaveAs);
    btnSaveAs->setIconSize(QSize(30, 30));

    btnHeart = new QPushButton;
    btnHeart->setFlat(true);
    btnHeart->setFixedSize(50, 50);
    btnHeart->setToolTip("Heart");
    QIcon iconHeart(":/icons/icons/heart.png");
    btnHeart->setIcon(iconHeart);
    btnHeart->setIconSize(QSize(20, 20));

    btnBubble = new QPushButton;
    btnBubble->setFlat(true);
    btnBubble->setFixedSize(50, 50);
    btnBubble->setToolTip("Bubble");
    QIcon iconBubble(":/icons/icons/bubble.png");
    btnBubble->setIcon(iconBubble);
    btnBubble->setIconSize(QSize(20, 20));

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
        actionsLayout->addWidget(btnSaveAs);
        actionsLayout->addWidget(btnUndo);
        actionsLayout->addWidget(btnRedo);
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
    drawingsToolbar->addWidget(btnPolygon);
    drawingsToolbar->addWidget(btnArrow);
    drawingsToolbar->addWidget(btnStar);
    drawingsToolbar->addWidget(btnHeart);
    drawingsToolbar->addWidget(btnBubble);
    drawingsToolbar->addWidget(btnEraser);
    drawingsToolbar->addWidget(btnZoomIn);
    drawingsToolbar->addWidget(btnZoomOut);
    drawingsToolbar->addWidget(btnTextTool);

    drawingsToolbar->setOrientation(Qt::Vertical);
    addToolBar(Qt::LeftToolBarArea, drawingsToolbar);

    adjustmentsToolbar = new QToolBar("Adjustments Tools", this);
    adjustmentsToolbar->addWidget(ui->btnGray);
    adjustmentsToolbar->setOrientation(Qt::Horizontal);
    addToolBar(Qt::TopToolBarArea, adjustmentsToolbar);
}

void MainWindow::onViewMousePressed(QMouseEvent *event)
{
    eventsManager->onViewMousePressed(event);   
}

void MainWindow::onViewMouseMoved(QMouseEvent *event)
{    
    eventsManager->onViewMouseMoved(event); 
}

void MainWindow::onViewMouseReleased(QMouseEvent *event)
{
    eventsManager->onViewMouseReleased(event);
}

void MainWindow::mouseDoubleClickEvent(QMouseEvent *event)
{


    eventsManager->onDoubleClickEvent(event);
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
    // connect(ui->graphicsView,&CustomGraphicsView::viewMousePressed,
    //         eventsManager,&EventsManager::onViewMousePressed);

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
    connect(btnTextTool, &QPushButton::clicked, this, &MainWindow::setTextToolMode);
    connect(btnPolygon, &QPushButton::clicked,this, &MainWindow::setDrawPolygonMode);
    connect(btnArrow, &QPushButton::clicked, this, &MainWindow::setDrawArrowMode);
    connect(btnStar, &QPushButton::clicked, this, &MainWindow::setDrawStarMode);
    connect(btnBrushColor,&QPushButton::clicked,this,&MainWindow::changeBrushColor);
    connect(btnLocked,&QPushButton::clicked,this,&MainWindow::lock);
    connect(btnUnlocked,&QPushButton::clicked,this,&MainWindow::unlock);
    connect(btnSendFront,&QPushButton::clicked,this,&MainWindow::sendToFront);
    connect(btnSendBack,&QPushButton::clicked,this,&MainWindow::sendToBack);
    connect(btnSendForward,&QPushButton::clicked,this,&MainWindow::sendFroward);
    connect(btnSendBackward,&QPushButton::clicked,this,&MainWindow::sendBackward);
    connect(btnHeart,&QPushButton::clicked,this,&MainWindow::setDrawHeartMode);
    connect(btnBubble,&QPushButton::clicked,this,&MainWindow::setDrawBubbleMode);

    connect(btnUndo, &QPushButton::clicked, undoAction, &QAction::trigger);

    connect(btnRedo, &QPushButton::clicked, redoAction, &QAction::trigger);

    connect(drawingManager, SIGNAL(textSelected()), this,SLOT(handleTextSelected()));

    connect(drawingManager, SIGNAL(textDeselected()), this,
            SLOT(handleTextDeselected()));



    connect(drawingManager, &DrawingManager::addShapeCommand,this,
            &MainWindow::onAddShapeCommand);



    connect(drawingManager, &DrawingManager::removeShapeCommand,this,
            &MainWindow::onRemoveShapeCommand);

    connect(drawingManager,&DrawingManager::borderSzChanged,this,
            &MainWindow::onChangeBorderSz);

    connect(drawingManager,&DrawingManager::itemUpdated,this,
            &MainWindow::onItemUpdated);

    connect(drawingManager,&DrawingManager::itemCopied,this,
            &MainWindow::updatePasteState);


    connect(undoStack, &QUndoStack::canUndoChanged, undoAction, &QAction::setEnabled);
    connect(undoStack, &QUndoStack::canRedoChanged, redoAction, &QAction::setEnabled);
    connect(undoStack, &QUndoStack::canRedoChanged, btnRedo, &QPushButton::setEnabled);
    connect(undoStack, &QUndoStack::canUndoChanged, btnUndo, &QPushButton::setEnabled);

    connect(toggleInfoWin,&QAction::triggered,this,&MainWindow::toggleInfoWindow);
    connect(toggleUtilityWin,&QAction::triggered,this,&MainWindow::toggleUtilityWindow);
    connect(toggleTextToolAc,&QAction::triggered,this,&MainWindow::toggleTextToolWindow);

    connect(propWin,&PropertiesWindow::sizeChanged,this,
            &MainWindow::onShapeSizeChanged);

    connect(propWin,&PropertiesWindow::posChanged,this,
            &MainWindow::onShapePosChanged);

    connect(eventsManager,&EventsManager::itemDoubleClicked,
            this, &MainWindow::onItemDoubleClicked);

    connect(drawingManager,&DrawingManager::addShapeCommand,infoWin,
            &SceneInfoWindo::updateInfo);

    connect(infoWin,&SceneInfoWindo::itemClicked,
                     this,&MainWindow::onListItemClicked);

    connect(infoWin,&SceneInfoWindo::itemDoubleClicked,this,
            &MainWindow::onItemDoubleClicked);

    connect(infoWin,&SceneInfoWindo::deleteItem,this,&MainWindow::deleteLstItem);

    connect(scene,&KGraphicsScene::contextmMenuRequested,this,
                                    &MainWindow::onContextMenuRequested);

    //CONTEXT MENU connections
    connect(copyAction,QAction::triggered,drawingManager,
                &DrawingManager::copyItems);
    connect(pasteAction,QAction::triggered,drawingManager,
            &DrawingManager::pasteItems);
    connect(propAction,QAction::triggered,this,&MainWindow::showPropWindow);
    connect(sendBackAction,&QAction::triggered,this,&MainWindow::sendToBack);
    connect(sendFrontAction,&QAction::triggered,this,&MainWindow::sendToFront);

    connect(sendForwardAction,&QAction::triggered,this,
            &MainWindow::sendFroward);

    connect(sendBackwardAction,&QAction::triggered,this,
            &MainWindow::sendBackward);

    connect(deleteAction,&QAction::triggered,drawingManager,
            &DrawingManager::deleteSelectedItems);

    connect(lockAction,&QAction::triggered,this,&MainWindow::lock);
    connect(unlockAction,&QAction::triggered,this,&MainWindow::unlock);

    connect(selectAllAction,&QAction::triggered,drawingManager,
            &DrawingManager::selectAllItems);

    connect(clearAction,&QAction::triggered,this,&MainWindow::clearScene);

    connect(btnGroup,&QPushButton::clicked,this,&MainWindow::groupItems);
    connect(btnUngroup,&QPushButton::clicked,this,&MainWindow::ungroupItems);

    connect(btnSaveAs,&QPushButton::clicked,this,[this]() {
        if(scene != nullptr)
            saveImage();
    });
    connect(saveImageAsAction,&QAction::triggered,this,[this]() {
        if(scene)
            saveImage();
    });
}

void MainWindow::onSizeChanged(int newSz)
{
    drawingManager->setToolSize(newSz);
}


void MainWindow::createUtilityWindow()
{
    utilityWindow = new QDockWidget("Utilities", this);
    utilityWindow->setAllowedAreas(Qt::AllDockWidgetAreas);

    brushSzLabel = new QLabel(tr("Brush Size"));
    szSlider = new QSlider(Qt::Horizontal);
    szSlider->setMinimum(1);
    szSlider->setMaximum(100);
    szSlider->setValue(10);
    szSlider->setEnabled(false);

    QLabel *szLabel = new QLabel;
    szLabel->setText(QString("%1").arg(drawingManager->getToolSize()));

    //Change Shapes FILL icon and lbs
    btnChangeItemFill = new QPushButton;
    btnChangeItemFill->setFlat(true);
    btnChangeItemFill->setFixedSize(39, 39);
    btnChangeItemFill->setToolTip("Change Shapes Fill");
    QIcon iconChangeItemFill(":/icons/icons/fillColor.png");
    btnChangeItemFill->setIcon(iconChangeItemFill);
    btnChangeItemFill->setIconSize(QSize(26, 26));

    changeItemFillLb = new QLabel("Shapes Fill");

    showChangeItmFillLb = new QLabel;
    showChangeItmFillLb->setFixedSize(100, 15);
    showChangeItmFillLb->setFrameStyle(QFrame::Sunken | QFrame::Panel);
    showChangeItmFillLb->setStyleSheet(
        QString("background-color: %1").arg(drawingManager->brushColor().name()));


    changeSysClrCheck = new QCheckBox(tr("Change System Color"), this);
    changeSysClrCheck->setChecked(false);

    connect(btnChangeItemFill, &QPushButton::clicked,this,&MainWindow::pickColor);

    //OPACITY
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

    //STAR
    starHeadsDial = new QDial;
    starHeadsDial->setRange(5, 10);
    starHeadsDial->setValue(5);
    starHeadsDial->setEnabled(false);
    starHeadsLb = new QLabel(tr("Star Heads"));
    starHeadsLbV = new QLabel("5");

    //POLYGON FINISH BTN
    btnFinishPolygon = new QPushButton;
    btnFinishPolygon->setText(tr("Finish Polygon"));
    btnFinishPolygon->setEnabled(false);

    btnChangeBorder = new QPushButton;
    // btnChangeBorder->setText(tr("Change Border Color"));
    btnChangeBorder->setFlat(true);
    btnChangeBorder->setFixedSize(40, 40);
    btnChangeBorder->setToolTip("Change Shapes Fill");
    QIcon iconChangeBorder(":/icons/icons/borderColor.png");
    btnChangeBorder->setIcon(iconChangeBorder);
    btnChangeBorder->setIconSize(QSize(30, 30));

    showChangeBorderLb = new QLabel;
    showChangeBorderLb->setFixedSize(100, 15);
    showChangeBorderLb->setFrameStyle(QFrame::Sunken | QFrame::Panel);
    showChangeBorderLb->setStyleSheet(
        QString("background-color: %1").arg(drawingManager->getBorderColor().name()));
    changeBorderLb = new QLabel(tr("Border color"));


    borderSizeSpin = new QSpinBox;
    borderSizeSpin->setRange(1, 50);
    borderSizeSpin->setEnabled(false);
    borderSzLabel = new QLabel(tr("Border Size"));

    brushColorLbel = new QLabel(tr("Brush Color"));

    showBrushColorLb = new QLabel;
    showBrushColorLb->setFixedSize(100, 15);
    showBrushColorLb->setFrameStyle(QFrame::Sunken | QFrame::Panel);
    showBrushColorLb->setStyleSheet(
        QString("background-color: %1").arg(
            drawingManager->getHandBrushColor().name()));



    // connect(borderSizeSpin,&QSpinBox::valueChanged,
    //         drawingManager, &DrawingManager::changeBorderSz);
    connect(borderSizeSpin,&QSpinBox::valueChanged,[=](int val) {
        if(!isProgrammaticChange) {
            drawingManager->changeBorderSz(val);
        }
    });

    connect(starHeadsDial,&QDial::valueChanged,this,[this](int val) {
        this->starHeadsLbV->setText(QString::number(val));
    });

    connect(starHeadsDial,&QDial::valueChanged,drawingManager,
            &DrawingManager::setStarHeads);

    connect(btnFinishPolygon, &QPushButton::clicked,this,&MainWindow::finishPolygon);

    connect(btnChangeBorder, &QPushButton::clicked,this,
            &MainWindow::changeItemsBorderColor);

    QWidget *sliderWindow = new QWidget;

    QHBoxLayout *sliderLayout = new QHBoxLayout;
    sliderLayout->addWidget(brushSzLabel);
    sliderLayout->addWidget(szSlider);
    sliderLayout->addWidget(szLabel);

    QHBoxLayout *borderSzLayout = new QHBoxLayout;
    borderSzLayout->addWidget(borderSzLabel);
    borderSzLayout->addWidget(borderSizeSpin);

    QHBoxLayout *brushColorLayout = new QHBoxLayout;
    brushColorLayout->addWidget(brushColorLbel);
    brushColorLayout->addWidget(showBrushColorLb);
    brushColorLayout->addWidget(btnBrushColor);

    QHBoxLayout *starHeadsLayout = new QHBoxLayout;
    starHeadsLayout->addWidget(starHeadsLb);
    starHeadsLayout->addWidget(starHeadsDial);
    starHeadsLayout->addWidget(starHeadsLbV);


    opacityLabelText = new QLabel(tr("Opacity"));
    QHBoxLayout *opacityLayout = new QHBoxLayout;
    opacityLayout->addWidget(opacityLabelText);
    opacityLayout->addWidget(opacitySlider);
    opacityLayout->addWidget(opacityLabel);

    QHBoxLayout *changeItemFillLayout = new QHBoxLayout;
    changeItemFillLayout->addWidget(changeItemFillLb);
    changeItemFillLayout->addWidget(showChangeItmFillLb);
    changeItemFillLayout->addWidget(btnChangeItemFill);
    // changeItemFillLayout->addStretch(0.5);
    // changeItemFillLayout->addWidget(changeSysClrCheck);

    QHBoxLayout *changeBorderLayout = new QHBoxLayout;
    changeBorderLayout->addWidget(changeBorderLb);
    changeBorderLayout->addWidget(showChangeBorderLb);
    changeBorderLayout->addWidget(btnChangeBorder);

    QHBoxLayout *lockGroupLayout = new QHBoxLayout;
    lockGroupLayout->addWidget(btnLocked);
    lockGroupLayout->addWidget(btnUnlocked);
    lockGroupLayout->addStretch(3);
    lockGroupLayout->addWidget(btnGroup);
    lockGroupLayout->addStretch(5);
    lockGroupLayout->addWidget(btnUngroup);
    lockGroupLayout->addStretch(5);

    QHBoxLayout *sendFrontLayout = new QHBoxLayout;
    sendFrontLayout->addWidget(btnSendFront);
    sendFrontLayout->addWidget(btnSendBack);
    sendFrontLayout->addWidget(btnSendForward);
    sendFrontLayout->addWidget(btnSendBackward);



    //DOCK WINDOW MAIN LAYOUT
    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addSpacing(20);
    mainLayout->addWidget(changeSysClrCheck);
    mainLayout->addLayout(changeItemFillLayout);
    // mainLayout->addSpacing(5);
    mainLayout->addLayout(brushColorLayout);
    // mainLayout->addSpacing(5);
    mainLayout->addLayout(changeBorderLayout);
    // mainLayout->addSpacing(5);
    // mainLayout->addWidget(btnChangeBorder);
    // mainLayout->addSpacing(5);
    mainLayout->addLayout(sliderLayout);
    // mainLayout->addSpacing(5);
    mainLayout->addLayout(opacityLayout);
    // mainLayout->addWidget(brushGroup);

    mainLayout->addSpacing(5);
    mainLayout->addLayout(borderSzLayout);
    mainLayout->addSpacing(5);
    mainLayout->addLayout(starHeadsLayout);
//    mainLayout->addStretch(1);
    mainLayout->addSpacing(10);
    mainLayout->addWidget(btnFinishPolygon);
//    mainLayout->addStretch(1);
    // mainLayout->addSpacing(10);
    mainLayout->addLayout(lockGroupLayout);
    // mainLayout->addSpacing(10);
    mainLayout->addLayout(sendFrontLayout);

    sliderWindow->setLayout(mainLayout);

    utilityWindow->setWidget(sliderWindow);
    addDockWidget(Qt::RightDockWidgetArea, utilityWindow);

    connect(szSlider, SIGNAL(valueChanged(int)), drawingManager,SLOT(setToolSize(int)));
    connect(szSlider, &QSlider::valueChanged,[=](int value) {
        szLabel->setText(QString("%1").arg(value));
        drawingManager->setToolSize(value);
    });

    // connect(drawingManager, SIGNAL(itemSelected(int,QGraphicsItem*)), this,SLOT(handleItemSelected(int)));
//    connect(drawingManager,&DrawingManager::itemSelected,this,&MainWindow::handleItemSelected);
    connect(drawingManager,static_cast<void(DrawingManager::*)(
                                int,QGraphicsItem*)>(&DrawingManager::itemSelected),
            this,&MainWindow::handleItemSelected);

    connect(drawingManager, SIGNAL(itemDeselected()), this,SLOT(handleItemDeselected()));
}


void MainWindow::createTextWindow()
{
    textDock = new QDockWidget(tr("Text Tools"), this);
    // QTimer::singleShot(0,this, [this](){textDock->hide(); });
    // addDockWidget(Qt::RightDockWidgetArea, textDock);
    splitDockWidget(infoWinDoc, textDock, Qt::Vertical);

    QWidget *dockWidgetContent = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(dockWidgetContent);

    fontComboBox = new QFontComboBox();
    colorComboBox = new QComboBox();
    fontSizeSpinBox = new QSpinBox();
    fontSizeSpinBox->setRange(1, 100);
    editCheckBox = new QCheckBox(tr("Enabled Text Editing"),dockWidgetContent);
    editCheckBox->setChecked(false);

    boldButton = new QToolButton;
    boldButton->setText("B");
    boldButton->setCheckable(true);

    italicButton = new QToolButton;
    italicButton->setText("I");
    italicButton->setCheckable(true);

    underlineButton = new QToolButton;
    underlineButton->setText("U");
    underlineButton->setCheckable(true);

    strickThroughButton = new QToolButton;
    strickThroughButton->setText("S");
    strickThroughButton->setCheckable(true);

     // Horizontal layout for the new formatting buttons
    QHBoxLayout *formattingLayout = new QHBoxLayout();
    formattingLayout->addWidget(boldButton);
    formattingLayout->addWidget(italicButton);
    formattingLayout->addWidget(underlineButton);
    formattingLayout->addWidget(strickThroughButton);

    layout->addWidget(fontComboBox);
    layout->addWidget(colorComboBox);
    layout->addWidget(fontSizeSpinBox);
    layout->addWidget(editCheckBox);
    layout->addLayout(formattingLayout);

    textDock->setWidget(dockWidgetContent);

    // Populate color combo box with some standard colors
    QStringList colorNames = QColor::colorNames();
    foreach(const QString &colorName, colorNames) {

        QColor color(colorName);
        QPixmap pixmap(16, 16);
        pixmap.fill(color);
        colorComboBox->addItem(QIcon(pixmap), colorName);
    }

    fontComboBox->setEnabled(false);
    colorComboBox->setEnabled(false);
    fontSizeSpinBox->setEnabled(false);
    editCheckBox->setEnabled(false);
    boldButton->setEnabled(false);
    italicButton->setEnabled(false);
    strickThroughButton->setEnabled(false);
    underlineButton->setEnabled(false);



    connect(fontComboBox,&QFontComboBox::currentFontChanged,this,
            &MainWindow::setTextFont);
    connect(colorComboBox,&QComboBox::currentTextChanged,this,
            &MainWindow::setTextColor);
    connect(fontSizeSpinBox,QOverload<int>::of(&QSpinBox::valueChanged), this,
            &MainWindow::setTextSize);
    connect(editCheckBox, &QCheckBox::toggled,this,&MainWindow::toggleTextEdittibility);

    connect(boldButton, &QToolButton::toggled,this,&MainWindow::setBoldText);
    connect(italicButton, &QToolButton::toggled,this,&MainWindow::setItalicText);
    connect(underlineButton,&QToolButton::toggled,this,&MainWindow::setUnderlineText);
    connect(strickThroughButton,&QToolButton::toggled,this,&MainWindow::setStrikeText);


}


void MainWindow::handleItemSelected(int opacity, QGraphicsItem *item)
{

    isProgrammaticChange = true;
    //Currenly selected shape opacity?
    opacitySlider->setValue((opacity * 100) / 255);

    auto kpathItem = dynamic_cast<KPathItem*>(item);


    if(kpathItem) {
        QColor pathItemClr = kpathItem->brush().color();

        showChangeItmFillLb->setStyleSheet(
            QString("background-color: %1").arg(pathItemClr.name()));


        showChangeBorderLb->setStyleSheet(QString(
                "background-color: %1").arg(kpathItem->pen().color().name()));

        btnLocked->setChecked(kpathItem->getLocked());

        btnUnlocked->setChecked(!kpathItem->getLocked());

        bool isGroupFound = (drawingManager->findGroup(item) != nullptr);
        btnGroup->setChecked(isGroupFound);

        updatePropWindow(kpathItem);
//        infoWin->selectItem(kpathItem);
        qDebug() << "the shape type is " << getShapeTypeStr(kpathItem->getShapeType());

    } else {
        qDebug() << "kpath is null";
    }


    isProgrammaticChange = false;
    opacitySlider->setEnabled(true);
    borderSizeSpin->setEnabled(true);
}

void MainWindow::handleItemDeselected()
{

    isProgrammaticChange = true;
    opacitySlider->setValue(100);
    isProgrammaticChange = false;
    opacitySlider->setEnabled(false);
    szSlider->setEnabled(false);
    borderSizeSpin->setEnabled(false);
    btnBrushColor->setEnabled(false);
    starHeadsDial->setEnabled(false);
    btnUnlocked->setChecked(false);
    btnLocked->setChecked(false);
    btnGroup->setChecked(false);

    propWin->hide();
    infoWin->clearListItemsSelc();

//    drawingManager->deselectAllItems();
}

void MainWindow::handleTextSelected()
{
    // textDock->show();

    fontComboBox->setEnabled(true);
    colorComboBox->setEnabled(true);
    fontSizeSpinBox->setEnabled(true);
    editCheckBox->setEnabled(true);

    boldButton->setEnabled(true);
    italicButton->setEnabled(true);
    strickThroughButton->setEnabled(true);
    underlineButton->setEnabled(true);

    for(auto *item : drawingManager->getSelectedItems()) {
        if(item) {
            QGraphicsTextItem *textItem = dynamic_cast<QGraphicsTextItem*>(item);
            if(textItem) {
                fontComboBox->setCurrentFont(textItem->font());
                colorComboBox->setCurrentText(
                    colorNameFromColor(textItem->defaultTextColor()));
                fontSizeSpinBox->setValue(textItem->font().pointSize());
            }
        }
    }
}


void MainWindow::handleTextDeselected()
{

    // textDock->hide();
    fontComboBox->setEnabled(false);
    colorComboBox->setEnabled(false);
    fontSizeSpinBox->setEnabled(false);
    editCheckBox->setEnabled(false);
    boldButton->setEnabled(false);
    italicButton->setEnabled(false);
    strickThroughButton->setEnabled(false);
    underlineButton->setEnabled(false);
}

void MainWindow::pickColor()
{
    QColor color = QColorDialog::getColor(Qt::blue, this, "Choose Brush Color");

    if(color.isValid() && !drawingManager->getSelectedItems().empty()) {
        drawingManager->changeItemsFillColor(color);

        if(changeSysClrCheck->isChecked()) {
            drawingManager->setSysFillColor(color);
        }
    }

    showChangeItmFillLb->setStyleSheet(
        QString("background-color: %1").arg(color.name()));
}

void MainWindow::changeBrushColor()
{
    QColor color = QColorDialog::getColor(Qt::blue,this,"Choose Brush Color");

    if(color.isValid()) {
        drawingManager->setHandBrushColor(color);

        showBrushColorLb->setStyleSheet(
            QString("background-color: %1").arg(color.name()));
    }
}

void MainWindow::changeItemsBorderColor()
{
    QColor color = QColorDialog::getColor(Qt::blue, this, "Choose Border Color");
    if(!drawingManager->getSelectedItems().empty()) {
        drawingManager->chageItemsBorderColor(color);
        // drawingManager->setBorderColor(color);
        showChangeBorderLb->setStyleSheet(
            QString("background-color: %1").arg(color.name()));
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
    buttonGroup->addButton(btnTextTool);
    buttonGroup->addButton(btnPolygon);
    buttonGroup->addButton(btnArrow);
    buttonGroup->addButton(btnStar);
    buttonGroup->addButton(btnHeart);
    buttonGroup->addButton(btnBubble);

    // buttonGroup->setExclusive(true);
    // qDebug() << "before lock buttongroup";
    // btnGroupLock = new QButtonGroup(this);
    //  qDebug() << "after lock buttongroup";
    // btnGroupLock->addButton(btnLocked);
    //   qDebug() << "after lock btn";
    // btnGroupLock->addButton(btnUnlocked);
    // btnGroupLock->setExclusive(true);
}


void MainWindow::saveSettings()
{
    QSettings settings("K&K", "KDITOR");

    QColor lastColor = drawingManager->brushColor();

    QString colorStr = QString("rgba(%1, %2, %3, %4").arg(
                            lastColor.red()).arg(lastColor.green()).arg(
                            lastColor.blue()).arg(lastColor.alpha());

    settings.setValue("lastColor", colorStr);

    qDebug() << "Saving color:" << colorStr;

    int brushThickness = drawingManager->getToolSize();
    settings.setValue("brushThickness", brushThickness);

    qDebug() << "Saving brush thickness:" << brushThickness;

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
        drawingManager->setSysFillColor(lastColor);
        qDebug() << "Loaded color:" << lastColor;
    }



    // Load the saved line thickness, default to 1 if not found
    int lineThickness = settings.value("brushThickness", 1).toInt();

    // Set the line thickness
    drawingManager->setToolSize(lineThickness);
    qDebug() << "Loaded brush thickness:" << lineThickness;
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

        if(event->key() == Qt::Key_A) {
            drawingManager->selectAllItems();
            if(drawingManager->drawMode() != DrawingManager::MoveTool) {

                setMoveToolMode();
            }
        }

        if(event->key() == Qt::Key_S)
            saveImage();
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

    if(event->key() == Qt::Key_T) {
        setTextToolMode();
    }

    if(event->key() == Qt::Key_S && event->modifiers() == Qt::NoModifier) {
        setDrawStarMode();
    }

    if(event->key() == Qt::Key_A && event->modifiers() == Qt::NoModifier) {
        setDrawArrowMode();
    }
}


void MainWindow::handToolEnabled()
{

    if(ui->graphicsView->dragMode() == QGraphicsView::NoDrag) {
        ui->graphicsView->setInteractive(true);
        ui->graphicsView->setDragMode(QGraphicsView::ScrollHandDrag);
        qDebug() << "Drag mode after setting: " << ui->graphicsView->dragMode();
        ui->graphicsView->setCursor(Qt::OpenHandCursor);

    }

    qDebug() << "Is Interactive: " << ui->graphicsView->isInteractive();
}


void MainWindow::setTextFont(const QFont &font)
{

    textTool->setTextFont(font);
    applyTextChanges();
}

void MainWindow::setTextColor(const QColor &color)
{

    textTool->setTextColor(color);
    applyTextChanges();
}

void MainWindow::setTextSize(int sz)
{

    textTool->setTextSize(sz);
    applyTextChanges();
}



QString MainWindow::colorNameFromColor(const QColor &color)
{
    for(const QString &name : QColor::colorNames()) {
        if(QColor(name) == color) {
            return name;
        }
    }

    return color.name(QColor::HexRgb);
}




void MainWindow::applyTextChanges()
{
    for(auto *item : drawingManager->getSelectedItems()) {
        QGraphicsTextItem *textItem = dynamic_cast<QGraphicsTextItem*>(item);
        if(textItem) {
            textItem->setFont(textTool->font());
            textItem->setDefaultTextColor(textTool->color());
            QFont currentFont = textItem->font();
            currentFont.setPointSize(textTool->getTextSz());
            textItem->setFont(currentFont);

        }
    }
}


void MainWindow::toggleTextEdittibility(bool enabled)
{
    textTool->setEditableStatus(enabled);
}

void MainWindow::setBoldText(bool bold)
{

    for(auto *item : drawingManager->getSelectedItems()) {
        QGraphicsTextItem *textItem = dynamic_cast<QGraphicsTextItem*>(item);
        if(textItem) {
            QFont font = textItem->font();
            font.setBold(bold);
            textItem->setFont(font);
        }
    }
}

void MainWindow::setItalicText(bool italic)
{
    for(auto *item : drawingManager->getSelectedItems()) {
        QGraphicsTextItem *textItem = dynamic_cast<QGraphicsTextItem*>(item);
        if(textItem) {
            QFont font = textItem->font();
            font.setItalic(italic);
            textItem->setFont(font);
        }
    }
}

void MainWindow::setUnderlineText(bool underline)
{
    for(auto *item : drawingManager->getSelectedItems()) {
        QGraphicsTextItem *textItem = dynamic_cast<QGraphicsTextItem*>(item);
        if(textItem) {
            QFont font = textItem->font();
            font.setUnderline(underline);
            textItem->setFont(font);
        }
    }
}


void MainWindow::setStrikeText(bool strike)
{
    for(auto *item : drawingManager->getSelectedItems()) {
        QGraphicsTextItem *textItem = dynamic_cast<QGraphicsTextItem*>(item);
        if(textItem) {
            QFont font = textItem->font();
            font.setStrikeOut(strike);
            textItem->setFont(font);
        }
    }
}

void MainWindow::finishPolygon()
{
    qDebug() << __FUNCTION__;
    drawingManager->finishPolygon();
    setMoveToolMode();
}


void MainWindow::onAddShapeCommand(QGraphicsItem *item)
{
    qDebug() << __FUNCTION__;
    AddShapeCommand *command = new AddShapeCommand(item, scene);
    undoStack->push(command);
}


void MainWindow::onRemoveShapeCommand(QGraphicsItem *item)
{
    qDebug() << __FUNCTION__;
    RemoveShapeCommand *command = new RemoveShapeCommand(item, scene);
    undoStack->push(command);
}





void MainWindow::setupMenues()
{
    QMenu *fileMenu = ui->menuFile;
    fileMenu->addAction(saveImageAsAction);
    fileMenu->addSeparator();
    fileMenu->addAction(undoAction);
    fileMenu->addAction(redoAction);


    editMenu = menuBar()->addMenu(tr("&Edit"));
    editMenu->addAction(copyAction);
    editMenu->addAction(pasteAction);
    editMenu->addAction(sendBackAction);
    editMenu->addAction(sendFrontAction);
    editMenu->addAction(sendBackwardAction);
    editMenu->addAction(sendForwardAction);
    editMenu->addAction(lockAction);
    editMenu->addAction(unlockAction);
    editMenu->addAction(deleteAction);
    pasteAction->setEnabled(!drawingManager->getCopiedItems().isEmpty());
    editMenu->addAction(selectAllAction);
    editMenu->addAction(clearAction);

    windowMenu = menuBar()->addMenu(tr("&Window"));
    windowMenu->addAction(toggleInfoWin);
    windowMenu->addAction(toggleUtilityWin);
    windowMenu->addAction(toggleTextToolAc);

    shapeContextMenu = new QMenu(this);
    sceneContextMenu = new QMenu(this);

    shapeContextMenu->addAction(copyAction);
    shapeContextMenu->addAction(pasteAction);
    shapeContextMenu->addAction(sendBackAction);
    shapeContextMenu->addAction(sendFrontAction);
    shapeContextMenu->addAction(sendForwardAction);
    shapeContextMenu->addAction(sendBackwardAction);
    shapeContextMenu->addAction(lockAction);
    shapeContextMenu->addAction(unlockAction);
    shapeContextMenu->addAction(deleteAction);
    shapeContextMenu->addAction(propAction);

    sceneContextMenu->addAction(selectAllAction);
    sceneContextMenu->addAction(clearAction);

}

void MainWindow::setupActions()
{
    QIcon copyIcon(":/icons/icons/copy.png");
    QIcon propIcon(":/icons/icons/properties.png");
    QIcon pasteIcon(":/icons/icons/paste.png");
    QIcon sendBackwardIcon(":/icons/icons/sendBackward.png");
    QIcon sendForwardIcon(":/icons/icons/sendForward.png");
    QIcon sendBackIcon(":/icons/icons/sendBack.png");
    QIcon sendFrontIcon(":/icons/icons/sendFront.png");
    QIcon deleteIcon(":/icons/icons/delete.png");
    QIcon lockIcon(":/icons/icons/lock.png");
    QIcon unlockIcon(":/icons/icons/unlocked.png");
    QIcon selectAllIcon(":/icons/icons/selectAll.png");
    QIcon clearIcon(":/icons/icons/cleaning.png");
    QIcon saveImageAsIcon(":/icons/icons/saveAs.png");
    // QIcon groupIcon("");
    // QIcon ungroupIcon("");

    undoAction = undoStack->createUndoAction(this, tr("&Undo"));
    undoAction->setShortcuts(QKeySequence::Undo);

    redoAction = undoStack->createRedoAction(this, tr("&Redo"));
    redoAction->setShortcuts(QKeySequence::Redo);

    toggleInfoWin = new QAction(tr("Scene Information"), this);
    toggleInfoWin->setCheckable(true);
    toggleInfoWin->setChecked(true);

    toggleUtilityWin = new QAction(tr("Utility"), this);
    toggleUtilityWin->setCheckable(true);
    toggleUtilityWin->setChecked(true);

    toggleTextToolAc = new QAction(tr("Text Tools"), this);
    toggleTextToolAc->setCheckable(true);
    toggleTextToolAc->setChecked(true);

    copyAction = new QAction(tr("Copy"), this);
    copyAction->setIcon(copyIcon);
    pasteAction = new QAction(tr("Paste"), this);
    pasteAction->setIcon(pasteIcon);
    sendBackAction = new QAction(tr("Send Back"), this);
    sendBackAction->setIcon(sendBackIcon);
    sendFrontAction = new QAction(tr("Send Front"), this);
    sendFrontAction->setIcon(sendFrontIcon);
    sendForwardAction = new QAction(tr("Send Forward"), this);
    sendForwardAction->setIcon(sendForwardIcon);
    sendBackwardAction = new QAction(tr("Send Backward"), this);
    sendBackwardAction->setIcon(sendBackwardIcon);
    lockAction = new QAction(tr("Lock"), this);
    lockAction->setIcon(lockIcon);
    unlockAction = new QAction(tr("Unlock"), this);
    unlockAction->setIcon(unlockIcon);
    deleteAction = new QAction(tr("Delete"), this);
    deleteAction->setIcon(deleteIcon);
    propAction = new QAction(tr("Properties"), this);
    propAction->setIcon(propIcon);
    selectAllAction = new QAction(tr("Select All"), this);
    selectAllAction->setIcon(selectAllIcon);
    clearAction = new QAction(tr("Select All"), this);
    clearAction->setIcon(clearIcon);

    saveImageAsAction = new QAction(tr("Save As"), this);
    saveImageAsAction->setIcon(saveImageAsIcon);



}

void MainWindow::onChangeBorderSz(int sz)
{   
    isProgrammaticChange = true;
    borderSizeSpin->setValue(sz);
    isProgrammaticChange = false;
}

void MainWindow::setupFunctionCallback()
{

    eventsManager->mapToSceneFunc = [this](const QMouseEvent *event) ->QPointF {
        return ui->graphicsView->mapToScene(event->pos());
    };

    eventsManager->txtToolFunc = [this](const QString& str,const QPointF& scenePoint) {
        if(btnTextTool->isChecked() && ok) {
            textTool->addText(str, scenePoint);
        }
    };

    eventsManager->isNearEdgeFunc = [this](const QPointF &point, QGraphicsItem *item) {
       return this->isNearEdge(point, item);
    };

    eventsManager->setCursorFunc = [this](bool isResizing) {
        if(isResizing) {
            ui->graphicsView->setCursor(Qt::SizeAllCursor);
        } else {
            ui->graphicsView->unsetCursor();
        }
    };

    eventsManager->setMoveToolFunc = [this]() {
        setMoveToolMode();
    };

    eventsManager->checkBtnTextTool = [this] () -> bool {
        return btnTextTool->isChecked();
    };
}

void MainWindow::lock()
{
    drawingManager->setIsLocked(true);
    btnUnlocked->setChecked(false);
}

void MainWindow::unlock()
{
    for(auto *item : drawingManager->getSelectedItems()) {
        drawingManager->setIsLocked(false);
        btnLocked->setChecked(false);
    }
    // drawingManager->setIsLocked(false);
    // btnLocked->setChecked(false);
}

// void MainWindow::unlockAllItems()
// {

// }


void MainWindow::onShapeSizeChanged(qreal width, qreal height)
{
    if(!isProgrammaticChange)
        drawingManager->changeShapeSz(width,height);
}


void MainWindow::updatePropWindow(QGraphicsItem *item)
{
    qDebug() << __FUNCTION__;

    auto kpathItem = dynamic_cast<KPathItem*>(item);

    if(kpathItem) {

        propWin->setFillClr(kpathItem->getFillFolor());

        propWin->setBorderClor(kpathItem->getBorderColor());

        propWin->setPos(kpathItem->pos());

        propWin->setCreationTime(kpathItem->getCreationTime().toString());
        propWin->setLastMod(kpathItem->getLastModifiedTime().toString());
        propWin->setLockState(kpathItem->getLocked());
        propWin->setShapeType(kpathItem->getShapeType());

        QRectF boundingRect = kpathItem->boundingRect();

        double width = boundingRect.width();
        double height = boundingRect.height();

        propWin->setWidth(width);
        propWin->setHeight(height);
    }
}

void MainWindow::onItemDoubleClicked(QGraphicsItem *item)
{
     qDebug() << __FUNCTION__;
    KPathItem *kpathItem = dynamic_cast<KPathItem*>(item);

    if(kpathItem) {
        qDebug() << "before updatePropWindow";
        updatePropWindow(kpathItem);

        propWin->show();
    }
}

void MainWindow::onShapePosChanged(qreal x, qreal y)
{
    if(!isProgrammaticChange)
         drawingManager->changeShapePos(x, y);
}

void MainWindow::onItemUpdated(QGraphicsItem *item)
{
    auto *kpathItem = dynamic_cast<KPathItem*>(item);
    if(kpathItem) {
        isProgrammaticChange = true;
        QRectF bundingRect = kpathItem->boundingRect();
        double width = bundingRect.width();
        double height = bundingRect.height();
        propWin->setWidth(width);
        propWin->setHeight(height);
        propWin->setPos(kpathItem->pos());
        propWin->setLastMod(kpathItem->getLastModifiedTime().toString());
        isProgrammaticChange = false;

    }
}

void MainWindow::sendToFront()
{
    auto selectedItems = drawingManager->getSelectedItems();
    if(!selectedItems.empty()) {
        auto item = selectedItems.first();
        auto kpathItem = dynamic_cast<KPathItem*>(item);
        if(kpathItem) {
            qreal highestZ = 0;

            for(auto &i : scene->items()) {
                if(i->zValue() > highestZ) {
                        highestZ = i->zValue();
                }
            }

            kpathItem->setZValue(highestZ + 1);
        }
    }
}


void MainWindow::sendToBack()
{
    auto selectedItems = drawingManager->getSelectedItems();
    if(!selectedItems.empty()) {
        auto item = selectedItems.first();
        auto kpathItem = dynamic_cast<KPathItem*>(item);
        if(kpathItem) {
            qreal lowestZ = 0;

            for(auto &i : scene->items()) {
                if(i->zValue() < lowestZ) {
                        lowestZ = i->zValue();
                }
            }

            kpathItem->setZValue(lowestZ - 1);
        }
    }
}


void MainWindow::sendFroward()
{
    auto selectedItems = drawingManager->getSelectedItems();
    if(!selectedItems.empty()) {
        auto item = selectedItems.first();
        auto kpathItem = dynamic_cast<KPathItem*>(item);

        if(kpathItem) {
            kpathItem->setZValue(kpathItem->zValue() + 1);
        }
    }
}

void MainWindow::sendBackward()
{
    auto selectedItems = drawingManager->getSelectedItems();
    if(!selectedItems.empty()) {
        auto item = selectedItems.first();
        auto kpathItem = dynamic_cast<KPathItem*>(item);

        if(kpathItem) {
            kpathItem->setZValue(kpathItem->zValue() - 1);
        }
    }
}


void MainWindow::alignInfoWindow()
{
    if(infoWin && utilityWindow) {

        QPoint position = this->pos() + utilityWindow->pos();

        position.setX(position.x() + utilityWindow->width());
        infoWin->move(position);
    }
}

void MainWindow::onListItemClicked(QGraphicsItem *item)
{
    if(!item)
        return;

    KPathItem *kpathItem = dynamic_cast<KPathItem*>(item);

    if(kpathItem) {
        drawingManager->deselectAllItems();

        drawingManager->selectItem(kpathItem);
    }
}

void MainWindow::deleteLstItem(QGraphicsItem *item)
{
    qDebug() << __FUNCTION__;

    drawingManager->deleteItem(item);
}

void MainWindow::infoWinDock()
{
    infoWinDoc = new QDockWidget(tr("Scene Information"), this);
    infoWinDoc->setWidget(infoWin);

    splitDockWidget(utilityWindow, infoWinDoc, Qt::Horizontal);
}



void MainWindow::toggleInfoWindow(bool checked)
{
    if(checked) {
        infoWin->show();
    } else {
        infoWin->hide();
    }
}

void MainWindow::toggleUtilityWindow(bool checked)
{
    if(checked) {
        utilityWindow->show();
    } else {
        utilityWindow->hide();
    }
}

void MainWindow::toggleTextToolWindow(bool checked)
{
    if(checked) {
        textDock->show();
    } else {
        textDock->hide();
    }
}

void MainWindow::onContextMenuRequested(QPointF /*scenePos*/, QGraphicsItem *item)
{
    qDebug() << __FUNCTION__;


    if(item) {
        auto *kpathItem = static_cast<KPathItem*>(item);
        if(!kpathItem)
            return;

        deleteAction->setEnabled(!kpathItem->getLocked());

        shapeContextMenu->exec(QCursor::pos());

    } else {
        sceneContextMenu->exec(QCursor::pos());
    }
}

void MainWindow::showPropWindow()
{
     qDebug() << __FUNCTION__;
    // updatePropWindow();
    propWin->show();
}

void MainWindow::updatePasteState()
{
    pasteAction->setEnabled(!drawingManager->getCopiedItems().isEmpty());
}

void MainWindow::groupItems()
{
    drawingManager->groupItems();
}

void MainWindow::ungroupItems()
{
    drawingManager->ungroupItems();
}


void MainWindow::saveImage()
{
    qDebug() << __FUNCTION__;

    SaveWin saveDialog(scene, this);

    saveDialog.exec();
}











