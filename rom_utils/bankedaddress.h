#ifndef BANKEDADDRESS_H
#define BANKEDADDRESS_H

#include <cstdint>

class BankedAddress
{
public:
    BankedAddress();
    BankedAddress(uint_fast8_t bank, uint_fast16_t offset);
    uint_fast32_t ToPointer();
    static BankedAddress FromPointer(uint_fast32_t pointer);

    uint_fast16_t Offset;
    uint_fast8_t Bank;
};

#endif // BANKEDADDRESS_H
