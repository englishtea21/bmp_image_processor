add_library(input_output utils.h bmp24.h bmp24.cpp
  bmp24_reader.h bmp24_reader.cpp bmp24_writer.h bmp24_writer.cpp)

target_link_libraries(input_output image)