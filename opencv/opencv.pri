INCLUDEPATH += $$PWD
INCLUDEPATH += $$PWD/include

win32 {
    LIBS += -L$$PWD/bin/win32 -lopencv_core455
    LIBS += -L$$PWD/bin/win32 -lopencv_imgproc455
    LIBS += -L$$PWD/bin/win32 -lopencv_imgcodecs455
}

macos {
    LIBS += $$PWD/bin/macos/libopencv_core.4.5.5.dylib
    LIBS += $$PWD/bin/macos/libopencv_imgproc.4.5.5.dylib
    LIBS += $$PWD/bin/macos/libopencv_imgcodecs.4.5.5.dylib
}

HEADERS += \
    $$PWD/opencvtool.h

SOURCES += \
    $$PWD/opencvtool.cpp

