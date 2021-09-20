#include "room_editor.h"
#include "./ui_room_editor.h"

RoomEditor::RoomEditor(QWidget *parent) : QMainWindow(parent), ui(new Ui::RoomEditor)
{
    ui->setupUi(this);
    
    // Initialize UI elements
    InitRoomEditor();
    InitBlockPicker();
    InitRoomInfo();
    InitStatusBar();
    
    // Populate UI elements after initialization is complete
    // This specific order is important because InitRoomInfo sets the
    // starting room and state, which PopulateBlockPicker uses to
    // populate its tileset and block graphics, which PopulateRoomEditor uses
    // to populate the graphics for the loaded room data.
    ChangeRoom();
    
    // Install the event filters after our init and first population are completed
    roomViewerGraphicsScene->installEventFilter(this);
    blockPickerGraphicsScene->installEventFilter(this);
}

RoomEditor::~RoomEditor()
{
    delete ui;
}

/// Initialize Room Editor UI elements.
void RoomEditor::InitRoomEditor()
{
    roomViewerGraphicsScene = new QGraphicsScene();
    roomViewerGraphicsScene->setBackgroundBrush(QBrush(Qt::black));
    roomViewerGraphicsView = new QGraphicsView(roomViewerGraphicsScene);
    
    setCentralWidget(roomViewerGraphicsView);
}

/// Initialize Block Picker UI elements.
void RoomEditor::InitBlockPicker()
{
    blockPickerDockWidget = new QDockWidget(tr("Block Picker"), this);
    QDockWidget::DockWidgetFeatures dockWidgetFeatures = (blockPickerDockWidget->features() & ~QDockWidget::DockWidgetClosable);
    blockPickerDockWidget->setFeatures(dockWidgetFeatures);
    QStyle *dockWidgetStyle = blockPickerDockWidget->style();
    int dockWidgetWidth = dockWidgetStyle->pixelMetric(QStyle::PM_DockWidgetFrameWidth);
    int titleBarHeight = dockWidgetStyle->pixelMetric(QStyle::PM_TitleBarHeight);
    int titleBarMargin = blockPickerDockWidget->style()->pixelMetric(QStyle::PM_DockWidgetTitleMargin);
    blockPickerDockWidget->setMinimumWidth(32 * PIXELS_PER_BLOCK + dockWidgetWidth);
    blockPickerDockWidget->setMinimumHeight(32 * PIXELS_PER_BLOCK + titleBarHeight + titleBarMargin);
    blockPickerDockWidget->size() = blockPickerDockWidget->minimumSize();
    blockPickerGraphicsScene = new QGraphicsScene();
    blockPickerGraphicsScene->setBackgroundBrush(QBrush(Qt::black));
    blockPickerGraphicsView = new QGraphicsView(blockPickerGraphicsScene, blockPickerDockWidget);
    blockPickerDockWidget->setWidget(blockPickerGraphicsView);
    
    addDockWidget(Qt::RightDockWidgetArea, blockPickerDockWidget);
}

/// Initialize Room Info UI elements.
void RoomEditor::InitRoomInfo()
{
    roomInfoDockWidget = new QDockWidget(tr("Room Info"), this);
    QDockWidget::DockWidgetFeatures dockWidgetFeatures = (roomInfoDockWidget->features() & ~QDockWidget::DockWidgetClosable);
    roomInfoDockWidget->setFeatures(dockWidgetFeatures);
    roomInfoDockWidget->setMinimumWidth(256);
    
    // Setup the comboboxes
    roomInfoAddressComboBox = new QComboBox();
    // Generate the list of room address strings
    QStringList roomAddressStrings;
    for (ushort &address : GlobalRooms.keys())
    {
        roomAddressStrings.append(QString::number(address, 16).toUpper());
    }
    roomInfoAddressComboBox->addItems(roomAddressStrings);
    connect(roomInfoAddressComboBox, &QComboBox::currentIndexChanged, this, &RoomEditor::roomInfoAddressComboBox_currentIndexChanged);
    // Set the currentRoom variable
    currentRoom = GlobalRooms.values().at(0);
    
    // Setup the frame and layout that holds the comboboxes
    QFrame *frame = new QFrame();
    QFormLayout *layout = new QFormLayout();
    layout->addRow(tr("Room Address:"), roomInfoAddressComboBox);
    frame->setLayout(layout);
    roomInfoDockWidget->setWidget(frame);
    
    addDockWidget(Qt::RightDockWidgetArea, roomInfoDockWidget);
}

