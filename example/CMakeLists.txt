function(slot_add_example example_name)
    add_executable(slot_${example_name}_example ${example_name}.cpp)
    target_link_libraries(slot_${example_name}_example PRIVATE slot::slot)
endfunction()

slot_add_example(cereal_like)
