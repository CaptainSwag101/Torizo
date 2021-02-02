#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QList>
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
    
private slots:
    void on_roomEditorButton_clicked();
    
private:
    Ui::MainWindow *ui;
    QList<std::shared_ptr<QWidget>> activeEditors;
};

#endif // MAINWINDOW_H
