#pragma once

#include <iostream>

#include "parser/parser.h"
#include "image/image.h"
#include "input_output/reader.h"
#include "input_output/writer.h"
#include "filters/filter.h"

std::vector<parser::Token> GetTokens(int argc, char* argv[]);
ImageBMP GetImage(const std::string& path);
void WriteImage(const ImageBMP& image_bmp, const std::string& path);
void ApplyFilters(ImageBMP& image, const std::vector<parser::Token>& tokens);