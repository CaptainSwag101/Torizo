#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "../rom_utils/tileset.h"

#include <QMainWindow>
#include <QFile>
#include <QFileDialog>
#include <QHash>
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
    QHash<QPushButton*, std::shared_ptr<QWidget>> activeEditors;  // Button clicked paired with form
};

#endif // MAINWINDOW_H
