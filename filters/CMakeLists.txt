add_library(filters utils.h
  filter.cpp filter.h)

target_link_libraries(filters image parser)