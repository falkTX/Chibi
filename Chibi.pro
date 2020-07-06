QT += core gui widgets

CONFIG += c++11 link_pkgconfig

DEFINES += QT_DEPRECATED_WARNINGS

PKGCONFIG += carla-standalone carla-utils

# FIXME for x11 systems only
QT += x11extras
PKGCONFIG += x11

SOURCES += \
    chibiembedwidget.cpp \
    main.cpp \
    chibiwindow.cpp

HEADERS += \
    chibiembedwidget.h \
    chibiwindow.h

FORMS += \
    resources/chibiwindow.ui

TRANSLATIONS += \
    i18n/en_GB.ts

# Default rules for deployment.
# qnx: target.path = /tmp/$${TARGET}/bin
# else: unix:!android: target.path = /opt/$${TARGET}/bin
# !isEmpty(target.path): INSTALLS += target
