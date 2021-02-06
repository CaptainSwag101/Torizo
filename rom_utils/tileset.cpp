#include "tileset.h"

QList<QByteArray> GlobalTileTables;
QList<QImage> GlobalTileGraphics;
QList<QList<ushort>> GlobalPalettes;

void LoadTilesets()
{
    // Reinitialize all tileset data
    GlobalTileTables = QList<QByteArray>();
    GlobalTileGraphics = QList<QImage>();
    GlobalPalettes = QList<QList<ushort>>();
    
    // Load global graphics sets
    for (int i = 0; i < 29; ++i)
    {
        QByteArray tileTable;
        QImage tileGraphics;
        QList<ushort> palette;
        ReadTilesetData(i, tileTable, tileGraphics, palette);
        GlobalTileTables.append(tileTable);
        GlobalTileGraphics.append(tileGraphics);
        GlobalPalettes.append(palette);

        // DEBUGGING ONLY!!!
        /*
        QFile debugFileOut;
        debugFileOut.setFileName("graphicSet" + QString::number(i) + "_TileTable_CppDebug.bin");
        debugFileOut.open(QFile::ReadWrite);
        debugFileOut.write(tileTable);
        debugFileOut.flush();
        debugFileOut.close();

        debugFileOut.setFileName("graphicSet" + QString::number(i) + "_TileBitplaneGraphics_CppDebug.bin");
        debugFileOut.open(QFile::ReadWrite);
        debugFileOut.write(tileGraphics);
        debugFileOut.flush();
        debugFileOut.close();

        debugFileOut.setFileName("graphicSet" + QString::number(i) + "_Palette_CppDebug.txt");
        debugFileOut.open(QFile::ReadWrite);
        for (ushort color : palette)
        {
            debugFileOut.write((QString::number(color, 16).rightJustified(4, '0') + '\n').toUpper().toUtf8());
            debugFileOut.flush();
        }
        debugFileOut.close();
        */
    }
}

void ReadTilesetPointers(int graphicSetIndex, BankedAddress &tTableAddress, BankedAddress &tGraphicsAddress, BankedAddress &paletteAddress)
{
    uint tilesetAddress = ROM_ADDRESS_TILESET_POINTER_LIST.ToPointer() + (graphicSetIndex * 9);

    QDataStream stream(ROMData.mid(tilesetAddress, 9));
    stream.setByteOrder(QDataStream::LittleEndian);
    stream >> tTableAddress.Offset;
    stream >> tTableAddress.Bank;
    stream >> tGraphicsAddress.Offset;
    stream >> tGraphicsAddress.Bank;
    stream >> paletteAddress.Offset;
    stream >> paletteAddress.Bank;
}

void ReadTilesetData(int graphicSetIndex, QByteArray &tileTable, QImage &tileGraphics, QList<ushort> &palette)
{
    // First, read tileset pointers
    BankedAddress tileTableAddress, tileGraphicsAddress, paletteAddress;
    ReadTilesetPointers(graphicSetIndex, tileTableAddress, tileGraphicsAddress, paletteAddress);

    // Read actual data from pointers
    QByteArray compressedCommonTileTable = ROMData.mid(ROM_ADDRESS_COMMON_TILE_TABLE.ToPointer(), 0x1000);
    QByteArray compressedUniqueTileTable = ROMData.mid(tileTableAddress.ToPointer(), 0x1000);
    tileTable.append(DecompressData(compressedCommonTileTable));
    tileTable.append(DecompressData(compressedUniqueTileTable));

    QByteArray compressedCommonTileGraphics = ROMData.mid(ROM_ADDRESS_COMMON_TILE_GRAPHICS.ToPointer(), 0x8000);
    QByteArray compressedUniqueTileGraphics = ROMData.mid(tileGraphicsAddress.ToPointer(), 0x8000);
    QByteArray combinedTileGraphics = QByteArray();
    combinedTileGraphics.append(DecompressData(compressedUniqueTileGraphics));
    combinedTileGraphics.append(QByteArray(0x5000 - combinedTileGraphics.size(), 0));   // pad tile graphics with zeroes up to 0x5000
    combinedTileGraphics.append(DecompressData(compressedCommonTileGraphics));
    tileGraphics = DecodeBitplaneGraphics(combinedTileGraphics, 4);

    QByteArray compressedPalette = ROMData.mid(paletteAddress.ToPointer(), 0x800);
    QByteArray paletteData = DecompressData(compressedPalette);

    // Convert palette data from bytes to ushorts
    for (int i = 0; i < paletteData.size(); i += 2)
    {
        uchar b1 = paletteData[i];
        uchar b2 = paletteData[i + 1];
        ushort color = (b2 << 8) | b1;
        palette.append(color);
    }
}

