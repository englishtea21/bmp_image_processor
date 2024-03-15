#pragma once

#include <algorithm>
#include <memory>
#include <vector>

#include "../image/image.h"
#include "../parser/parser.h"

namespace filters {
class Filter {
public:
    Filter() = default;
    virtual ~Filter() = default;
    virtual ImageBMP Apply(const ImageBMP) = 0;

protected:
    template <typename T>
    Pixel<T> filters::Filter::GetPixel(const std::vector<std::vector<T>>& matrix, const ImageBMP& image_bmp, size_t x,
                                       size_t y) const;
};

class Sharpening : public Filter {
public:
    ImageBMP Apply(const ImageBMP& image) const;
};

class Negative : public Filter {
public:
    ImageBMP Apply(const ImageBMP& image) const;
};

class Grayscale : public Filter {
public:
    ImageBMP Apply(const ImageBMP& image) const;
};

std::unique_ptr<Filter> CreateFilter(const parser::Token& token);
}  // namespace filters