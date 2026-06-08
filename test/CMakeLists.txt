include(CTest)

find_package(Catch2 3 CONFIG REQUIRED)

function(slot_add_test test_name)
    set(name slot_${test_name}_test)
    add_executable(${name} ${test_name}.cpp)
    target_link_libraries(${name} PRIVATE slot::slot Catch2::Catch2WithMain)
    add_test(NAME ${name} COMMAND ${name})
endfunction()

function(slot_add_tests)
    foreach(test_name ${ARGN})
        slot_add_test(${test_name})
    endforeach()
endfunction()

slot_add_tests(
    writable
    binding
    direct_or_temp
)
