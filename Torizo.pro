QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    editors/oam_sprite_editor/oam_sprite_creator.cpp \
    editors/room_editor/room_editor.cpp \
    globals/rom_variables.cpp \
    main.cpp \
    mainwindow.cpp \
    rom_utils/bankedaddress.cpp \
    rom_utils/compression.cpp \
    rom_utils/room.cpp \
    rom_utils/tileset.cpp

HEADERS += \
    editors/oam_sprite_editor/oam_sprite.h \
    editors/oam_sprite_editor/oam_sprite_creator.h \
    editors/room_editor/room_editor.h \
    globals/graphics_constants.h \
    globals/rom_constants.h \
    globals/rom_variables.h \
    mainwindow.h \
    rom_utils/bankedaddress.h \
    rom_utils/compression.h \
    rom_utils/room.h \
    rom_utils/tileset.h

FORMS += \
    editors/oam_sprite_editor/oam_sprite_creator.ui \
    editors/room_editor/room_editor.ui \
    mainwindow.ui

TRANSLATIONS += \
    Torizo_en_US.ts
CONFIG += lrelease
CONFIG += embed_translations

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
