QT       += core gui multimedia multimediawidgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11


# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    camera_controller.cpp \
    main.cpp \
    mainwindow.cpp

HEADERS += \
    camera_controller.h \
    mainwindow.h

FORMS += \
    mainwindow.ui
armeabi-v7a | arm64-v8a{

}

linux-g++ | linux-g++-64 | linux-g++-32 {
    INCLUDEPATH += -I/usr/local/include/opencv2
    LIBS += -L/usr/local/lib \
            -lopencv_calib3d \
            -lopencv_core \
            -lopencv_dnn \
            -lopencv_features2d \
            -lopencv_flann \
            -lopencv_highgui \
            -lopencv_imgcodecs \
            -lopencv_imgproc \
            -lopencv_ml \
            -lopencv_objdetect \
            -lopencv_photo \
            -lopencv_shape \
            -lopencv_stitching \
            -lopencv_superres \
            -lopencv_videoio \
            -lopencv_video \
            -lopencv_videostab \
            -lopencv_viz

}

win32 {
 # TODO add Windows opencv includes
}


# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
