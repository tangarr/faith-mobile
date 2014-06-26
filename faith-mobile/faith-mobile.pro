TEMPLATE = app

QT += qml quick widgets sql

SOURCES += main.cpp \
    config.cpp \
    laboratoriesmodel.cpp \
    laboratoriesmodelelement.cpp \
    applicationmanager.cpp \
    computerlab.cpp \
    host.cpp \
    disk.cpp \
    partition.cpp \
    user.cpp

RESOURCES += qml.qrc

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Default rules for deployment.
include(deployment.pri)

OTHER_FILES +=

HEADERS += \
    config.h \
    laboratoriesmodel.h \
    laboratoriesmodelelement.h \
    applicationmanager.h \
    computerlab.h \
    host.h \
    disk.h \
    partition.h \
    user.h

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../faith-core/release/ -lfaith-core
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../faith-core/debug/ -lfaith-core
else:unix: LIBS += -L$$OUT_PWD/../faith-core/ -lfaith-core

INCLUDEPATH += $$PWD/../faith-core
DEPENDPATH += $$PWD/../faith-core

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../faith-core/release/libfaith-core.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../faith-core/debug/libfaith-core.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../faith-core/release/faith-core.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../faith-core/debug/faith-core.lib
else:unix: PRE_TARGETDEPS += $$OUT_PWD/../faith-core/libfaith-core.a


