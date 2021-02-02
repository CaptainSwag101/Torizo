#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "editors/room_editor.h"

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
    for (std::shared_ptr<QWidget> &editorPtr : activeEditors)
    {
        if (editorPtr.get()->windowTitle() == tr("Room Editor"))
            return;
    }
    
    std::shared_ptr<RoomEditor> editorPtr = std::make_shared<RoomEditor>(this);
    activeEditors.append(editorPtr);
    editorPtr.get()->show();
}
