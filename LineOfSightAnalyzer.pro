QT       += core gui openglwidgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

include(Dependencies/qtimgui/qtimgui.pri)

SOURCES += \
    Camera.cpp \
    Controller.cpp \
    FreeCamera.cpp \
    Helper.cpp \
    Light.cpp \
    Main.cpp \
    Mouse.cpp \
    Node.cpp \
    OrthographicCamera.cpp \
    PerspectiveCamera.cpp \
    Quad.cpp \
    Shader.cpp \
    ShaderManager.cpp \
    Terrain.cpp \
    Window.cpp

HEADERS += \
    Camera.h \
    Common.h \
    Constants.h \
    Controller.h \
    FreeCamera.h \
    Helper.h \
    Light.h \
    Mouse.h \
    Node.h \
    OrthographicCamera.h \
    PerspectiveCamera.h \
    Quad.h \
    Shader.h \
    ShaderManager.h \
    Terrain.h \
    Window.h

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    Resources.qrc
