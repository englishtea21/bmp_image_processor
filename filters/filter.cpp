#include "filter.h"
#include "utils.h"

template <typename T>
Pixel<T> filters::Filter::GetPixel(const std::vector<std::vector<T>>& matrix, const ImageBMP& image_bmp, size_t x,
                                   size_t y) const {
    Pixel<T> pixel{0, 0, 0};
    for (size_t i = 0; i < matrix.size(); ++i) {
        for (size_t j = 0; j < matrix.front().size(); ++j) {
            pixel += image_bmp.GeImagePixel(std::clamp(x + i - 1, static_cast<size_t>(0), image_bmp.GetHeight() - 1),
                                            std::clamp(y + j - 1, static_cast<size_t>(0), image_bmp.GetWidth() - 1)) *
                     matrix[i][j];
        }
    }

    return pixel;
}

namespace filters {
ImageBMP Sharpening::Apply(const ImageBMP& image) const {
    std::vector<std::vector<Pixel<uint8_t>>> new_data;

    for (size_t i = 0; i < image.GetHeight(); ++i) {
        std::vector<Pixel<uint8_t>> row(image.GetWidth());
        for (size_t j = 0; j < image.GetWidth(); j++) {
            Pixel<int> color = GetPixel<int>(filters::utils::matrices::sharpenning, image, i, j);
            row[j] = color.NormalizePixel<uint8_t>();
        }
        new_data.push_back(row);
    }
    return {new_data};
}

ImageBMP Negative::Apply(const ImageBMP& image) const {
    std::vector<std::vector<Pixel<uint8_t>>> new_data;

    for (size_t i = 0; i < image.GetHeight(); ++i) {
        std::vector<Pixel<uint8_t>> row(image.GetWidth());
        for (size_t j = 0; j < image.GetWidth(); j++) {
            row[j] = filters::utils::pixels::white - image.GeImagePixel(i, j);
        }
        new_data.push_back(row);
    }
    return {new_data};
}

std::unique_ptr<Filter> CreateFilter(const parser::Token& token) {
    const std::string& filter = token.name;
    if (filter == "-sharp") {
        if (!token.args.empty()) {
            throw std::invalid_argument("Too many arguments for filter \"" + filter + "\"");
        }
        auto ptr = std::make_unique<Sharpening>();
        return ptr;
    } else {
        throw std::invalid_argument("Unknown filter: \"" + filter + "\"");
    }
}
}  // namespace filters