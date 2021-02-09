#ifndef ROM_CONSTANTS_H
#define ROM_CONSTANTS_H

#include "../rom_utils/bankedaddress.h"

#include <QByteArray>
#include <QByteArrayView>
#include <QDataStream>
#include <QIODevice>

// Tile Graphics
static const BankedAddress ROM_ADDRESS_COMMON_TILE_GRAPHICS = BankedAddress(0xB9, 0x08000);
static const BankedAddress ROM_ADDRESS_COMMON_TILE_TABLE = BankedAddress(0xB9, 0xA09D);
static const BankedAddress ROM_ADDRESS_TILESET_POINTER_LIST = BankedAddress(0x8F, 0xE6A2);
static constexpr uchar BYTES_PER_BITPLANE_PAIR = 16;
// Room Data
static constexpr uchar ROM_BANK_ROOM_DEFINITIONS = 0x8F;
static const BankedAddress ROM_ADDRESS_ROOM_DEFINITIONS = BankedAddress(ROM_BANK_ROOM_DEFINITIONS, 0x91F8); // TEMPORARY

#endif // ROM_CONSTANTS_H
