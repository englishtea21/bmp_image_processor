#include "test_helper.h"
namespace test_helper {

double CalcImagesDistance(const ImageBmp& image1, const ImageBmp& image2) {
    if (image1.GetHeight() != image2.GetHeight() || image1.GetWidth() != image2.GetWidth()) {
        throw std::invalid_argument(utils::IMAGE_SIDES_DONT_FIT);
    }

    double images_distance = 0;

    for (size_t i = 0; i < image1.GetHeight(); i++) {
        for (size_t j = 0; j < image1.GetWidth(); j++) {
            images_distance += CalcPixelsDistance(image1.GetImagePixel(i, j), image2.GetImagePixel(i, j));
        }
    }

    return std::sqrt(images_distance / static_cast<double>(image1.GetHeight() * image1.GetWidth()));
}

double CalcPixelsDistance(const Pixel<uint8_t>& pixel1, const Pixel<uint8_t>& pixel2) {
    return std::pow(static_cast<int32_t>(pixel1.GetBlue()) - static_cast<int32_t>(pixel2.GetBlue()), 2) +
           std::pow(static_cast<int32_t>(pixel1.GetGreen()) - static_cast<int32_t>(pixel2.GetGreen()), 2) +
           std::pow(static_cast<int32_t>(pixel1.GetRed()) - static_cast<int32_t>(pixel2.GetRed()), 2);
}
}  // namespace test_helper