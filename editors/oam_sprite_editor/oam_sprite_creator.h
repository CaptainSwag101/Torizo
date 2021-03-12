#ifndef OAM_SPRITE_CREATOR_H
#define OAM_SPRITE_CREATOR_H

#include "oam_sprite.h"

#include <QByteArray>
#include <QMainWindow>
#include <QFileDialog>
#include <QDataStream>
#include <QImage>
#include <QPixmap>
#include <QGraphicsView>
#include <QGraphicsPixmapItem>
#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QEvent>
#include <QRgb>
#include <QString>
#include <QMessageBox>

namespace Ui {
class OAMSpriteCreator;
}

class OAMSpriteCreator : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit OAMSpriteCreator(QWidget *parent = nullptr);
    ~OAMSpriteCreator();
    
private slots:
    void on_loadTileGFXButton_clicked();
    
    void on_loadPaletteButton_clicked();
    
private:
    void InitTilePicker();
    void InitFrameEditor();
    void PopulateTilePicker();
    void PopulateFrameEditor();
    
    Ui::OAMSpriteCreator *ui;
    QGraphicsScene *tilePickerScene;
    QGraphicsScene *frameEditorScene;
    QImage loadedTileGraphics;
    QList<QRgb> loadedPalette;
    QList<OAMSprite> Sprites;
    constexpr static int TILE_GRAPHICS_SIZE = 256;
};

#endif // OAM_SPRITE_CREATOR_H
