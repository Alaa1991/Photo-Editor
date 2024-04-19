#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsScene>
#include <QPushButton>
#include <QMouseEvent>
#include "DrawingManager.h"
#include <QSlider>
#include "EdgeEnum.h"
#include "TextTool.h"
#include <QInputDialog>
#include <QLineEdit>
#include <QFontComboBox>
#include <QComboBox>
#include <QSpinBox>
#include <QCheckBox>
#include <QToolButton>
#include <QUndoCommand>
#include <QUndoStack>
#include <QAction>
#include <QLabel>
#include <QDial>
#include "EventsManager.h"
#include "PropertiesWindow.h"
#include "SceneInfoWind.h"
#include <QWidgetList>
#include "KGraphicsScene.h"
#include "SaveWin.h"

class CustomGraphicsView;
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);

    ~MainWindow();

    void setProgrammaticChange(bool value)
    { isProgrammaticChange = value; }
    bool getProgrammaticChange() const
    { return isProgrammaticChange; }


    bool getIsResizing() const {return isResizing;}
    bool getIsMoving() const {return isMoving;}


private slots:
    void openImage();
    // bool saveImageAs(QGraphicsScene *scene);
    void saveImage();
    void convertToGrayscale();
    void setDrawRectMode();
    void setDrawCircleMode();
    void setDrawBrushMode();
    void setDrawLineMode();
    void setEraserMode();
    void setMoveToolMode();
    void setHandToolMode();
    void setTextToolMode();
    void setDrawPolygonMode();
    void setDrawArrowMode();
    void setDrawStarMode();
    void setDrawHeartMode();
    void setDrawBubbleMode();

    void clearScene();
    void pickColor();

    void zoomIn();
    void zoomOut();

    void onSizeChanged(int newSz);
    void onViewMousePressed(QMouseEvent* event);
    void onViewMouseMoved(QMouseEvent* event);
    void onViewMouseReleased(QMouseEvent* event);

    void handleItemSelected(int opacity,QGraphicsItem *item);
    void handleItemDeselected();
    void handleTextSelected();
    void handleTextDeselected();

    void setTextFont(const QFont &font);
    void setTextColor(const QColor &color);
    void setTextSize(int sz);
    void setBoldText(bool bold);
    void setItalicText(bool italic);
    void setUnderlineText(bool underline);
    void setStrikeText(bool strike);


    void saveSettings();
    void loadSettings();

    void handToolEnabled();
    void toggleTextEdittibility(bool enabled);

    void finishPolygon();

    void onAddShapeCommand(QGraphicsItem *item);
    void onRemoveShapeCommand(QGraphicsItem *item);

    void changeItemsBorderColor();
    void onChangeBorderSz(int sz);
    void changeBrushColor();
    void lock();
    void unlock();
    void onShapeSizeChanged(qreal width,qreal height);
    void onItemDoubleClicked(QGraphicsItem *item);
    void onShapePosChanged(qreal x, qreal y);

    void updatePropWindow(QGraphicsItem *kpathItem);
    void onItemUpdated(QGraphicsItem *item);

    void sendToFront();
    void sendToBack();
    void sendFroward();
    void sendBackward();

    void onListItemClicked(QGraphicsItem *item);
    void deleteLstItem(QGraphicsItem *item);

    void toggleInfoWindow(bool checked);
    void toggleUtilityWindow(bool checked);
    void toggleTextToolWindow(bool checked);
//    void onListItemDoubleClicked();

    void onContextMenuRequested(QPointF scenePos,QGraphicsItem *item);
    void showPropWindow();
    void updatePasteState();
    // void updateDeleteState();
    void groupItems();
    void ungroupItems();

