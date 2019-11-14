QT += charts quick
QT += serialport sql xml printsupport
CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Refer to the documentation for the
# deprecated API to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

CONFIG += "lang-zh_CN"
# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \        
    module/actionparser.cpp \
    module/exglobal.cpp \
    module/imageanalysis.cpp \
    module/imagecapture.cpp \
    module/imageprovider.cpp \
    module/log.cpp \
    module/sequence.cpp \
    module/serialmgr.cpp \
    module/sqlitemgr.cpp \
    module/v4l.cpp \
    main.cpp \   
    module/dao/usermgr.cpp \
    module/qrcoder.cpp \
    module/dao/testmgr.cpp \
    module/dao/testmodel.cpp \
    module/dao/testresultmodel.cpp \
    globalapplication.cpp

RESOURCES += qml.qrc

include(QZXing/QZXing.pri)

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Additional import path used to resolve QML modules just for Qt Quick Designer
QML_DESIGNER_IMPORT_PATH =

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

HEADERS += \
    module/actionparser.h \
    module/exglobal.h \
    module/imageanalysis.h \
    module/imagecapture.h \
    module/imageprovider.h \
    module/log.h \
    module/sequence.h \
    module/serialmgr.h \
    module/sqlitemgr.h \
    module/v4l.h \
    module/entity/User.h \
    module/dao/usermgr.h \
    module/qrcoder.h \
    module/entity.h \
    module/dao/testmgr.h \
    module/dao/testmodel.h \
    module/entity/assayitem.h \
    module/entity/testresult.h \
    module/dao/testresultmodel.h \
    globalapplication.h

if(contains(DEFINES,PLUTFORM_ARM)){
INCLUDEPATH += /opt/opencv-4.1.1/include \
                /opt/opencv-4.1.1/include/opencv4 \
                /opt/opencv-4.1.1/include/opencv4/opencv2

LIBS += /opt/opencv-4.1.1/lib/libopencv_highgui.so \
        /opt/opencv-4.1.1/lib/libopencv_core.so \
        /opt/opencv-4.1.1/lib/libopencv_videoio.so \
        /opt/opencv-4.1.1/lib/libopencv_imgproc.so \
        /opt/opencv-4.1.1/lib/libopencv_objdetect.so \
        /opt/opencv-4.1.1/lib/libopencv_imgcodecs.so
}else{
INCLUDEPATH += /opt/opencv-4.1.1L/include \
                /opt/opencv-4.1.1L/include/opencv4 \
                /opt/opencv-4.1.1L/include/opencv4/opencv2

LIBS += /opt/opencv-4.1.1L/lib/libopencv_highgui.so \
        /opt/opencv-4.1.1L/lib/libopencv_core.so \
        /opt/opencv-4.1.1L/lib/libopencv_videoio.so \
        /opt/opencv-4.1.1L/lib/libopencv_imgproc.so \
        /opt/opencv-4.1.1L/lib/libopencv_objdetect.so \
        /opt/opencv-4.1.1L/lib/libopencv_imgcodecs.so

}
