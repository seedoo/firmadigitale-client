set(FDOTOOL_SETTINGS_SOURCES
        fdosettings.cpp
        manager.cpp)

set(FDOTOOL_SETTINGS_HEADERS
        defaults.hpp
        fdosettings.hpp
        manager.hpp)

find_package(Qt5 REQUIRED COMPONENTS
        Core)

add_library(settings
        ${FDOTOOL_SETTINGS_SOURCES}
        ${FDOTOOL_SETTINGS_HEADERS})

target_link_libraries(settings
        Qt5::Core)

target_include_directories(settings
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR})
