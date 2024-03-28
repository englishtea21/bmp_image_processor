set (CMAKE_CXX_STANDARD 20)
set (CMAKE_CXX_STANDARD_REQUIRED ON)

cmake_minimum_required(VERSION 3.4)
project(image_processor_proj)

enable_testing()

add_subdirectory(src)
add_subdirectory(unit_tests)

add_executable(
    image_processor
    main/image_processor.cpp
)

# Enable AddressSanitizer for Debug builds
# if(CMAKE_BUILD_TYPE STREQUAL "Debug")
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address")
# set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fsanitize=leak")
# set(ENV{LSAN_OPTIONS} "verbosity=1:log_threads=1")
# endif()
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=undefined")


# dependencies
target_link_libraries(image_processor image_processor_lib)

set_target_properties(image_processor_lib PROPERTIES LINKER_LANGUAGE CXX)