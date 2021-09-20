#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "rom_utils/tileset.h"

#include <QMainWindow>
#include <QFile>
#include <QFileDialog>
#include <QList>
#include <QMessageBox>
#include <QWidget>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void OpenROM(QString romPath);
    
private slots:
    void on_roomEditorButton_clicked();
    void on_oamSpriteCreatorButton_clicked();
    void on_action_Open_ROM_triggered();
    
private:
    Ui::MainWindow *ui;
    QList<std::shared_ptr<QWidget>> activeEditors;
};

#endif // MAINWINDOW_H
