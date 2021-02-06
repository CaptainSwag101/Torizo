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
    
    
    
    //TEMPORARY: Populate tilePicker
    // Apply a temporary grayscale palette to the indexed pixel data
    int tilesetId = 0;
    QImage decodedTileGraphics = GlobalTileGraphics[tilesetId];
    /*
    QVector<QRgb> grayscaleColorTable;
    for (int i = 0; i < 16; ++i)
    {
        int brightness = 16 * i;
        if (brightness > 255)
            brightness = 255;
        uint color = (0xFF << 24) | (brightness << 16) | (brightness << 8) | brightness;
        grayscaleColorTable.append(QRgb(color));
    }
    decodedTileGraphics.setColorTable(grayscaleColorTable);
    */

    // DEBUGGING ONLY!!!
    //QImage converted = decodedTileGraphics.convertToFormat(QImage::Format_RGB32);
    //converted.save("decodedTileGraphicsCppDebug.png");

    // Generate a color table from the palette
    QVector<QRgb> paletteColorTable;
    for (int palNum = 0; palNum < GlobalPalettes[tilesetId].size(); ++palNum)
    {
        // Mask out the first color in the each bank, it's intended to be transparent
        if (palNum % 16 == 0)
            paletteColorTable.append(QRgb(0x00000000));
        else
            paletteColorTable.append(SnesToPcColor(GlobalPalettes[tilesetId][palNum]));
    }
    
    // Apply the new color table to the image
    decodedTileGraphics.setColorTable(paletteColorTable);
    
    
    // Reset the block picker & re-initialize graphics items
    tilePickerGraphicsScene->clear();
    int blockCount = 1024;
    int blocksWide = 32;  // number of blocks per line
    int blocksTall = blockCount / blocksWide;
    tilePickerGraphicsScene->setSceneRect(0, 0, blocksWide * PIXELS_PER_BLOCK, blocksTall * PIXELS_PER_BLOCK);
    for (int blockNum = 0; blockNum < blockCount; ++blockNum)
    {
        // Get block image and cache it
        QImage blockImage = GetImageForBlock(blockNum, GlobalTileTables[tilesetId], decodedTileGraphics);
        blockImage.setColorTable(paletteColorTable);

        // Draw the block to the picker scene
        QGraphicsPixmapItem *blockItem = tilePickerGraphicsScene->addPixmap(QPixmap::fromImage(blockImage));
        blockItem->setPos((blockNum % blocksWide) * PIXELS_PER_BLOCK, (blockNum / blocksWide) * PIXELS_PER_BLOCK);

        // DEBUGGING ONLY!!!
        //QGraphicsTextItem *text = blockPickerScene->addText(QString::number(blockNum, 16).toUpper());
        //text->setFont(QFont("Noto Mono", 8));
        //text->setDefaultTextColor(QColor(Qt::red));
        //text->setPos((blockNum % blockPickerWidth) * PIXELS_PER_BLOCK - 4, (blockNum / blockPickerWidth) * PIXELS_PER_BLOCK);

        //convertedBlockImage.save("block" + QString::number(blockNum) + ".png");
    }
    //blockPickerGraphicsView->update();
}

RoomEditor::~RoomEditor()
{
    delete ui;
}

