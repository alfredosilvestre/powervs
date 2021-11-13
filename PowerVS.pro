TEMPLATE = app

TARGET = "PowerVS"

CONFIG += qt debug_and_release

QT += opengl

# Add the headers to the include path
INCLUDEPATH += powervs-core/decklink_sdk/ powervs-core/include powervs-core/include/decklink powervs-core/include/player powervs-core/include/previewer/ powervs-core/include/recorder include include/channel include/recorder include/playlist

# Required for some C99 defines
DEFINES += __STDC_CONSTANT_MACROS

# Define features to use
DEFINES += DECKLINK GUI PORTAUDIO

# Add the headers to the include path
INCLUDEPATH += powervs-core/ffmpeg/include powervs-core/portaudio/include powervs-core/log4cxx/include

# Add the path to static libraries
LIBS += -Lpowervs-core/ffmpeg/lib/ -Lpowervs-core/portaudio/lib/

# Add the path to shared libraries
LIBS += -Lpowervs-core/ffmpeg/shared/ -Lpowervs-core/portaudio/shared/

# Log4CXX is a special cookie :)
CONFIG(debug, debug|release){
    LIBS += -Lpowervs-core/log4cxx/Debug/
} else {
    LIBS += -Lpowervs-core/log4cxx/lib/ -Lpowervs-core/log4cxx/shared/
}

# Set list of required libraries
LIBS += -lcomsuppw -lavcodec-57 -lavformat-57 -lavutil-55 -lswscale-4 -lswresample-2 -lavfilter-6 -lportaudio_x86 -llog4cxx -lopengl32 -lglu32

SOURCES += \
    powervs-core/decklink_sdk/DeckLinkAPI_i.c \
    powervs-core/src/decklink/decklinkinput.cpp \
    powervs-core/src/decklink/decklinkoutput.cpp \
    powervs-core/src/decklink/decklinkcontrol.cpp \
    powervs-core/src/player/audiosamplearray.cpp \
    powervs-core/src/player/avdecodedframe.cpp \
    powervs-core/src/player/ffdecoder.cpp \
    powervs-core/src/player/audiothread.cpp \
    powervs-core/src/player/videothread.cpp \
    powervs-core/src/player/player.cpp \
    powervs-core/src/previewer/previewerrgb.cpp \
    powervs-core/src/previewer/qvumeter.cpp \
    powervs-core/src/recorder/videoencoder.cpp \
    powervs-core/src/recorder/recorder.cpp \
    powervs-core/src/recorder/muxer.cpp \
    powervs-core/src/recorder/audioencoder.cpp \
    powervs-core/src/iobridge.cpp \
    src/channel/channelgui.cpp \
    src/recorder/recordergui.cpp \
    src/playlist/playlist.cpp \
    src/playlist/playlistgui.cpp \
    src/playlist/playlisttable.cpp \
    src/mainwindow.cpp \
    src/main.cpp

HEADERS += \
    powervs-core/decklink_sdk/DeckLinkAPI_h.h \
    powervs-core/include/decklink/decklinkinput.h \
    powervs-core/include/decklink/decklinkoutput.h \
    powervs-core/include/decklink/decklinkcontrol.h \
    powervs-core/include/player/audiosamplearray.h \
    powervs-core/include/player/avdecodedframe.h \
    powervs-core/include/player/ffdecoder.h \
    powervs-core/include/player/audiothread.h \
    powervs-core/include/player/videothread.h \
    powervs-core/include/player/player.h \
    powervs-core/include/previewer/previewerrgb.h \
    powervs-core/include/previewer/qvumeter.h \
    powervs-core/include/recorder/videoencoder.h \
    powervs-core/include/recorder/recorder.h \
    powervs-core/include/recorder/muxer.h \
    powervs-core/include/recorder/audioencoder.h \
    powervs-core/include/iobridge.h \
    include/channel/channelgui.h \
    include/recorder/recordergui.h \
    include/playlist/playlist.h \
    include/playlist/playlistgui.h \
    include/playlist/playlisttable.h \
    include/mainwindow.h

FORMS += \
    forms/mainwindow.ui \
    forms/config.ui \
    forms/playlistgui.ui \
    forms/channelgui.ui \
    forms/recordergui.ui

RESOURCES += \
    resources.qrc

RC_FILE += \
    proj.qrc
