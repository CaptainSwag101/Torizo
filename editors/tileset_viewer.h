#ifndef TILESET_VIEWER_H
#define TILESET_VIEWER_H

#include <QMainWindow>

namespace Ui {
class TilesetViewer;
}

class TilesetViewer : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit TilesetViewer(QWidget *parent = nullptr);
    ~TilesetViewer();
    
private:
    Ui::TilesetViewer *ui;
};

#endif // TILESET_VIEWER_H
