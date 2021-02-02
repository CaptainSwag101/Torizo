#ifndef COMPRESSION_H
#define COMPRESSION_H

#include "../globals/rom_constants.h"

#include <QDataStream>

enum CompressionMethod
{
    DirectCopy,
    ByteFill,
    WordFill,
    SigmaFill,
    LibraryCopy,
    XorCopy,
    MinusCopy
};

QByteArray DecompressData(QByteArray source, int maxDataSize = 0x10000);
QByteArray CompressData(QByteArray source);

#endif // COMPRESSION_H
