set(FDOTOOL_WINDOW_MAIN
        mainwindow.cpp
        mainwindow.hpp
        mainwindow.ui)

set(FDOTOOL_WINDOW_PROCESS
        processwindow.cpp
        processwindow.hpp
        processwindow.ui)

find_package(Qt5 REQUIRED COMPONENTS
        Core
        Widgets
        Gui)

add_library(window
        ${FDOTOOL_WINDOW_MAIN}
        ${FDOTOOL_WINDOW_PROCESS})

target_link_libraries(window
        Qt5::Core
        Qt5::Gui
        Qt5::Widgets)

target_include_directories(window PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