/// Initialize the status bar and its elements, like the zoom slider
void RoomEditor::InitStatusBar()
{
    zoomSlider = new QSlider();
    zoomSlider->setOrientation(Qt::Horizontal);
    zoomSlider->setTickInterval(100);
    zoomSlider->setTickPosition(QSlider::TicksBelow);
    zoomSlider->setMinimum(100);
    zoomSlider->setMaximum(400);
    zoomSlider->setValue(100);
    zoomSlider->setSingleStep(100);
    zoomSlider->setPageStep(200);
    connect(zoomSlider, &QSlider::valueChanged, this, &RoomEditor::zoomSlider_valueChanged);
    ui->statusbar->addWidget(zoomSlider);
}

void RoomEditor::PopulateRoomViewer()
{
    // Clear the old graphics from RoomViewerGraphicsScene
    roomViewerGraphicsScene->clear();
    
    int tilesetId = currentRoom.States[0].GraphicSet;
    
    ////// Populate the Room Editor with the current room's blocks //////
    
    // Populate tile layer 2 graphics
    for (int i = 0; i < currentLevelData.TileLayer2.size(); ++i)
    {
        Block curBlock = currentLevelData.TileLayer2[i];

        // DEBUG: Draw block ID text
        //QGraphicsTextItem *text = new QGraphicsTextItem(QString::number(curBlock.BlockId));
        //text->setFont(QFont("Noto Mono"));
        //text->setPos((i % (currentRoom.Header.Width * BLOCKS_PER_ROOM)) * PIXELS_PER_BLOCK, (i / (currentRoom.Header.Width * BLOCKS_PER_ROOM)) * PIXELS_PER_BLOCK);
        //layer2Group->addToGroup(text);

        // Draw block
        QImage blockImage = GetImageForBlock(curBlock.BlockNum, GlobalTileTables[tilesetId], GlobalTileGraphics[tilesetId]);

        // Generate a color table from the palette
        QList<QRgb> paletteColorTable;
        for (int palNum = 0; palNum < GlobalPalettes[tilesetId].size(); ++palNum)
        {
            // Mask out the alpha for the first color in each bank, it's intended to be transparent
            QRgb color = SnesToPcColor(GlobalPalettes[tilesetId][palNum]);
            if (palNum % 16 == 0)
                paletteColorTable.append(color & 0x00FFFFFF);
            else
                paletteColorTable.append(color);
        }
        
        // Apply the color table to the image
        blockImage.setColorTable(paletteColorTable);
        
        // Adjust x/y flipping
        blockImage = blockImage.mirrored(curBlock.XFlip, curBlock.YFlip);

        QGraphicsPixmapItem *blockPixmapItem = new QGraphicsPixmapItem(QPixmap::fromImage(blockImage));
        int xPos = (i % (BLOCKS_PER_ROOM_SEGMENT * currentRoom.Header.Width)) * PIXELS_PER_BLOCK;
        int yPos = (i / (BLOCKS_PER_ROOM_SEGMENT * currentRoom.Header.Width)) * PIXELS_PER_BLOCK;
        blockPixmapItem->setPos(xPos, yPos);
        roomViewerGraphicsScene->addItem(blockPixmapItem);
    }
    
    // Populate tile layer 1 graphics
    for (int i = 0; i < currentLevelData.TileLayer1.size(); ++i)
    {
        Block curBlock = currentLevelData.TileLayer1[i];

        // Draw block ID text
        //QGraphicsTextItem *text = new QGraphicsTextItem(QString::number(curBlock.BlockId));
        //text->setFont(QFont("Noto Mono"));
        //text->setPos((i % (currentRoom.Header.Width * BLOCKS_PER_ROOM)) * PIXELS_PER_BLOCK, (i / (currentRoom.Header.Width * BLOCKS_PER_ROOM)) * PIXELS_PER_BLOCK);
        //layer1Group->addToGroup(text);

        // Draw block
        QImage blockImage = GetImageForBlock(curBlock.BlockNum, GlobalTileTables[currentRoom.States[0].GraphicSet], GlobalTileGraphics[currentRoom.States[0].GraphicSet]);

        // Generate a color table from the palette
        QList<QRgb> paletteColorTable;
        for (int palNum = 0; palNum < GlobalPalettes[tilesetId].size(); ++palNum)
        {
            // Mask out the alpha for the first color in each bank, it's intended to be transparent
            QRgb color = SnesToPcColor(GlobalPalettes[tilesetId][palNum]);
            if (palNum % 16 == 0)
                paletteColorTable.append(color & 0x00FFFFFF);
            else
                paletteColorTable.append(color);
        }
        
        // Apply the color table to the image
        blockImage.setColorTable(paletteColorTable);
        
        // Adjust x/y flipping
        blockImage = blockImage.mirrored(curBlock.XFlip, curBlock.YFlip);

        QGraphicsPixmapItem *blockPixmapItem = new QGraphicsPixmapItem(QPixmap::fromImage(blockImage));
        int xPos = (i % (BLOCKS_PER_ROOM_SEGMENT * currentRoom.Header.Width)) * PIXELS_PER_BLOCK;
        int yPos = (i / (BLOCKS_PER_ROOM_SEGMENT * currentRoom.Header.Width)) * PIXELS_PER_BLOCK;
        blockPixmapItem->setPos(xPos, yPos);
        roomViewerGraphicsScene->addItem(blockPixmapItem);
    }
    
    //roomViewerGraphicsScene->update();
}

