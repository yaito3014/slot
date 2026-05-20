cmake_minimum_required(VERSION 3.24)

project(slot LANGUAGES CXX)

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
