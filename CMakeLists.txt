cmake_minimum_required(VERSION 3.24)

project(slot LANGUAGES CXX VERSION 0.1.0)

add_library(slot INTERFACE)

file(GLOB_RECURSE SLOT_HEADERS CONFIGURE_DEPENDS include/*.hpp)

target_sources(
    slot
    INTERFACE
        FILE_SET slot_headers_set
            TYPE HEADERS
            BASE_DIRS include
            FILES ${SLOT_HEADERS}
)

target_compile_features(slot INTERFACE cxx_std_23)

add_library(slot::slot ALIAS slot)

option(SLOT_BUILD_TESTING "build test" ${PROJECT_IS_TOP_LEVEL})

if(SLOT_BUILD_TESTING)
    enable_testing()
    add_subdirectory(test)
endif()

option(SLOT_BUILD_EXAMPLES "build examples" ${PROJECT_IS_TOP_LEVEL})

if(SLOT_BUILD_EXAMPLES)
    add_subdirectory(example)
endif()

option(SLOT_INSTALL "generate install rules for slot" ${PROJECT_IS_TOP_LEVEL})

if(SLOT_INSTALL)
    include(GNUInstallDirs)
    include(CMakePackageConfigHelpers)

    set(SLOT_INSTALL_CMAKEDIR ${CMAKE_INSTALL_LIBDIR}/cmake/slot)

    install(
        TARGETS slot
        EXPORT slot-targets
        FILE_SET slot_headers_set DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    )

    install(
        EXPORT slot-targets
        NAMESPACE slot::
        DESTINATION ${SLOT_INSTALL_CMAKEDIR}
        FILE slot-targets.cmake
    )

    configure_package_config_file(
        cmake/slot-config.cmake.in
        ${CMAKE_CURRENT_BINARY_DIR}/slot-config.cmake
        INSTALL_DESTINATION ${SLOT_INSTALL_CMAKEDIR}
    )

    write_basic_package_version_file(
        ${CMAKE_CURRENT_BINARY_DIR}/slot-config-version.cmake
        VERSION ${PROJECT_VERSION}
        COMPATIBILITY SameMajorVersion
        ARCH_INDEPENDENT
    )

    install(
        FILES
            ${CMAKE_CURRENT_BINARY_DIR}/slot-config.cmake
            ${CMAKE_CURRENT_BINARY_DIR}/slot-config-version.cmake
        DESTINATION ${SLOT_INSTALL_CMAKEDIR}
    )

    if(CMAKE_VERSION VERSION_GREATER_EQUAL "4.3")
        set(SLOT_INSTALL_CPS_DEFAULT ON)
    else()
        set(SLOT_INSTALL_CPS_DEFAULT OFF)
    endif()

    option(
        SLOT_INSTALL_CPS
        "generate CPS package info (requires CMake 4.3+)"
        ${SLOT_INSTALL_CPS_DEFAULT}
    )

    if(SLOT_INSTALL_CPS)
        if(CMAKE_VERSION VERSION_LESS "4.3")
            message(
                FATAL_ERROR
                "SLOT_INSTALL_CPS requires CMake 4.3 or newer (found ${CMAKE_VERSION})."
            )
        endif()

        install(
            PACKAGE_INFO slot
            EXPORT slot-targets
            LOWER_CASE_FILE
            VERSION ${PROJECT_VERSION}
            DEFAULT_TARGETS slot
        )
    endif()
endif()
