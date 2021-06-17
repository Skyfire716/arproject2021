QT       += core gui multimedia multimediawidgets 3dcore 3drender 3dinput 3dlogic 3dextras 3danimation

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11 resources_big


# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    archesswidget.cpp \
    camera_controller.cpp \
    main.cpp \
    mainwindow.cpp

HEADERS += \
    archesswidget.h \
    camera_controller.h \
    mainwindow.h

FORMS += \
    mainwindow.ui

contains(ANDROID_TARGET_ARCH,armeabi-v7a) {
    QT += androidextras
    ANDROID_EXTRA_LIBS = \
        $$PWD/3rdparty/install/lib/armeabi-v7a/libopencv_core.so \
        $$PWD/3rdparty/install/lib/armeabi-v7a/libopencv_imgproc.so \
        $$PWD/3rdparty/install/lib/armeabi-v7a/libopencv_dnn.so \
        $$PWD/3rdparty/install/lib/armeabi-v7a/libopencv_videoio.so \
        $$PWD/3rdparty/install/lib/armeabi-v7a/libtbb.so \
        $$PWD/3rdparty/install/lib/armeabi-v7a/libopencv_imgcodecs.so
    INCLUDEPATH = $$PWD/3rdparty/install/sdk/native/jni/include
    # Order might be important it seems, linker in older droids (4.2) are dumb
    LIBS += -L$$PWD/3rdparty/install/lib/armeabi-v7a/ -ltbb -lopencv_core -lopencv_imgproc -lopencv_dnn -lopencv_videoio -lopencv_imgcodecs

    ANDROID_PACKAGE_SOURCE_DIR = \
        $$PWD/org.arproject.archess
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

DISTFILES += \
    org.arproject.archess/AndroidManifest.xml \
    org.arproject.archess/build.gradle \
    org.arproject.archess/gradle/wrapper/gradle-wrapper.jar \
    org.arproject.archess/gradle/wrapper/gradle-wrapper.properties \
    org.arproject.archess/gradlew \
    org.arproject.archess/gradlew.bat \
    org.arproject.archess/res/values/libs.xml

RESOURCES += \
    qtresourcemanager.qrc


