QT += core gui widgets

CONFIG += debug c++11 link_pkgconfig

#     CARLA_FRONTEND_NO_CACHED_PLUGIN_API \

DEFINES += \
    BUILDING_CARLA \
    CARLA_BACKEND_NAMESPACE=Chibi \
    CARLA_PLUGIN_BUILD \
    CARLA_PLUGIN_ONLY_BRIDGE \
    CARLA_UTILS_USE_QT \
    HAVE_JACK \
    STATIC_PLUGIN_TARGET \
    QT_DEPRECATED_WARNINGS

# FIXME for x11 systems only
QT += x11extras
PKGCONFIG += jack x11

SOURCES += \
    main.cpp \
    chibiembedwidget.cpp \
    chibiwindow.cpp \
    carla/source/backend/CarlaStandalone.cpp \
    carla/source/backend/engine/CarlaEngine.cpp \
    carla/source/backend/engine/CarlaEngineClient.cpp \
    carla/source/backend/engine/CarlaEngineData.cpp \
    carla/source/backend/engine/CarlaEngineGraph.cpp \
    carla/source/backend/engine/CarlaEngineInternal.cpp \
    carla/source/backend/engine/CarlaEngineJACK.cpp \
    carla/source/backend/engine/CarlaEngineNative.cpp \
    carla/source/backend/engine/CarlaEnginePorts.cpp \
    carla/source/backend/engine/CarlaEngineRunner.cpp \
    carla/source/backend/plugin/CarlaPlugin.cpp \
    carla/source/backend/plugin/CarlaPluginBridge.cpp \
    carla/source/backend/plugin/CarlaPluginInternal.cpp \
    carla/source/backend/utils/Information.cpp \
    carla/source/backend/utils/JUCE.cpp \
    carla/source/backend/utils/PluginDiscovery.cpp \
    carla/source/frontend/carla_frontend.cpp \
    carla/source/frontend/pluginlist/pluginlistdialog.cpp \
    carla/source/jackbridge/jackbridge1.cpp \
    carla/source/jackbridge/jackbridge2.cpp \
    carla/source/modules/rtmempool/rtmempool.c \
    carla/source/modules/water/water.cpp

HEADERS += \
    chibiembedwidget.h \
    chibiwindow.h

INCLUDEPATH += \
    carla/source \
    carla/source/backend \
    carla/source/includes \
    carla/source/frontend \
    carla/source/frontend/utils \
    carla/source/modules \
    carla/source/utils

FORMS += \
    carla/source/frontend/pluginlist/pluginlistdialog.ui

TRANSLATIONS += \
    i18n/en_GB.ts

# Default rules for deployment.
# qnx: target.path = /tmp/$${TARGET}/bin
# else: unix:!android: target.path = /opt/$${TARGET}/bin
# !isEmpty(target.path): INSTALLS += target