QImage DecodeBitplaneGraphics(QByteArray bitplaneGraphics, int bitsPerPixel)
{
    int bitplanePairCount = bitsPerPixel / 2;
    int tileCount = bitplaneGraphics.size() / (BYTES_PER_BITPLANE_PAIR * bitplanePairCount);

    QImage output(PIXELS_PER_TILE, PIXELS_PER_TILE * tileCount, QImage::Format_Indexed8);

    for (int tileNum = 0; tileNum < tileCount; ++tileNum)
    {
        QImage tilePixData(PIXELS_PER_TILE, PIXELS_PER_TILE, QImage::Format_Indexed8);

        // Zero out pixel data first, it's not actually initialized to zero beforehand
        for (int i = 0; i < tilePixData.sizeInBytes(); ++i)
        {
            tilePixData.bits()[i] = 0;
        }

        // Decode bitplane graphics data
        int tileDataOffset = (tileNum * (BYTES_PER_BITPLANE_PAIR * bitplanePairCount));
        for (int bpPair = 0; bpPair < bitplanePairCount; ++bpPair)
        {
            for (int y = 0; y < PIXELS_PER_TILE; ++y)
            {
                int rowOffset = (bpPair * BYTES_PER_BITPLANE_PAIR) + (y * 2);
                uchar *tileScanline = tilePixData.scanLine(y);

                // Process bitplane 0
                for (int x = 0; x < PIXELS_PER_TILE; ++x)
                {
                    uchar bit = ((bitplaneGraphics[tileDataOffset + rowOffset]) >> x);
                    bit &= 1;   // mask only the bit we need
                    tileScanline[x] = tileScanline[x] | (uchar)(bit << (bpPair * 2));
                }

                // Process bitplane 1
                for (int x = 0; x < PIXELS_PER_TILE; ++x)
                {
                    uchar bit = ((bitplaneGraphics[tileDataOffset + rowOffset + 1]) >> x);
                    bit &= 1;   // mask only the bit we need
                    tileScanline[x] = tileScanline[x] | (uchar)(bit << ((bpPair * 2) + 1));
                }
            }
        }

        // Flip horizontally to be correct
        tilePixData = tilePixData.mirrored(true, false);

        // DEBUGGING ONLY: get array of raw bit data
        //QByteArray temp = QByteArray::fromRawData((const char *)tilePixData.bits(), tilePixData.sizeInBytes());

        // Copy tile pixel data to output pixel data
        for (int y = 0; y < PIXELS_PER_TILE; ++y)
        {
            const uchar *tileScanline = tilePixData.constScanLine(y);
            uchar *outputScanline = output.scanLine((tileNum * PIXELS_PER_TILE) + y);

            // Process bitplane 0
            for (int x = 0; x < PIXELS_PER_TILE; ++x)
            {
                outputScanline[x] = tileScanline[x];
            }
        }
    }

    return output;
}

uint SnesToPcColor(ushort snesColor)
{
    uint pcColor = 0xFF000000 | (uint)(((snesColor & 0x1F) << 0x13) | ((snesColor & 0x3E0) << 6) | ((snesColor >> 7) & 0xF8));
    return pcColor;
}

QList<ushort> GetTileTableEntry(int blockId, QByteArray tileTable)
{
    QList<ushort> entry;

    for (int i = 0; i < 4; ++i)
    {
        int offset = ((blockId & 0x3FF) * 8) + (i * 2);
        if (offset >= tileTable.size() || offset + 1 >= tileTable.size())
            return entry;

        uchar b1 = tileTable[offset];
        uchar b2 = tileTable[offset + 1];
        ushort value = (b2 << 8) | b1;
        entry.append(value);
    }

    return entry;
}

QImage GetImageForTile(int tileNum, QImage tileGraphics)
{
    return tileGraphics.copy(0, PIXELS_PER_TILE * tileNum, PIXELS_PER_TILE, PIXELS_PER_TILE);
}

QImage GetImageForBlock(int blockNum, QByteArray tileTable, QImage tileGraphics)
{
    QImage blockImage(PIXELS_PER_TILE * 2, PIXELS_PER_TILE * 2, QImage::Format_Indexed8);

    QList<ushort> entry = GetTileTableEntry(blockNum, tileTable);
    if (entry.size() < 4)
        return blockImage;

    for (int e = 0; e < 4; ++e)
    {
        ushort tileData = entry[e];
        ushort tileNum = (ushort)(tileData & 0x3FF);
        bool xFlip = ((tileData & 0b0100000000000000) >> 14) == 1;
        bool yFlip = ((tileData & 0b1000000000000000) >> 15) == 1;
        uchar paletteBank = (uchar)((tileData & 0b0001110000000000) >> 10);

        QImage tileImage = tileGraphics.copy(0, PIXELS_PER_TILE * tileNum, PIXELS_PER_TILE, PIXELS_PER_TILE);
        tileImage = tileImage.mirrored(xFlip, yFlip);

        for (int y = 0; y < PIXELS_PER_TILE; ++y)
        {
            const uchar *tileScanline = tileImage.constScanLine(y);
            uchar *blockScanline = blockImage.scanLine(((e / 2) * PIXELS_PER_TILE) + y);

            for (int x = 0; x < PIXELS_PER_TILE; ++x)
            {
                blockScanline[((e % 2) * PIXELS_PER_TILE) + x] = tileScanline[x] + (0x10 * paletteBank);
            }
        }
    }

    return blockImage;
}
