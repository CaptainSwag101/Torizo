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
    
    if (!ok || (tileGfxAddress + TILE_GRAPHICS_SIZE) > ROMData.size()) return;
    
    QByteArray tileGfxData = ROMData.mid(tileGfxAddress, TILE_GRAPHICS_SIZE);
    loadedTileGraphics = DecodeBitplaneGraphics(tileGfxData, 4);
    
    // If a palette is already loaded, apply it immediately. Otherwise, generate a grayscale one
    if (loadedPalette.size() > 0)
    {
        loadedTileGraphics.setColorTable(loadedPalette);
    }
    else
    {
        // Apply a temporary grayscale palette to the indexed pixel data
        QList<QRgb> grayscaleColorTable;
        for (int i = 0; i < 16; ++i)
        {
            int brightness = 16 * i;
            if (brightness > 255)
                brightness = 255;
            uint color = (0xFF << 24) | (brightness << 16) | (brightness << 8) | brightness;
            grayscaleColorTable.append(QRgb(color));
        }
        loadedTileGraphics.setColorTable(grayscaleColorTable);
    }
    
    // Add the tile graphics to the tile picker scene
    tilePickerScene->clear();
    int tileCount = loadedTileGraphics.height() / PIXELS_PER_TILE;
    int tilesPerRow = 16;
    tilePickerScene->setSceneRect(0, 0, tilesPerRow * PIXELS_PER_TILE, tilesPerRow * PIXELS_PER_TILE);
    QTransform transform;
    transform.scale(2, 2);
    ui->tilePickerGraphicsView->setTransform(transform);
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
    // First, load the address from the user input field, provided it's valid
    bool ok;
    
    // Load as hexadecimal
    int paletteAddress = ui->paletteAddressInput->text().toInt(&ok, 16);
    
    if (!ok || paletteAddress >= ROMData.size()) return;
    
    // Load at most the uncompressed size of a 16-color palette
    //QByteArray compressedPaletteData = ROMData.mid(paletteAddress);  
    //QByteArray decompressedPaletteData = DecompressData(compressedPaletteData);
    QByteArray decompressedPaletteData = ROMData.mid(paletteAddress, DECOMPRESSED_PALETTE_DATA_SIZE);
    QDataStream paletteStream(decompressedPaletteData);
    paletteStream.setByteOrder(QDataStream::LittleEndian);
    
    // Read the SNES colors
    int colorCount = decompressedPaletteData.size() / 2;
    QList<ushort> snesPalette;
    for (int c = 0; c < colorCount; ++c)
    {
        ushort snesColor;
        paletteStream >> snesColor;
        
        snesPalette.append(snesColor);
    }
    
    // Clear the old palette
    loadedPalette.clear();
    
    // Convert the SNES colors to PC RGB
    for (int c = 0; c < snesPalette.size(); ++c)
    {
        QRgb pcColor = SnesToPcColor(snesPalette[c]);
        
        loadedPalette.append(pcColor);
    }
}
