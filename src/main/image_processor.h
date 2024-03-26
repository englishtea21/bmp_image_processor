#ifndef IMAGE_PROCESSOR_H
#define IMAGE_PROCESSOR_H

#include <iostream>

#include "../parser/parser.h"
#include "../image/image.h"
#include "../input_output/bmp24_reader.h"
#include "../input_output/bmp24_writer.h"
#include "../filters/filter.h"

std::vector<parser::Token> GetTokens(int argc, char *argv[]);
ImageBmp GetImage(const std::string &path);
void WriteImage(const ImageBmp &image_bmp, const std::string &path);
void ApplyFilters(ImageBmp &image, const std::vector<parser::Token> &tokens);

#endif  // IMAGE_PROCESSOR_H