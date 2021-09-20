#ifndef OAMSPRITE_H
#define OAMSPRITE_H

#include <QtGlobal>

struct OAMSprite
{
    short XPos : 9;
    short YPos : 8;
    uchar TileId;
    bool YFlip : 1;
    bool XFlip : 1;
    uchar Priority : 2;
    uchar PaletteId : 3;
    bool LargeSize : 1;
};

#endif // OAMSPRITE_H
