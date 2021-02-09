#ifndef ROM_VARIABLES_H
#define ROM_VARIABLES_H

#include "../rom_utils/room.h"

#include <QByteArray>
#include <QImage>
#include <QList>
#include <QMap>
#include <QString>

extern QString LoadedROMPath;
extern QByteArray ROMHeader;
extern QByteArray ROMData;

extern QList<QByteArray> GlobalTileTables;
extern QList<QImage> GlobalTileGraphics;
extern QList<QList<ushort>> GlobalPalettes;

extern QMap<ushort, Room> GlobalRooms;

#endif // ROM_VARIABLES_H
