#include "room_editor.h"
#include "./ui_room_editor.h"

RoomEditor::RoomEditor(QWidget *parent) : QMainWindow(parent), ui(new Ui::RoomEditor)
{
    ui->setupUi(this);
    
    // Initialize UI elements
    InitRoomEditor();
    InitBlockPicker();
    InitRoomInfo();
    
    // Populate UI elements after initialization is complete
    // This specific order is important because InitRoomInfo sets the
    // starting room and state, which PopulateBlockPicker uses to
    // populate its tileset and block graphics, which PopulateRoomEditor uses
    // to populate the graphics for the loaded room data.
    PopulateBlockPicker();
    PopulateRoomEditor();
}

RoomEditor::~RoomEditor()
{
    delete ui;
}

/// Initialize Room Editor UI elements.
void RoomEditor::InitRoomEditor()
{
    roomEditorGraphicsScene = new QGraphicsScene();
    roomEditorGraphicsScene->setBackgroundBrush(QBrush(Qt::black));
    roomEditorGraphicsView = new QGraphicsView(roomEditorGraphicsScene);
    
    setCentralWidget(roomEditorGraphicsView);
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

void RoomEditor::PopulateRoomEditor()
{
    
}

void RoomEditor::PopulateBlockPicker()
{
    int tilesetId = currentRoom.States.at(0).GraphicSet;    // TEMPORARY
    QImage decodedTileGraphics = GlobalTileGraphics[tilesetId];
    
    /*
    // DEBUGGING ONLY!!!
    // Apply a temporary grayscale palette to the indexed pixel data
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

    QImage converted = decodedTileGraphics.convertToFormat(QImage::Format_RGB32);
    converted.save("decodedTileGraphicsCppDebug.png");
    */

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
    
    // Apply the color table to the image
    decodedTileGraphics.setColorTable(paletteColorTable);
    
    // Reset the block picker & re-initialize graphics items
    blockPickerGraphicsScene->clear();
    int blockCount = 1024;
    int blocksWide = 32;  // number of blocks per line
    int blocksTall = blockCount / blocksWide;
    blockPickerGraphicsScene->setSceneRect(0, 0, blocksWide * PIXELS_PER_BLOCK, blocksTall * PIXELS_PER_BLOCK);
    for (int blockNum = 0; blockNum < blockCount; ++blockNum)
    {
        // Get block image and cache it
        QImage blockImage = GetImageForBlock(blockNum, GlobalTileTables[tilesetId], decodedTileGraphics);
        blockImage.setColorTable(paletteColorTable);

        // Draw the block to the picker scene
        QGraphicsPixmapItem *blockItem = blockPickerGraphicsScene->addPixmap(QPixmap::fromImage(blockImage));
        blockItem->setPos((blockNum % blocksWide) * PIXELS_PER_BLOCK, (blockNum / blocksWide) * PIXELS_PER_BLOCK);

        // DEBUGGING ONLY!!!
        //QGraphicsTextItem *text = blockPickerScene->addText(QString::number(blockNum, 16).toUpper());
        //text->setFont(QFont("Noto Mono", 8));
        //text->setDefaultTextColor(QColor(Qt::red));
        //text->setPos((blockNum % blockPickerWidth) * PIXELS_PER_BLOCK - 4, (blockNum / blockPickerWidth) * PIXELS_PER_BLOCK);

        //convertedBlockImage.save("block" + QString::number(blockNum) + ".png");
    }
}

void RoomEditor::roomInfoAddressComboBox_currentIndexChanged(int index)
{
    // Don't try to access an invalid room index
    if (index < 0 || index >= GlobalRooms.values().size())
        return;
    
    // Trigger the UI elements to repopulate
    currentRoom = GlobalRooms.values().at(index);
    PopulateBlockPicker();
    PopulateRoomEditor();
}
