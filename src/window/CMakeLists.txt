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
        Gui)

include_directories(${CMAKE_BINARY_DIR})

add_library(window
        ${FDOTOOL_WINDOW_MAIN}
        ${FDOTOOL_WINDOW_PROCESS}
        ${FDOTOOL_WINDOW_CONFIG})

target_link_libraries(window
        Qt5::Core
        Qt5::Gui
        Qt5::Widgets
        utility
        signer)

target_include_directories(window PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
