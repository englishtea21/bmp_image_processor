#ifndef BMP24_UTILS_H
#define BMP24_UTILS_H

#include <stddef.h>
#include <vector>
#include <string>

namespace bmp24::utils {
// Bytes proceeding
const uint8_t BYTE_MASK = 0xff;

// BMP general
const std::vector<size_t> SHIFT_BITS{8, 16, 24};
const size_t PADDING_SIZE = 4;
const size_t BITS_PER_PIXEL = 24;
const size_t BYTES_PER_PIXEL = 3;
const size_t COLOR_CHANNEL_MAX_VALUE = 255;
const size_t COLOR_CHANNELS_AMOUNT = 3;

// BMP header data
const size_t HEADER_SIZE = 14;
const std::vector<char> BMP_SIGN{'B', 'M'};
// BMP header data positioning
const int FORMAT_FIRST_POSITION = 0;
const int FORMAT_SECOND_POSITION = 1;
const int WIDTH_POSITION = 4;
const int HEIGHT_POSITION = 8;
const int FILE_SIZE_POSITION = 2;
const int PIXS_ARRAY_OFFSET_POSITION = 10;

// DIB header data
const size_t DIB_HEADER_SIZE = 40;
const int DEFAULT_NUMBER_OF_PLANES = 1;
// DIB header data positioning
const int DIB_HEADER_SIZE_POSITION = 0;
const int PLANES_POSITION = 12;
const int BITS_PER_PIXEL_POSITION = 14;
const int IMAGE_SIZE_POSITION = 24;
const int IMAGE_DATA_SIZE_POSITION = 30;
}  // namespace bmp24::utils

namespace bmp24::utils::exceptions {
const std::string FILE_OPENNING_ERROR = "Cannot open file: ";
const std::string FILE_PERMISSIONS_ERROR = "Permission denied to file: ";
const std::string FILE_READING_ERROR = "Error reading file: ";
const std::string FILE_TYPE_ERROR = "Not a BMP file: ";

}  // namespace bmp24::utils::exceptions

#endif  // BMP24_UTILS_H