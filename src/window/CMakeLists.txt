set(FDOTOOL_WINDOW_MAIN
        mainwindow.cpp
        mainwindow.hpp
        mainwindow.ui)

set(FDOTOOL_WINDOW_PROCESS
        processwindow.cpp
        processwindow.hpp
        processwindow.ui)

set(FDOTOOL_WINDOW_CONFIG
        configdialog.cpp
        configdialog.hpp
        configdialog.ui)

find_package(Qt5 REQUIRED COMPONENTS
        Core
        Widgets
        Gui
        Svg)

include_directories(${CMAKE_BINARY_DIR})

add_library(window STATIC
        ${FDOTOOL_WINDOW_MAIN}
        ${FDOTOOL_WINDOW_PROCESS}
        ${FDOTOOL_WINDOW_CONFIG})

target_link_libraries(window
        Qt5::Core
        Qt5::Gui
        Qt5::Widgets
        Qt5::Svg
        signer)

target_include_directories(window PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
