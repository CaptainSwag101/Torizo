#ifndef ROOMEDITOR_H
#define ROOMEDITOR_H

#include "../globals/graphics_constants.h"
#include "../globals/rom_constants.h"
#include "../globals/rom_variables.h"
#include "../rom_utils/tileset.h"

#include <QMainWindow>
#include <QBrush>
#include <QColor>
#include <QComboBox>
#include <QDockWidget>
#include <QFormLayout>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>
#include <QImage>
#include <QRgb>

QT_BEGIN_NAMESPACE
namespace Ui { class RoomEditor; }
QT_END_NAMESPACE

class RoomEditor : public QMainWindow
{
    Q_OBJECT
    
public:
    /* Methods */
    RoomEditor(QWidget *parent = nullptr);
    ~RoomEditor();
    /* Variables */
    Room currentRoom;

private slots:
    void roomInfoAddressComboBox_currentIndexChanged(int index);
    
private:
    /* Methods */
    // UI stuff
    void InitRoomEditor();
    void InitBlockPicker();
    void InitRoomInfo();
    void PopulateRoomEditor();
    void PopulateBlockPicker();
    
    /* Variables */
    // UI stuff
    Ui::RoomEditor *ui;
    // Room Editor area
    QGraphicsView *roomEditorGraphicsView;
    QGraphicsScene *roomEditorGraphicsScene;
    // Block Picker area
    QDockWidget *blockPickerDockWidget;
    QGraphicsView *blockPickerGraphicsView;
    QGraphicsScene *blockPickerGraphicsScene;
    // Room Info area
    QDockWidget *roomInfoDockWidget;
    QComboBox *roomInfoAddressComboBox;
    QComboBox *roomInfoStateComboBox;
    
};
#endif // ROOMEDITOR_H
