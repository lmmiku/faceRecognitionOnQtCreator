QT       += core gui sql charts axcontainer

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

INCLUDEPATH += C:/Users/shocker/QTpractise/face_recognition/include\
                C:/Users/shocker/QTpractise/face_recognition/include/opencv\
                C:/Users/shocker/QTpractise/face_recognition/include/opencv2\
                C:/Users/shocker/QTpractise/face_recognition/dlib/include\
                C:/Users/shocker/QTpractise/face_recognition/dlib/include/dlib/gui_widgets

LIBS += C:/Users/shocker/QTpractise/face_recognition/x64/mingw/bin/libopencv_*.dll\
        C:/Users/shocker/QTpractise/face_recognition/dlib/lib/libdlib.a

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    database.cpp \
    facerecongnition.cpp \
    hogsvm.cpp \
    liveblink.cpp \
    log.cpp \
    main.cpp \
    managementinfo.cpp \
    src/bif.cpp \
    src/eigen_faces.cpp \
    src/face_alignment.cpp \
    src/face_basic.cpp \
    src/facemark.cpp \
    src/facemarkAAM.cpp \
    src/facemarkLBF.cpp \
    src/facerec.cpp \
    src/fisher_faces.cpp \
    src/getlandmarks.cpp \
    src/lbph_faces.cpp \
    src/mace.cpp \
    src/predict_collector.cpp \
    src/regtree.cpp \
    src/trainFacemark.cpp \
    threadfacerecong.cpp \
    threadfacerecord.cpp \
    widget.cpp

HEADERS += \
    database.h \
    face/bif.hpp \
    face/face_alignment.hpp \
    face/facemark.hpp \
    face/facemarkAAM.hpp \
    face/facemarkLBF.hpp \
    face/facemark_train.hpp \
    face/facerec.hpp \
    face/mace.hpp \
    face/predict_collector.hpp \
    facerecongnition.h \
    hogsvm.h \
    liveblink.h \
    log.h \
    managementinfo.h \
    src/face_alignmentimpl.hpp \
    src/face_utils.hpp \
    src/precomp.hpp \
    threadfacerecong.h \
    threadfacerecord.h \
    widget.h

FORMS += \
    AdminLogin.ui \
    InformationEntry.ui \
    facerecongnition.ui \
    managementinfo.ui \
    widget.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES += \

RESOURCES += \
    image/rc.qrc
