include(FetchContent)

FetchContent_Declare(
    Catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2
    GIT_TAG v3.15.0
    GIT_SHALLOW ON
)

function(slot_provide_dependency method package_name)
    if("${package_name}" STREQUAL "Catch2")
        FetchContent_MakeAvailable(Catch2)
        set(Catch2_FOUND ON)
    endif()
endfunction()

cmake_language(
    SET_DEPENDENCY_PROVIDER slot_provide_dependency
    SUPPORTED_METHODS FIND_PACKAGE
)
