#ifndef UTILS_H;
#define UTILS_H ;

#include <stddef.h>
#include <vector>

namespace image::utils {
const int BMP_HEADER_SIZE = 14;
const int DIB_HEADER_SIZE = 40;
const int BITS_PER_PIXEL = 24;
const int BYTES_PER_PIXEL = 3;
const int PADDING_SIZE = 4;
const int WIDTH_POSITION = 4;
const int HEIGHT_POSITION = 8;
const int FORMAT_FIRST_POSITION = 0;
const int FORMAT_SECOND_POSITION = 1;
const int PIXS_ARRAY_OFFSET_POSITION = 10;
// conts int

const std::vector<char> BMP_SIGNAME = {'B', 'M'};
const std::vector<size_t> SHIFT_BITS{8, 16, 24};
}  // namespace image::utils

#endif  // UTILS_H