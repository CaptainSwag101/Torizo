#include "room.h"
#include "compression.h"
#include "../globals/rom_constants.h"
#include "../globals/rom_variables.h"

void LoadRooms()
{
    // Scan for first room definition
    BankedAddress firstRoomDefinitionAddress = FindFirstRoomDefinition(ROM_BANK_ROOM_DEFINITIONS);
    Room firstRoom = ReadRoomDefinition(firstRoomDefinitionAddress);
    
    GlobalRooms.clear();
    GlobalRooms.insert(firstRoomDefinitionAddress.Offset, firstRoom);
    
    ScanConnectedRoomsRecursive(firstRoom);
}

void ScanConnectedRoomsRecursive(Room room)
{
    QList<DoorData> doorList = ReadDoorData(room, ROM_BANK_ROOM_DEFINITIONS);

    for (DoorData &door : doorList)
    {
        BankedAddress targetRoomAddress(ROM_BANK_ROOM_DEFINITIONS, door.TargetRoomPointer);
        Room targetRoom = ReadRoomDefinition(targetRoomAddress);

        // Sanity check: if a room has a blank event list it means we
        // failed the sanity check when reading its events,
        // and it is likely not real room data.
        if (targetRoom.States.size() <= 0)
        {
            continue;
        }
        
        // If we already have this room accounted for, don't add it
        if (GlobalRooms.keys().contains(targetRoomAddress.Offset))
        {
            continue;
        }
        
        // Add the room to our map, then scan it too
        GlobalRooms.insert(targetRoomAddress.Offset, targetRoom);
        ScanConnectedRoomsRecursive(targetRoom);
    }
}

Room ReadRoomDefinition(BankedAddress roomAddress)
{
    Room result;

    // Read room data from ROM
    QDataStream stream(ROMData);
    stream.setByteOrder(QDataStream::LittleEndian);
    stream.device()->seek(roomAddress.ToPointer());
    stream >> result.Header.RoomId;
    stream >> result.Header.Region;
    stream >> result.Header.X;
    stream >> result.Header.Y;
    stream >> result.Header.Width;
    stream >> result.Header.Height;
    stream >> result.Header.UpScroller;
    stream >> result.Header.DownScroller;
    stream >> result.Header.Unknown;
    stream >> result.Header.DoorOut;

    // Read room state headers
    QVector<RoomStateHeader> stateHeaders;
    while (true)
    {
        RoomStateHeader header;
        stream >> header.StateCode;

        // I think the "Standard" state code should always be the last one in the list?
        if (header.StateCode == 0xE5E6)
        {
            stateHeaders.append((header));
            break;
        }

        // Read extra state parameter(?) bytes
        switch (header.StateCode)
        {
        case 0xE5EB:
            {
                for (int i = 0; i < 4; ++i)
                {
                    uchar b;
                    stream >> b;
                    header.StateParams.append(b);
                }
            }
            break;

        case 0xE612:
        case 0xE629:
            {
                for (int i = 0; i < 3; ++i)
                {
                    uchar b;
                    stream >> b;
                    header.StateParams.append(b);
                }
            }
            break;

        case 0xE5FF:
        case 0xE640:
        case 0xE652:
        case 0xE669:
        case 0xE678:
            {
                for (int i = 0; i < 2; ++i)
                {
                    uchar b;
                    stream >> b;
                    header.StateParams.append(b);
                }
            }
            break;

        default:    // For sanity checking, if a room has a state code that isn't in this list, it's assumed to be invalid data
            {
                //Room blank;
                //return blank;
            }
            break;
        }

        stateHeaders.append(header);
    }

    // Read main state data
    for (RoomStateHeader &header : stateHeaders)
    {
        RoomState state;
        state.Header = header;

        stream >> state.LevelDataAddress.Offset;
        stream >> state.LevelDataAddress.Bank;
        stream >> state.GraphicSet;
        stream >> state.MusicTrack;
        stream >> state.MusicControl;
        stream >> state.FX1;
        stream >> state.EnemyPopulation;
        stream >> state.EnemySet;
        stream >> state.ScrollDataLayer2;
        stream >> state.ScrollData;
        stream >> state.Unknown;
        stream >> state.FX2;
        stream >> state.PLM;
        stream >> state.BGData;
        stream >> state.Layer1_2Handling;

        result.States.append(state);
    }

    return result;
}

LevelData ReadLevel(Room room, int stateIndex)
{
    RoomState state = room.States[stateIndex];
    uint levelDataPointer = state.LevelDataAddress.ToPointer();

    // Decompress level data
    QByteArray compressedLevelData = ROMData.mid(levelDataPointer, 0xFFFF);
    QByteArray decompressedLevelData = DecompressData(compressedLevelData);
    QDataStream levelStream(decompressedLevelData);
    levelStream.setByteOrder(QDataStream::LittleEndian);

    LevelData result;
    levelStream >> result.Header;

    int tileCount = result.Header / 2;
    int bytesRead = 0;

    // Read Layer 1 block data
    for (int i = 0; i < tileCount; ++i)
    {
        Block b;
        levelStream >> b.BlockId;
        levelStream >> b.PatternByte;
        result.TileLayer1.append(b);
        bytesRead += 2;
    }

    // If we haven't reached the end of level data, read BTS data
    if (bytesRead + tileCount <= levelStream.device()->size())
    {
        for (int i = 0; i < tileCount; ++i)
        {
            uchar bts;
            levelStream >> bts;
            result.BtsData.append(bts);
            ++bytesRead;
        }
    }

    // If we haven't reached the end of level data, read Layer 2 block data
    long pos = levelStream.device()->pos();
    if (bytesRead + (tileCount * 2) <= levelStream.device()->size())
    {
        for (int i = 0; i < tileCount; ++i)
        {
            Block b;
            levelStream >> b.BlockId;
            levelStream >> b.PatternByte;
            result.TileLayer2.append(b);
            bytesRead += 2;
        }
    }

    return result;
}

QList<DoorData> ReadDoorData(Room room, int bankForRoomDefinitions)
{
    QList<DoorData> doorDataList;

    BankedAddress doorListBankedAddress((uchar)bankForRoomDefinitions, room.Header.DoorOut);

    QDataStream stream(ROMData);
    stream.setByteOrder(QDataStream::LittleEndian);

    uint currentDoorListAddress = doorListBankedAddress.ToPointer();

    while (true)
    {
        stream.device()->seek(currentDoorListAddress);
        ushort doorDataPtr;
        stream >> doorDataPtr;

        // 0000 - 7FFF aren't pointers, means we've hit the end of the list
        if (doorDataPtr < 0x8000)
            break;

        BankedAddress doorDataBankedAddress(0x83, doorDataPtr);
        stream.device()->seek(doorDataBankedAddress.ToPointer());

        DoorData data;
        stream >> data.TargetRoomPointer;

        if (data.TargetRoomPointer == 0x0000)
            break;

        stream >> data.DoorBitFlag;
        stream >> data.Direction;
        stream >> data.Xi;
        stream >> data.Yi;
        stream >> data.X;
        stream >> data.Y;
        stream >> data.Distance;
        stream >> data.ScrollData;

        doorDataList.append(data);

        currentDoorListAddress += 2;
    }

    return doorDataList;
}

BankedAddress FindFirstRoomDefinition(int bank)
{
    return ROM_ADDRESS_ROOM_DEFINITIONS; // TEMPORARY
}
