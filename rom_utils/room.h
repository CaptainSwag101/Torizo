#ifndef ROOM_H
#define ROOM_H

#include "bankedaddress.h"

#include <QDataStream>
#include <QList>

struct DoorData
{
    ushort TargetRoomPointer;      // $8F pointer
    uchar DoorBitFlag;  // elevator properties, change region
    uchar Direction;
    uchar Xi, Yi;
    uchar X, Y;
    ushort Distance;
    ushort ScrollData;  // may also be used as custom ASM, run once when room is loaded
};

struct PLM
{
    ushort Type;
    uchar X, Y;
    uchar I; // index?
    uchar Unknown;
};

struct Block
{
    ushort BlockNum : 10;
    bool XFlip;
    bool YFlip;
    char BlockType : 4;
};

struct LevelData
{
    ushort TileLayer1Size;
    QList<Block> TileLayer1;
    QByteArray BtsData;
    QList<Block> TileLayer2;
};

struct RoomHeader
{
    uchar RoomId;
    uchar Region;
    uchar X, Y;
    uchar Width, Height;
    uchar UpScroller, DownScroller;
    uchar Unknown;
    ushort DoorOut; // $8F pointer to a list of $83 pointers
};

struct RoomStateHeader
{
    ushort StateCode;
    QList<uchar> StateParams;
};

struct RoomState
{
    RoomStateHeader Header;
    BankedAddress LevelDataAddress;
    uchar GraphicSet;
    uchar MusicTrack;
    uchar MusicControl;
    ushort FX1;                 // pointer to room_fx1 ($83)
    ushort EnemyPopulation;     // pointer to enemy_pop ($A1)
    ushort EnemySet;            // pointer to room_set ($B4)
    ushort ScrollDataLayer2;    // layer 2 scroll data
    ushort ScrollData;          // pointer to mdb_scroll ($8F)
    ushort Unknown;             // used in escape version of Bomb Torizo's room **WTF** // xray casing code??? ($8F)
    ushort FX2;                 // pointer to room_fx2 ($8F)
    ushort PLM;                 // pointer to PLM data ($8F)
    ushort BGData;              // pointer to bg_data ($8F)
    ushort Layer1_2Handling;    // pointer to layer 1 and 2 handling code ($8F)
};

struct Room
{
    RoomHeader Header;
    QList<RoomState> States;
    //QList<DoorData> Doors;
};

void LoadRooms();
void ScanConnectedRoomsRecursive(Room room);
Room ReadRoomDefinition(BankedAddress roomAddress);
LevelData ReadLevel(Room room, int stateIndex);
QList<DoorData> ReadDoorData(Room room, int bankForRoomDefinitions);
BankedAddress FindFirstRoomDefinition(int bank);

#endif // ROOM_H