void RoomEditor::PopulateBlockPicker()
{
    int tilesetId = currentRoom.States.at(0).GraphicSet;    // TEMPORARY, GET THIS FROM CURRENT ROOM STATE INSTEAD!
    QImage decodedTileGraphics = GlobalTileGraphics[tilesetId];
    
    // DEBUGGING ONLY!!!
    // Apply a temporary grayscale palette to the indexed pixel data
    //QVector<QRgb> grayscaleColorTable;
    //for (int i = 0; i < 16; ++i)
    //{
    //    int brightness = 16 * i;
    //    if (brightness > 255)
    //        brightness = 255;
    //    uint color = (0xFF << 24) | (brightness << 16) | (brightness << 8) | brightness;
    //    grayscaleColorTable.append(QRgb(color));
    //}
    //decodedTileGraphics.setColorTable(grayscaleColorTable);
    //QImage converted = decodedTileGraphics.convertToFormat(QImage::Format_RGB32);
    //converted.save("decodedTileGraphicsCppDebug.png");

    // Generate a color table from the palette
    QList<QRgb> paletteColorTable;
    for (int palNum = 0; palNum < GlobalPalettes[tilesetId].size(); ++palNum)
    {
        // Mask out the alpha for the first color in each bank, it's intended to be transparent
        QRgb color = SnesToPcColor(GlobalPalettes[tilesetId][palNum]);
        if (palNum % 16 == 0)
            paletteColorTable.append(color & 0x00FFFFFF);
        else
            paletteColorTable.append(color);
    }
    
    // Apply the color table to the image
    //decodedTileGraphics.setColorTable(paletteColorTable);
    
    // Reset the block picker & re-initialize graphics items
    blockPickerGraphicsScene->clear();
    int blockCount = 1024;
    int blocksWide = 32;    // number of blocks per line
    int blocksTall = blockCount / blocksWide;
    blockPickerGraphicsScene->setSceneRect(0, 0, blocksWide * PIXELS_PER_BLOCK, blocksTall * PIXELS_PER_BLOCK);
    for (int blockNum = 0; blockNum < blockCount; ++blockNum)
    {
        // Get block image
        QImage blockImage = GetImageForBlock(blockNum, GlobalTileTables[tilesetId], decodedTileGraphics);
        blockImage.setColorTable(paletteColorTable);

        // Draw the block to the picker scene
        QGraphicsPixmapItem *blockItem = blockPickerGraphicsScene->addPixmap(QPixmap::fromImage(blockImage));
        int xPos = (blockNum % blocksWide) * PIXELS_PER_BLOCK;
        int yPos = (blockNum / blocksWide) * PIXELS_PER_BLOCK;
        blockItem->setPos(xPos, yPos);

        // DEBUGGING ONLY!!!
        //QGraphicsTextItem *text = blockPickerGraphicsScene->addText(QString::number(blockNum, 16).toUpper());
        //text->setFont(QFont("Noto Mono", 8));
        //text->setDefaultTextColor(QColor(Qt::red));
        //text->setPos((blockNum % blockPickerWidth) * PIXELS_PER_BLOCK - 4, (blockNum / blockPickerWidth) * PIXELS_PER_BLOCK);
        //convertedBlockImage.save("block" + QString::number(blockNum) + ".png");
    }
}

