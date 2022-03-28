QT       += core gui xml

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++14

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

INCLUDEPATH += widgets

SOURCES += \
    ../utils/Augmentations.cpp \
    ../utils/tinystr.cpp \
    ../utils/tinyxml.cpp \
    ../utils/tinyxmlerror.cpp \
    ../utils/tinyxmlparser.cpp \
    ../utils/util.cpp \
    canvas.cpp \
    main.cpp \
    labeltool.cpp \
    managers/annotationmanager.cpp \
    managers/colormanager.cpp \
    managers/labelmanager.cpp \
    widgets/labeldialog.cpp \
    widgets/labellineedit.cpp \
    widgets/labeldialog.cpp \
    widgets/labellineedit.cpp

HEADERS += \
    ../utils/Augmentations.h \
    ../utils/_dirent.h \
    ../utils/readfile.h \
    ../utils/tinystr.h \
    ../utils/tinyxml.h \
    ../utils/util.h \
    canvas.h \
    labeltool.h \
    managers/annotationmanager.h \
    managers/colormanager.h \
    managers/labelmanager.h \
    widgets/labeldialog.h \
    widgets/labellineedit.h \
    widgets/labeldialog.h \
    widgets/labellineedit.h

FORMS += \
    labeltool.ui \
    widgets/labelDialog.ui \
    widgets/labeldialog.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    icons.qrc

DISTFILES +=


INCLUDEPATH += D:\AllentFiles\code\dependency\opencv-4.5.0-vc14_vc15\opencv\build\include \
D:\AllentFiles\code\dependency\libtorch17release\include \
D:\AllentFiles\code\dependency\libtorch17release\include\torch\csrc\api\include

LIBS += -LD:\AllentFiles\code\dependency\opencv-4.5.0-vc14_vc15\opencv\build\x64\vc15\lib -lopencv_world450 \
-LD:\AllentFiles\code\dependency\libtorch17release\lib -lc10 -ltorch -lc10_cuda -lcaffe2_detectron_ops_gpu -lc10d -ltorch_cpu \
-ltorch_cuda -lgloo -lcaffe2_module_test_dynamic -lasmjit -lcaffe2_nvrtc -lclog -lcpuinfo -ldnnl -lfbgemm -lgloo_cuda \
-lmkldnn -INCLUDE:?warp_size@cuda@at@@YAHXZ
