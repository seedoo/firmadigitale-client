set(FDOTOOL_UTILITY_SOURCES
        utility.cpp)

set(FDOTOOL_UTILITY_HEADERS
        utility.hpp)

find_package(Qt5 REQUIRED COMPONENTS
        Core)

include_directories(${CMAKE_BINARY_DIR})

add_library(utility
        ${FDOTOOL_UTILITY_SOURCES}
        ${FDOTOOL_UTILITY_HEADERS})

target_link_libraries(utility
        Qt5::Core
        settings)

target_include_directories(utility
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR})
