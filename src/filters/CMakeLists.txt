add_library(filters_lib utils.h
  filter.cpp filter.h)

target_link_libraries(filters_lib image_lib parser_lib)