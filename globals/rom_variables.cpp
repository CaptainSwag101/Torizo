#include "rom_variables.h"

QString LoadedROMPath;
QByteArray ROMHeader;
QByteArray ROMData;

QList<QByteArray> GlobalTileTables;
QList<QImage> GlobalTileGraphics;
QList<QList<ushort>> GlobalPalettes;

QMap<ushort, Room> GlobalRooms;
