#include "bankedaddress.h"

BankedAddress::BankedAddress()
{
    Offset = 0;
    Bank = 0;
}

BankedAddress::BankedAddress(uchar bank, ushort offset)
{
    Offset = offset;
    Bank = bank;
}

BankedAddress::BankedAddress(uint pointer)
{
    Bank = (pointer / 0x8000);
    if (Bank < 0x80)
        Bank += 0x80;

    Offset = (ushort)(pointer - (Bank * 0x8000));
    if (Offset < 0x8000)
        Offset += 0x8000;
}

uint BankedAddress::ToPointer() const
{
    uint result = ((Bank % 0x80) * 0x8000) + (Offset % 0x8000);
    return result;
}
