#ifndef BANKEDADDRESS_H
#define BANKEDADDRESS_H

#include <QtGlobal>

class BankedAddress
{
public:
    BankedAddress();
    BankedAddress(uchar bank, ushort offset);
    BankedAddress(uint pointer);
    uint ToPointer() const;

    ushort Offset;
    uchar Bank;
};

#endif // BANKEDADDRESS_H
