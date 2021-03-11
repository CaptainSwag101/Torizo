#include "tileset_viewer.h"
#include "ui_tileset_viewer.h"

TilesetViewer::TilesetViewer(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::TilesetViewer)
{
    ui->setupUi(this);
}

TilesetViewer::~TilesetViewer()
{
    delete ui;
}
