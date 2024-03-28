#ifndef __TEST_HELPER_H__
#define __TEST_HELPER_H__

#include "../main/image_processor.h"
#include "utils.h"
#include <cmath>

namespace test_helper {

double CalcImagesDistance(const ImageBmp& image1, const ImageBmp& image2);
double CalcPixelsDistance(const Pixel<uint8_t>& pixel1, const Pixel<uint8_t>& pixel2);
};      // namespace test_helper
#endif  // __TEST_HELPER_H__