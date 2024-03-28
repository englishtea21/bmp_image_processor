project(image_processor_tests)

add_library(
        catch_main_lib
        ../../../contrib/catch/catch_main.cpp
)

target_include_directories(catch_main_lib PUBLIC ../../../contrib/catch)

add_executable(start_unit_tests tests.cpp test_helper.h test_helper.cpp)

target_link_libraries(start_unit_tests image_processor_lib catch_main_lib)