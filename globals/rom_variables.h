#ifndef ROM_VARIABLES_H
#define ROM_VARIABLES_H

#include "../rom_utils/room.h"

#include <QByteArray>
#include <QImage>
#include <QList>
#include <QMap>
#include <QString>

// General ROM data
extern QString LoadedROMPath;
extern QByteArray ROMHeader;
extern QByteArray ROMData;
// Graphics and tileset-related data
extern QList<QByteArray> GlobalTileTables;
extern QList<QImage> GlobalTileGraphics;
extern QList<QList<ushort>> GlobalPalettes;
// Level and room-related data
extern QMap<ushort, Room> GlobalRooms;

#endif // ROM_VARIABLES_H
