add_subdirectory(filters)
add_subdirectory(image)
add_subdirectory(input_output)
add_subdirectory(parser)

add_library(image_processor_lib filters image input_output parser)

target_link_libraries(image_processor_lib PUBLIC filters_lib image_lib input_output_lib parser_lib)