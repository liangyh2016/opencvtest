INCLUDEPATH += $$PWD
INCLUDEPATH += $$PWD/include

macos {
    LIBS += $$PWD/bin/macos/libopencv_core.4.5.5.dylib
    LIBS += $$PWD/bin/macos/libopencv_imgproc.4.5.5.dylib
    LIBS += $$PWD/bin/macos/libopencv_imgcodecs.4.5.5.dylib
}

HEADERS += \
    $$PWD/opencvtool.h

SOURCES += \
    $$PWD/opencvtool.cpp
