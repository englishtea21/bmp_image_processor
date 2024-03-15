#ifndef MATRICES_H;
#define MATRICES_H ;

#include <stddef.h>
#include <vector>
#include "../image/pixel.h"

namespace filters::utils::matrices {
const std::vector<std::vector<int>> sharpenning = {{0, -1, 0}, {-1, 5, -1}, {0, -1, 0}};
const std::vector<double> grayscale_color_ratio = {0.114, 0.587, 0.299};
}  // namespace filters::utils::matrices

namespace filters::utils::pixels {
const Pixel<uint8_t> white(255, 255, 255);
}  // namespace filters::utils::pixels

#endif  // MATRICES_H