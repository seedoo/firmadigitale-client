include(ExternalProject)

if (UNIX AND NOT APPLE)
    if ("${CMAKE_SIZEOF_VOID_P}" STREQUAL "8")
        set(TOOLS_OPENSSL_CONFIGURE_ARCH "linux-x86_64")
    else ()
        set(TOOLS_OPENSSL_CONFIGURE_ARCH "linux-i386")
    endif ()

    set(TOOLS_OPENSSL_CONFIGURE_COMMAND ./Configure --prefix=${CMAKE_BINARY_DIR}/openssl_patched no-shared no-idea no-mdc2 no-rc5 ${TOOLS_OPENSSL_CONFIGURE_ARCH})
    set(TOOLS_OPENSSL_BINARY openssl)
elseif (WIN32)
    if ("${CMAKE_SIZEOF_VOID_P}" STREQUAL "8")
        set(TOOLS_OPENSSL_CONFIGURE_ARCH "linux-x86_64")
    else ()
        set(TOOLS_OPENSSL_CONFIGURE_ARCH "linux-i386")
    endif ()

    set(TOOLS_OPENSSL_CONFIGURE_COMMAND ./Configure --prefix=${CMAKE_BINARY_DIR}/openssl_patched no-shared no-idea no-mdc2 no-rc5 ${TOOLS_OPENSSL_CONFIGURE_ARCH})
    set(TOOLS_OPENSSL_BINARY openssl.exe)
elseif (APPLE)
    if ("${CMAKE_SIZEOF_VOID_P}" STREQUAL "8")
        set(TOOLS_OPENSSL_CONFIGURE_ARCH "linux-x86_64")
    else ()
        set(TOOLS_OPENSSL_CONFIGURE_ARCH "linux-i386")
    endif ()

    set(TOOLS_OPENSSL_CONFIGURE_COMMAND ./Configure --prefix=${CMAKE_BINARY_DIR}/openssl_patched no-shared no-idea no-mdc2 no-rc5 ${TOOLS_OPENSSL_CONFIGURE_ARCH})
    set(TOOLS_OPENSSL_BINARY openssl)
endif ()

ExternalProject_Add(
        OpenSSLPatched
        URL https://www.openssl.org/source/openssl-1.0.2m.tar.gz
        URL_HASH SHA256=8c6ff15ec6b319b50788f42c7abc2890c08ba5a1cdcd3810eb9092deada37b0f
        PATCH_COMMAND patch -Np1 -i ${CMAKE_SOURCE_DIR}/tools/openssl-1.0.2m-cades.patch
        CONFIGURE_COMMAND ${TOOLS_OPENSSL_CONFIGURE_COMMAND}
        BUILD_COMMAND make depend && make
        INSTALL_COMMAND make install_sw
        BUILD_IN_SOURCE 1
)

install(PROGRAMS ${CMAKE_BINARY_DIR}/openssl_patched/bin/${TOOLS_OPENSSL_BINARY}
        DESTINATION ${TOOLS_INSTALL_DIR}
        COMPONENT OpenSSLPatched)
