add_library(catch_main ../catch/catch_main.cpp)

target_include_directories(catch_main PUBLIC ../catch)

add_executable(unit_tests tests.cpp)

target_link_libraries(unit_tests image_processor_lib catch_main)