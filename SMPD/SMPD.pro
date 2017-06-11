#-------------------------------------------------
#
# Project created by QtCreator 2017-02-14T15:53:25
#
#-------------------------------------------------

QT       += core gui
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
CONFIG += gnu++11
TARGET = SMPD
TEMPLATE = app

INCLUDEPATH += $$MY_EXTERNAL_INCLUDES \
                /usr/local/include/opencv
LIBS+=/usr/local/lib/libopencv_calib3d.so
#LIBS+=/usr/local/lib/libopencv_contrib.so
LIBS+=/usr/local/lib/libopencv_core.so
LIBS+=/usr/local/lib/libopencv_features2d.so
LIBS+=/usr/local/lib/libopencv_highgui.so
LIBS+=/usr/local/lib/libopencv_imgproc.so
LIBS+=/usr/local/lib/libopencv_flann.so
LIBS+=/usr/local/lib/libopencv_imgcodecs.so
LIBS+=/usr/local/lib/libopencv_ml.so
LIBS+=/usr/local/lib/libopencv_objdetect.so
LIBS+=/usr/local/lib/libopencv_photo.so
LIBS+=/usr/local/lib/libopencv_shape.so
LIBS+=/usr/local/lib/libopencv_stitching.so
LIBS+=/usr/local/lib/libopencv_superres.so
LIBS+=/usr/local/lib/libopencv_videoio.so
LIBS+=/usr/local/lib/libopencv_video.so
LIBS+=/usr/local/lib/libopencv_videostab.so
LIBS+=/usr/local/lib/libopencv_viz.so
# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += main.cpp\
        mainwindow.cpp \
    database.cpp \
    object.cpp

HEADERS  += mainwindow.h \
    database.h \
    object.h \
    matrixutil.hpp

FORMS    += mainwindow.ui
