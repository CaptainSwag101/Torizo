#include "editors/room_editor.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    RoomEditor w;
    w.show();
    return a.exec();
}
