set(FDOTOOL_SIGNER_ODOO
        digisigner.cpp
        digisigner.hpp)

find_package(Qt5 REQUIRED COMPONENTS
        Core
        Network)

include_directories(${CMAKE_BINARY_DIR})

add_library(signer STATIC
        ${FDOTOOL_SIGNER_ODOO})

target_link_libraries(signer
        Qt5::Core
        Qt5::Network
        settings)

target_include_directories(signer
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR})
