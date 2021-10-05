#ifndef ROOMEDITOR_H
#define ROOMEDITOR_H

#include "../../../globals/graphics_constants.h"
#include "../../../globals/rom_constants.h"
#include "../../../globals/rom_variables.h"
#include "../../../rom_utils/tileset.h"

#include <QMainWindow>
#include <QBrush>
#include <QColor>
#include <QComboBox>
#include <QDockWidget>
#include <QFormLayout>
#include <QGraphicsItem>
#include <QGraphicsPixmapItem>
#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsView>
#include <QImage>
#include <QMouseEvent>
#include <QPointF>
#include <QRgb>
#include <QSlider>

namespace Ui {
class RoomEditor;
}

class RoomEditor : public QMainWindow
{
    Q_OBJECT
    
public:
    /* Methods */
    RoomEditor(QWidget *parent = nullptr);
    ~RoomEditor();
    
    /* Variables */
    Room currentRoom;
    LevelData currentLevelData;

private slots:
    void roomInfoAddressComboBox_currentIndexChanged(int index);
    void zoomSlider_valueChanged(int value);
    
private:
    /* Methods */
    // UI stuff
    void InitRoomEditor();
    void InitBlockPicker();
    void InitRoomInfo();
    void InitStatusBar();
    void PopulateRoomViewer();
    void PopulateBlockPicker();
    void PopulateBlockPickerDEBUG();
    void ChangeRoom();
    // Event filters
    bool eventFilter(QObject *target, QEvent *event);
    
    /* Variables */
    // UI stuff
    Ui::RoomEditor *ui;
    // Room Editor area
    QGraphicsView *roomViewerGraphicsView;
    QGraphicsScene *roomViewerGraphicsScene;
    // Block Picker area
    QDockWidget *blockPickerDockWidget;
    QGraphicsView *blockPickerGraphicsView;
    QGraphicsScene *blockPickerGraphicsScene;
    // Room Info area
    QDockWidget *roomInfoDockWidget;
    QComboBox *roomInfoAddressComboBox;
    QComboBox *roomInfoStateComboBox;
    // Status bar area
    QSlider *zoomSlider;
};
#endif // ROOMEDITOR_H