protected:
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void mouseDoubleClickEvent(QMouseEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;

private:

    //Functions
    void setupButtons();
    void setupToolbar();
    void setupLayouts();
    void setupStyles();
    void setupConnections();
    void setupActions();
    void setupMenues();

    void createUtilityWindow();
    void createTextWindow();
    void setupButtonGroups();
    void setupFunctionCallback();

    void resetZoom();

    Edge isNearEdge(const QPointF &cursorPos, QGraphicsItem *item);


    void activateDrawButtons();

    QString colorNameFromColor(const QColor &color);
    QGraphicsTextItem *getSelectedTextItem();
    void applyTextChanges();

    void alignInfoWindow();
    void infoWinDock();



    QToolBar *drawingsToolbar;
    QToolBar *adjustmentsToolbar;

    QDockWidget *infoWinDoc;
    QDockWidget *textDock;

    QPointF lastPoint;

    Ui::MainWindow *ui;
    // QGraphicsScene* scene;
    KGraphicsScene *scene;
    DrawingManager *drawingManager;
    CustomGraphicsView *customView;
    QPointF startPoint;
    QGraphicsItem *currentShape;

    QRectF initialSceneRect;
    QGraphicsRectItem *transparentRect;

    QPointF rectStartPoint;
    QPointF circleStartPoint;
    QPointF lineStartPoint;
    QGraphicsRectItem *tempRect = nullptr;
    QGraphicsEllipseItem *tempCircle = nullptr;
    QSlider *szSlider;
    QSlider *opacitySlider;
    QGraphicsItem *selectedItem = nullptr;

    QPushButton *btnBrush;
    QPushButton *btnLine;
    QPushButton *btnEraser;
    QPushButton *btnMove;  
    QPushButton *btnChangeItemFill;
    QPushButton *btnLoad;
    QPushButton *btnZoomIn;
    QPushButton *btnZoomOut;
    QPushButton *btnHandTool;
    QPushButton *btnTextTool;
    QPushButton *btnPolygon;
    QPushButton *btnFinishPolygon;
    QPushButton *btnUndo;
    QPushButton *btnRedo;
    QPushButton *btnArrow;
    QPushButton *btnStar;
    QPushButton *btnChangeBorder;
    QPushButton *btnBrushColor;
    QPushButton *btnLocked;
    QPushButton *btnUnlocked;
    QPushButton *btnSendFront;
    QPushButton *btnSendBack;
    QPushButton *btnSendForward;
    QPushButton *btnSendBackward;
    QPushButton *btnGroup;
    QPushButton *btnUngroup;
    QPushButton *btnSaveAs;
    QPushButton *btnHeart;
    QPushButton *btnBubble;

    QButtonGroup *buttonGroup;
    QButtonGroup *btnGroupLock;

    bool isProgrammaticChange = false;

    bool isResizing = false;
    bool isMoving = false;

    Edge currentEdge = None;

    const double zoomFactor = 1.5;

    TextTool *textTool = nullptr;
    QString str;
    bool ok;

    QFontComboBox *fontComboBox;
    QComboBox *colorComboBox;
    QSpinBox *fontSizeSpinBox;
    QCheckBox *editCheckBox;
    QToolButton *boldButton;
    QToolButton *italicButton;
    QToolButton *underlineButton;
    QToolButton *strickThroughButton;

    bool isDrawingPolygon = false;

    QUndoStack *undoStack;


    QAction *undoAction;
    QAction *redoAction;
    QAction *copyAction;
    QAction *pasteAction;
    QAction *sendBackAction;
    QAction *sendFrontAction;
    QAction *sendForwardAction;
    QAction *sendBackwardAction;
    QAction *lockAction;
    QAction *unlockAction;
    QAction *deleteAction;
    QAction *propAction;
    QAction *selectAllAction;
    QAction *clearAction;
    QAction *saveImageAsAction;

    QPointF arrowStartPoint;
    QPointF starStartPoint;
    QGraphicsPathItem *tempArrow = nullptr;
    QGraphicsPathItem *tempStar = nullptr;

    QSpinBox *borderSizeSpin;
    QLabel *borderSzLabel;
    QLabel *brushSzLabel;
    QLabel *opacityLabelText;
    QLabel *brushColorLbel;
    QLabel *starHeadsLb;
    QLabel *starHeadsLbV;
    QDial *starHeadsDial;
    QLabel *showBrushColorLb;
    QLabel *changeItemFillLb;
    QLabel *showChangeItmFillLb;
    QLabel *showChangeBorderLb;
    QLabel *changeBorderLb;

    QCheckBox *changeSysClrCheck;

    QMenu *windowMenu;
    QAction *toggleInfoWin;
    QAction *toggleUtilityWin;
    QAction *toggleTextToolAc;

    QMenu *editMenu;
    QMenu *shapeContextMenu;
    QMenu *sceneContextMenu;

    QDockWidget *utilityWindow;

    EventsManager *eventsManager;
    PropertiesWindow *propWin;
    SceneInfoWindo *infoWin;


    // Standard file filters
    QStringList filters = {"PNG (*.png)", "JPEG (*.jpg;*.jpeg)",
                           "BMP (*.bmp)", "GIF (*.gif)"};


    // QMenu *contextMenu;

    // QMenu contextMenu;

//    QSlider *szSlider;

};

#endif // MAINWINDOW_H
