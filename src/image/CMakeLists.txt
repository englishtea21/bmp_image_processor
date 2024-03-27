add_library(image_lib pixel.h pixel.cpp image.h image.cpp)

target_link_libraries(image_lib input_output_lib)