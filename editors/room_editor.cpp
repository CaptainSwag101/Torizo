#include "room_editor.h"
#include "./ui_room_editor.h"

RoomEditor::RoomEditor(QWidget *parent) : QMainWindow(parent), ui(new Ui::RoomEditor)
{
    ui->setupUi(this);
    
    // Initialize Room Editor UI elements
    roomEditorGraphicsScene = new QGraphicsScene();
    roomEditorGraphicsScene->setBackgroundBrush(QBrush(Qt::black));
    roomEditorGraphicsView = new QGraphicsView(roomEditorGraphicsScene);
    setCentralWidget(roomEditorGraphicsView);
    // Initialize Tile Picker UI elements
    tilePickerDockWidget = new QDockWidget(tr("Tile Picker"), this);
    QDockWidget::DockWidgetFeatures dockWidgetFeatures = (tilePickerDockWidget->features() & ~QDockWidget::DockWidgetClosable);
    tilePickerDockWidget->setFeatures(dockWidgetFeatures);
    tilePickerDockWidget->setMinimumWidth(256);
    tilePickerGraphicsScene = new QGraphicsScene();
    tilePickerGraphicsScene->setBackgroundBrush(QBrush(Qt::black));
    tilePickerGraphicsView = new QGraphicsView(tilePickerGraphicsScene, tilePickerDockWidget);
    tilePickerDockWidget->setWidget(tilePickerGraphicsView);
    addDockWidget(Qt::RightDockWidgetArea, tilePickerDockWidget);
    // Initialize Room Info UI elements
    roomInfoDockWidget = new QDockWidget(tr("Room Info"), this);
    roomInfoDockWidget->setFeatures(dockWidgetFeatures);
    roomInfoDockWidget->setMinimumWidth(256);
    addDockWidget(Qt::RightDockWidgetArea, roomInfoDockWidget);
}

RoomEditor::~RoomEditor()
{
    delete ui;
}

