set(FDOTOOL_SIGNER_ODOO
        digisigner.cpp
        digisigner.hpp)

find_package(Qt5 REQUIRED COMPONENTS
        Core)

add_library(signer
        ${FDOTOOL_SIGNER_ODOO})

target_link_libraries(signer
        Qt5::Core)

target_include_directories(signer
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR})
