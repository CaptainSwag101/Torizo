#ifndef COMPRESSION_H
#define COMPRESSION_H

#include "../globals/rom_constants.h"

#include <QDataStream>

QByteArray DecompressData(QByteArray source, int maxDataSize = 0x10000);
QByteArray CompressData(QByteArray source);

#endif // COMPRESSION_H
