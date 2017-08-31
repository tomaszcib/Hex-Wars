LIBS += -lSDL -lSDL_mixer

QT += widgets

HEADERS       = \
    const.h \
    ui/strings.h \
    ui/dialog.h \
    struct.h \
    rand.h \
    ui/hexitem.h \
    mainwind.h \
    ui/sound.h \
    game/piecerules.h
SOURCES       = main.cpp \
    core.cpp \
    ui/graphics.cpp \
    game/move.cpp \
    ui/dock.cpp \
    ui/strings.cpp \
    ui/actions.cpp \
    ui/dialog.cpp \
    game/gameplay.cpp \
    rand.cpp \
    ui/hexitem.cpp \
    ui/mainwind.cpp \
    game/aimove.cpp \
    game/aiproduce.cpp \
    game/aitransport.cpp \
    game/highscores.cpp \
    ui/sound.cpp \
    game/generatemap.cpp \
    game/piecerules.cpp \
    ui/miscui.cpp
RESOURCES     = \
    hexwars.qrc

TARGET = HexWars

FORMS +=
DISTFILES +=

RC_ICONS = winicon.ico
VERSION = 1.0.0
QMAKE_TARGET_PRODUCT = "Hex Wars\0"
QMAKE_TARGET_DESCRIPTION = "Hex Wars\0"
QMAKE_TARGET_COMPANY = "Tomasz Ciborski\0"
QMAKE_TARGET_COPYRIGHT = "Tomasz Ciborski\0"


OTHER_FILES +=
