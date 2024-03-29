#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "editors/oam_sprite_editor/oam_sprite_creator.h"
#include "editors/room_editor/room_editor.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    
    setWindowFlag(Qt::MSWindowsFixedSizeDialogHint, true);
    setWindowFlag(Qt::Dialog, true);
    setFixedSize(size());
    statusBar()->setSizeGripEnabled(false);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_roomEditorButton_clicked()
{
    // First, check to make sure RoomEditor isn't already open
    if (activeEditors.contains(ui->roomEditorButton))
    {
        if (activeEditors[ui->roomEditorButton])
        {
            activeEditors[ui->roomEditorButton].get()->showNormal();
            return;
        }
    }

    activeEditors.insert(ui->roomEditorButton, std::make_shared<RoomEditor>(this));
    activeEditors[ui->roomEditorButton].get()->show();
}

void MainWindow::on_oamSpriteCreatorButton_clicked()
{
    // First, check to make sure OAMSpriteCreator isn't already open
    if (activeEditors.contains(ui->oamSpriteCreatorButton))
    {
        if (activeEditors[ui->oamSpriteCreatorButton])
        {
            activeEditors[ui->oamSpriteCreatorButton].get()->showNormal();
            return;
        }
    }

    activeEditors.insert(ui->oamSpriteCreatorButton, std::make_shared<OAMSpriteCreator>(this));
    activeEditors[ui->oamSpriteCreatorButton].get()->show();
}

void MainWindow::on_action_Open_ROM_triggered()
{
    QString romPath = QFileDialog::getOpenFileName(this, tr("Open ROM"), QString(), tr("SNES ROM Files (*.smc *.sfc);;All Files (*.*)"));
    if (!romPath.isEmpty())
    {
        OpenROM(romPath);
    }
}

void MainWindow::OpenROM(QString romPath)
{
    if (!QFile::exists(romPath))
    {
        QMessageBox errorMsg(QMessageBox::Warning, tr("Error"), tr("Specified ROM file does not exist"), QMessageBox::Ok);
        errorMsg.exec();
        return;
    }
    
    QFile romFile = QFile(romPath);
    romFile.open(QFile::ReadOnly);
    QByteArray raw = romFile.readAll();
    
    LoadedROMPath = romPath;
    
    // Check if rom is headered
    if (raw.size() % 0x10000 > 0)
    {
        // Header and data
        ROMHeader = raw.left(512);
        ROMData = raw.right(raw.size() - 512);
    }
    else
    {
        // Data only
        ROMHeader = QByteArray();
        ROMData = raw;
    }
    
    
    
    // Import all relevant data from ROM
    LoadTilesets();
    LoadRooms();
    
    
    // Enable editor buttons once a ROM is loaded
    ui->roomEditorButton->setEnabled(true);
    ui->oamSpriteCreatorButton->setEnabled(true);
}
