cmake_minimum_required(VERSION 3.25.1)

project(LineOfSightAnalyzer VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_PREFIX_PATH "$ENV{Qt6_DIR}")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(LIBS_DIR        "${CMAKE_CURRENT_SOURCE_DIR}/Libs")
set(QT_IMGUI_DIR    "${LIBS_DIR}/qtimgui")
set(INCLUDE_DIR     "${QT_IMGUI_DIR}/include")
set(LIBS_DIR        "${QT_IMGUI_DIR}/lib")
set(LIBS
    debug imguid                optimized imgui
    debug qt_imgui_widgetsd     optimized qt_imgui_widgets
)

find_package(Qt6 COMPONENTS Core Widgets OpenGL Gui REQUIRED)

file(GLOB_RECURSE SOURCES Source/*.cpp LineOfSightAnalyzer.qrc)

add_executable(LineOfSightAnalyzer ${SOURCES})

target_include_directories(LineOfSightAnalyzer PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/Source" ${INCLUDE_DIR})

target_link_directories(LineOfSightAnalyzer PRIVATE ${LIBS_DIR})

target_link_libraries(LineOfSightAnalyzer Qt6::Core Qt6::Widgets Qt6::OpenGL ${LIBS})

add_custom_command(TARGET LineOfSightAnalyzer
    POST_BUILD COMMAND ${CMAKE_COMMAND}
    -E copy_directory
    "${CMAKE_SOURCE_DIR}/Resources/"
    "$<TARGET_FILE_DIR:LineOfSightAnalyzer>/Resources/"
)

add_custom_command(TARGET LineOfSightAnalyzer
    POST_BUILD COMMAND
    Qt6::windeployqt
    --dir "$<TARGET_FILE_DIR:LineOfSightAnalyzer>"
    "$<TARGET_FILE_DIR:LineOfSightAnalyzer>/$<TARGET_FILE_NAME:LineOfSightAnalyzer>"
)
