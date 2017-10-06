set(FDOTOOL_SOURCES
        fdotool.cpp
        action.cpp)

set(FDOTOOL_HEADERS
        version.hpp
        fdotool.hpp
        action.hpp)

set(FDOTOOL_RESOURCES
        fdotool.rc
        resources.qrc)

find_package(Qt5 REQUIRED COMPONENTS
        Core
        Gui
        Svg
        Widgets)

set(FDOTOOL_MAC_ICON_NAME icon.icns)
set(FDOTOOL_MAC_ICON_PATH ../icons/macos/icon.icns)

set_source_files_properties(${FDOTOOL_MAC_ICON_PATH}
        PROPERTIES
        MACOSX_PACKAGE_LOCATION
        Resources)

set(MACOSX_BUNDLE_EXECUTABLE_NAME fdotool)
set(MACOSX_BUNDLE_BUNDLE_NAME FDOTool)
set(MACOSX_BUNDLE_ICON_FILE ${FDOTOOL_MAC_ICON_NAME})

add_executable(fdotool
        WIN32 MACOSX_BUNDLE
        ${FDOTOOL_SOURCES}
        ${FDOTOOL_HEADERS}
        ${FDOTOOL_RESOURCES}
        ${FDOTOOL_MAC_ICON_PATH})

target_link_libraries(fdotool
#        card
        Qt5::Core
        Qt5::Gui
        Qt5::Svg
        Qt5::Widgets)

set_target_properties(fdotool
        PROPERTIES
        MACOSX_BUNDLE_ICON_FILE
        ${FDOTOOL_MAC_ICON_NAME})
