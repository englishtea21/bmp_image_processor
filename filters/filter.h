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
    std::vector<T> GetPixel(const std::vector<std::vector<T>>& matrix, const ImageBMP& image_bmp, size_t x,
                            size_t y) const;
};

class Sharpening : public Filter {
public:
    ImageBMP Apply(const ImageBMP& image) const override;
};

uint8_t NormalizeColor(int color, const uint8_t max_color = 255);

std::unique_ptr<Filter> CreateFilter(const parser::Token& token);
}  // namespace filters