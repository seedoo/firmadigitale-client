include(ExternalProject)

if (WIN32)
    if ("${CMAKE_SIZEOF_VOID_P}" STREQUAL "8")
        set(TOOLS_OPENSSL_CONFIGURE_ARCH "VC-WIN64A")
        set(TOOLS_OPENSSL_CONFIGURE_DO ".\\ms\\do_win64a")
    else ()
        set(TOOLS_OPENSSL_CONFIGURE_ARCH "VC-WIN32")
        set(TOOLS_OPENSSL_CONFIGURE_DO ".\\ms\\do_ms.bat")
    endif ()

    set(TOOLS_OPENSSL_CONFIGURE_COMMAND perl Configure --prefix=${CMAKE_BINARY_DIR}/openssl_patched no-asm no-shared no-idea no-mdc2 no-rc5 ${TOOLS_OPENSSL_CONFIGURE_ARCH} && ${TOOLS_OPENSSL_CONFIGURE_DO})
    set(TOOLS_OPENSSL_BUILD_COMMAND nmake -f ms/nt.mak)
    set(TOOLS_OPENSSL_INSTALL_COMMAND nmake -f ms/nt.mak install)

    set(TOOLS_OPENSSL_BINARY openssl.exe)
elseif (APPLE)
    if ("${CMAKE_SIZEOF_VOID_P}" STREQUAL "8")
        set(TOOLS_OPENSSL_CONFIGURE_ARCH "linux-x86_64")
    else ()
        set(TOOLS_OPENSSL_CONFIGURE_ARCH "linux-i386")
    endif ()

    set(TOOLS_OPENSSL_CONFIGURE_COMMAND ./Configure --prefix=${CMAKE_BINARY_DIR}/openssl_patched no-asm no-shared no-idea no-mdc2 no-rc5 ${TOOLS_OPENSSL_CONFIGURE_ARCH})
    set(TOOLS_OPENSSL_BUILD_COMMAND make depend && make)
    set(TOOLS_OPENSSL_INSTALL_COMMAND make install_sw)

    set(TOOLS_OPENSSL_BINARY openssl)
else ()
    if ("${CMAKE_SIZEOF_VOID_P}" STREQUAL "8")
        set(TOOLS_OPENSSL_CONFIGURE_ARCH "linux-x86_64")
    else ()
        set(TOOLS_OPENSSL_CONFIGURE_ARCH "linux-i386")
    endif ()

    set(TOOLS_OPENSSL_CONFIGURE_COMMAND ./Configure --prefix=${CMAKE_BINARY_DIR}/openssl_patched no-asm no-shared no-idea no-mdc2 no-rc5 ${TOOLS_OPENSSL_CONFIGURE_ARCH})
    set(TOOLS_OPENSSL_BUILD_COMMAND make depend && make)
    set(TOOLS_OPENSSL_INSTALL_COMMAND make install_sw)

    set(TOOLS_OPENSSL_BINARY openssl)
endif ()


ExternalProject_Add(
        OpenSSLPatched
        URL https://www.openssl.org/source/openssl-1.0.2m.tar.gz
        URL_HASH SHA256=8c6ff15ec6b319b50788f42c7abc2890c08ba5a1cdcd3810eb9092deada37b0f
        PATCH_COMMAND patch -Np1 -i ${CMAKE_SOURCE_DIR}/tools/openssl-1.0.2m-cades.patch
        CONFIGURE_COMMAND ${TOOLS_OPENSSL_CONFIGURE_COMMAND}
        BUILD_COMMAND ${TOOLS_OPENSSL_BUILD_COMMAND}
        INSTALL_COMMAND ${TOOLS_OPENSSL_INSTALL_COMMAND}
        BUILD_IN_SOURCE 1
)

install(PROGRAMS ${CMAKE_BINARY_DIR}/openssl_patched/bin/${TOOLS_OPENSSL_BINARY}
        DESTINATION ${TOOLS_INSTALL_DIR}
        COMPONENT OpenSSLPatched)