void RoomEditor::PopulateBlockPickerDEBUG()
{
    // Load the current room's tile set from the 0th room state
    int tilesetNum = currentRoom.States.at(0).GraphicSet;    // TEMPORARY, GET THIS FROM CURRENT ROOM STATE INSTEAD!
    QImage currentTileGraphics = GlobalTileGraphics[tilesetNum];
    
    // Load the appropriate color palette from the 0th room state
    QList<QRgb> paletteColorTable;
    for (int paletteNum = 0; paletteNum < GlobalPalettes[tilesetNum].size(); ++paletteNum)
    {
        // Mask out the alpha for the first color in each bank, it's intended to be transparent
        QRgb color = SnesToPcColor(GlobalPalettes[tilesetNum][paletteNum]);
        if (paletteNum % 16 == 0)
            paletteColorTable.append(color & 0x00FFFFFF);
        else
            paletteColorTable.append(color);
    }
    
    // Apply the color table to the image
    currentTileGraphics.setColorTable(paletteColorTable);
    
    // Reset the block picker scene and start painting the tile graphics to it
    blockPickerGraphicsScene->clear();
    int tileCount = currentTileGraphics.height() / PIXELS_PER_TILE;
    int tilesWide = 16; // Number of tiles per line
    int tilesTall = tileCount / tilesWide;
    blockPickerGraphicsScene->setSceneRect(0, 0, tilesWide * PIXELS_PER_TILE, tilesTall * PIXELS_PER_TILE);
    for (int tileNum = 0; tileNum < tileCount; ++tileNum)
    {
        // Get the tile image
        QImage tileImage = GetImageForTile(tileNum, currentTileGraphics);
        
        // Draw the tile image
        QGraphicsPixmapItem *tilePixmapItem = blockPickerGraphicsScene->addPixmap(QPixmap::fromImage(tileImage));
        int xPos = (tileNum % tilesWide) * PIXELS_PER_TILE;
        int yPos = (tileNum / tilesWide) * PIXELS_PER_TILE;
        tilePixmapItem->setPos(xPos, yPos);
    }
}

void RoomEditor::ChangeRoom()
{
    // Load the current state's LevelData into memory
    currentLevelData = ReadLevel(currentRoom, 0);   // TEMPORARY, GET THIS FROM CURRENT ROOM STATE INSTEAD!
    
    PopulateBlockPicker();
    PopulateRoomViewer();
}

void RoomEditor::roomInfoAddressComboBox_currentIndexChanged(int index)
{
    // Don't try to access an invalid room index
    if (index < 0 || index >= GlobalRooms.values().size())
        return;
    
    // Trigger the UI elements to repopulate
    currentRoom = GlobalRooms.values().at(index);
    ChangeRoom();
}

void RoomEditor::zoomSlider_valueChanged(int value)
{
    QTransform roomEditorTransform;
    qreal zoomFactor = value / 100.0f;
    roomEditorTransform.scale(zoomFactor, zoomFactor);
    
    roomViewerGraphicsView->setTransform(roomEditorTransform);
}

bool RoomEditor::eventFilter(QObject *target, QEvent *event)
{
    if (event->type() == QEvent::GraphicsSceneMousePress)
    {
        QGraphicsSceneMouseEvent *mouseEvent = static_cast<QGraphicsSceneMouseEvent *>(event);
        
        // Determine which QGraphicsScene object the click took place over
        if (target == (QObject *)roomViewerGraphicsScene)
        {
            QGraphicsItem *item = roomViewerGraphicsScene->itemAt(mouseEvent->scenePos(), QTransform());
            QPoint sceneBlockPos = mouseEvent->scenePos().toPoint();
            QPoint roomBlockPos = QPoint(sceneBlockPos.x() / PIXELS_PER_BLOCK, sceneBlockPos.y() / PIXELS_PER_BLOCK);
            
            // If we're left-clicking, replace the block in Layer 1
            if (mouseEvent->button() == Qt::LeftButton)
            {
                int blockIndex = (roomBlockPos.y() * currentRoom.Header.Width * BLOCKS_PER_ROOM_SEGMENT) + roomBlockPos.x();
                
                Block b;
                b.BlockNum = 0;
                b.XFlip = false;
                b.YFlip = false;
                b.BlockType = 0;
                currentLevelData.TileLayer1[blockIndex] = b;
                
            }
            
            PopulateRoomViewer();
            return true;
        }
        else if (target == (QObject *)blockPickerGraphicsScene)
        {
            return true;
        }
        
        return false;
    }
    
    return false;
}
