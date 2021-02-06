#ifndef TILESET_H
#define TILESET_H

#include "bankedaddress.h"
#include "compression.h"
#include "../globals/graphics_constants.h"
#include "../globals/rom_constants.h"
#include "../globals/rom_variables.h"

#include <QtGlobal>
#include <QImage>

extern QList<QByteArray> GlobalTileTables;
extern QList<QImage> GlobalTileGraphics;
extern QList<QList<ushort>> GlobalPalettes;

void LoadTilesets();

QImage DecodeBitplaneGraphics(QByteArray bitplaneGraphics, int bitsPerPixel);

uint SnesToPcColor(ushort snesColor);

QList<ushort> GetTileTableEntry(int blockId, QByteArray tileTable);
QImage GetImageForTile(int tileNum, QImage tileGraphics);
QImage GetImageForBlock(int blockNum, QByteArray tileTable, QImage tileGraphics);

void ReadTilesetPointers(int graphicSetIndex, BankedAddress &tTableAddress, BankedAddress &tGraphicsAddress, BankedAddress &paletteAddress);
void ReadTilesetData(int graphicSetIndex, QByteArray &tileTable, QImage &tileGraphics, QList<ushort> &palette);

#endif // TILESET_H
