cmake_minimum_required(VERSION 3.5)

project(Torizo LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# QtCreator supports the following variables for Android, which are identical to qmake Android variables.
# Check https://doc.qt.io/qt/deployment-android.html for more information.
# They need to be set before the find_package( ...) calls below.

#if(ANDROID)
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
#    if (ANDROID_ABI STREQUAL "armeabi-v7a")
#        set(ANDROID_EXTRA_LIBS
#            ${CMAKE_CURRENT_SOURCE_DIR}/path/to/libcrypto.so
#            ${CMAKE_CURRENT_SOURCE_DIR}/path/to/libssl.so)
#    endif()
#endif()

find_package(QT NAMES Qt6 Qt5 COMPONENTS Widgets LinguistTools REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets LinguistTools REQUIRED)

set(TS_FILES Torizo_en_US.ts)

set(PROJECT_SOURCES
        # Root project files
        main.cpp
        mainwindow.cpp
        mainwindow.h
        mainwindow.ui
        
        # Editors
        editors/room_editor.cpp
        editors/room_editor.h
        editors/room_editor.ui
        
        # Globals
        globals/graphics_constants.h
        globals/rom_constants.h
        globals/rom_variables.cpp
        globals/rom_variables.h
        
        # Utilities for interacting with ROM
        rom_utils/bankedaddress.cpp
        rom_utils/bankedaddress.h
        rom_utils/compression.cpp
        rom_utils/compression.h
        
        # Misc.
        ${TS_FILES}
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(Torizo
        ${PROJECT_SOURCES}
    )

    qt_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})
else()
    if(ANDROID)
        add_library(Torizo SHARED
            ${PROJECT_SOURCES}
        )
    else()
        add_executable(Torizo
            ${PROJECT_SOURCES}
        )
    endif()

    qt5_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})
endif()

target_link_libraries(Torizo PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)
