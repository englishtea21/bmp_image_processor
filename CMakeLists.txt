set (CMAKE_CXX_STANDARD 20)

cmake_minimum_required(VERSION 3.4)
project(image_processor.out)

add_executable(
    image_processor.out
    image_processor.cpp
)
add_subdirectory(filters)
add_subdirectory(image)
add_subdirectory(input_output)
add_subdirectory(parser)

# Enable AddressSanitizer for Debug builds
# if(CMAKE_BUILD_TYPE STREQUAL "Debug")
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address")
# set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fsanitize=leak")
# set(ENV{LSAN_OPTIONS} "verbosity=1:log_threads=1")
# endif()
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=undefined")


# dependencies
target_link_libraries(image_processor.out filters image input_output parser)