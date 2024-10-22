cmake_minimum_required(VERSION 3.25.1)

project(LineOfSightAnalyzer VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_PREFIX_PATH "$ENV{Qt6_DIR}")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(INCLUDE_DIR Libs/qtimgui/include)    
set(LIBS_DIR    Libs/qtimgui/lib)

set(LIBS    debug   debug/imgui             optimized release/imgui
            debug   debug/qt_imgui_widgets  optimized release/qt_imgui_widgets)

find_package(Qt6 COMPONENTS Core Widgets OpenGL Gui REQUIRED)

add_compile_options(/bigobj)

file(GLOB_RECURSE SOURCES Source/*.cpp Source/*.h LineOfSightAnalyzer.qrc)

include_directories("${CMAKE_CURRENT_SOURCE_DIR}/Source" ${INCLUDE_DIR})

link_directories(${LIBS_DIR})

add_executable(LineOfSightAnalyzer ${SOURCES})

target_link_libraries(LineOfSightAnalyzer Qt6::Core Qt6::Widgets Qt6::OpenGL ${LIBS})

add_custom_command(TARGET LineOfSightAnalyzer
    POST_BUILD COMMAND ${CMAKE_COMMAND}
    -E copy_directory
    "${CMAKE_SOURCE_DIR}/Resources/"
    "$<TARGET_FILE_DIR:LineOfSightAnalyzer>/Resources/")

add_custom_command(TARGET LineOfSightAnalyzer
    POST_BUILD COMMAND
    Qt6::windeployqt
    --dir "$<TARGET_FILE_DIR:LineOfSightAnalyzer>"
    "$<TARGET_FILE_DIR:LineOfSightAnalyzer>/$<TARGET_FILE_NAME:LineOfSightAnalyzer>")
