#ifndef ROOMEDITOR_H
#define ROOMEDITOR_H

#include "../globals/graphics_constants.h"
#include "../globals/rom_constants.h"
#include "../globals/rom_variables.h"
#include "../rom_utils/tileset.h"

#include <QMainWindow>
#include <QBrush>
#include <QColor>
#include <QDockWidget>
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
    RoomEditor(QWidget *parent = nullptr);
    ~RoomEditor();
    
private:
    void OpenROM(QString romPath);
    
    Ui::RoomEditor *ui;
    // Room Editor area
    QGraphicsView *roomEditorGraphicsView;
    QGraphicsScene *roomEditorGraphicsScene;
    // Tile Picker area
    QDockWidget *tilePickerDockWidget;
    QGraphicsView *tilePickerGraphicsView;
    QGraphicsScene *tilePickerGraphicsScene;
    // Room Info area
    QDockWidget *roomInfoDockWidget;
    
};
#endif // ROOMEDITOR_H
