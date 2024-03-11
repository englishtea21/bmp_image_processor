#include "filter.h"

template <typename T>
std::vector<T> filters::Filter::GetPixel(const std::vector<std::vector<T>>& matrix, const ImageBMP& image_bmp, size_t x,
                                         size_t y) const {
    T blue = 0, green = 0, red = 0;
    for (size_t i = 0; i < matrix.size(); ++i) {
        for (size_t j = 0; j < matrix.front().size(); ++j) {
            blue += image_bmp
                        .GeImagePixel(std::clamp(x + i - 1, static_cast<size_t>(0), image_bmp.GetHeight() - 1),
                                      std::clamp(y + j - 1, static_cast<size_t>(0), image_bmp.GetWidth() - 1))
                        .blue_ = matrix[i][j];
            green += image_bmp
                         .GeImagePixel(std::clamp(x + i - 1, static_cast<size_t>(0), image_bmp.GetHeight() - 1),
                                       std::clamp(y + j - 1, static_cast<size_t>(0), image_bmp.GetWidth() - 1))
                         .green_ = matrix[i][j];
            red += image_bmp
                       .GeImagePixel(std::clamp(x + i - 1, static_cast<size_t>(0), image_bmp.GetHeight() - 1),
                                     std::clamp(y + j - 1, static_cast<size_t>(0), image_bmp.GetWidth() - 1))
                       .red_ = matrix[i][j];
        }
    }

    return {blue, green, red};
}

uint8_t filters::NormalizeColor(int color, const uint8_t max_color = 255) {
    if (color < 0) {
        return 0;
    } else if (color > max_color) {
        return max_color;
    }
    return static_cast<uint8_t>(color);
}

namespace filters {
ImageBMP Sharpening::Apply(const ImageBMP& image) const {
    std::vector<std::vector<Color>> new_data;
    const std::vector<std::vector<int>> matrix = {{0, -1, 0}, {-1, 5, -1}, {0, -1, 0}};

    for (size_t i = 0; i < image.GetHeight(); ++i) {
        std::vector<Color> row(image.GetWidth());
        for (size_t j = 0; j < image.GetWidth(); j++) {
            std::vector<int> colors = GetPixel<int>(matrix, image, i, j);
            row[j].blue_ = NormalizeColor(colors[0]);
            row[j].green_ = NormalizeColor(colors[1]);
            row[j].red_ = NormalizeColor(colors[2]);
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