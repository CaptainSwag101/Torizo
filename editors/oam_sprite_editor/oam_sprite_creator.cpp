#include "oam_sprite_creator.h"
#include "ui_oam_sprite_creator.h"

#include "globals/graphics_constants.h"
#include "globals/rom_variables.h"
#include "rom_utils/tileset.h"

OAMSpriteCreator::OAMSpriteCreator(QWidget *parent) : QMainWindow(parent), ui(new Ui::OAMSpriteCreator)
{
    ui->setupUi(this);
    
    // Initialize UI
    InitTilePicker();
    InitFrameEditor();
}

OAMSpriteCreator::~OAMSpriteCreator()
{
    delete ui;
}

void OAMSpriteCreator::InitTilePicker()
{
    tilePickerScene = new QGraphicsScene();
    tilePickerScene->setBackgroundBrush(QBrush(Qt::black));
    ui->tilePickerGraphicsView->setScene(tilePickerScene);
}

void OAMSpriteCreator::InitFrameEditor()
{
    frameEditorScene = new QGraphicsScene();
    frameEditorScene->setBackgroundBrush(QBrush(Qt::black));
    ui->frameEditorGraphicsView->setScene(frameEditorScene);
}

void OAMSpriteCreator::on_loadTileGFXButton_clicked()
{
    // First, load the address from the user input field, provided it's valid
    bool ok;
    
    // Load as hexadecimal
    int tileGfxAddress = ui->tileGFXAddressInput->text().toInt(&ok, 16);
    
    if (!ok) return;
    
    tilePickerScene->clear();
    
    QByteArray tileGfxData = ROMData.mid(tileGfxAddress, TILE_GRAPHICS_SIZE);
    loadedTileGraphics = DecodeBitplaneGraphics(tileGfxData, 4);
    
    // If a palette is already loaded, apply it immediately. Otherwise, generate a grayscale one
    if (loadedPalette.size() > 0)
    {
        loadedTileGraphics.setColorTable(loadedPalette);
    }
    
    // Add the tile graphics to the tile picker scene
    int tileCount = loadedTileGraphics.height() / PIXELS_PER_TILE;
    int tilesPerRow = 16;
    for (int tileNum = 0; tileNum < tileCount; ++tileNum)
    {
        int xPos = (tileNum % tilesPerRow) * PIXELS_PER_TILE;
        int yPos = (tileNum / tilesPerRow) * PIXELS_PER_TILE;
        QImage tileImage = GetImageForTile(tileNum, loadedTileGraphics);
        
        QGraphicsPixmapItem *tilePixmapItem = tilePickerScene->addPixmap(QPixmap::fromImage(tileImage));
        tilePixmapItem->setPos(xPos, yPos);
    }
}

void OAMSpriteCreator::on_loadPaletteButton_clicked()
{
    
}
