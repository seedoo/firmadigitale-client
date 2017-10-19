set(FDOTOOL_WORKER_ODOO
        odooworker.cpp
        odooworker.hpp)

find_package(Qt5 REQUIRED COMPONENTS
        Core
        Network)

add_library(worker
        ${FDOTOOL_WORKER_ODOO})

target_link_libraries(worker
        Qt5::Core
        Qt5::Network)

target_include_directories(worker PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
