#include "bankedaddress.h"

BankedAddress::BankedAddress()
{
    Offset = 0;
    Bank = 0;
}

BankedAddress::BankedAddress(uint_fast8_t bank, uint_fast16_t offset)
{
    Offset = offset;
    Bank = bank;
}

uint_fast32_t BankedAddress::ToPointer()
{
    uint_fast32_t result = ((Bank % 0x80) * 0x8000) + (Offset % 0x8000);
    return result;
}

BankedAddress BankedAddress::FromPointer(uint_fast32_t pointer)
{
    uint_fast8_t bank = (pointer / 0x8000);
    if (bank < 0x80)
        bank += 0x80;

    uint_fast16_t offset = (uint_fast16_t)(pointer - (bank * 0x8000));
    if (offset < 0x8000)
        offset += 0x8000;

    return BankedAddress(bank, offset);
}
