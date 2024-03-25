#ifndef BMP24_H
#define BMP24_H

#include "utils.h"
#include <algorithm>
#include <stdexcept>
namespace bmp24 {

template <typename T>
void NumToBytes(T num, unsigned char *bytes);

size_t BytesToNum(const unsigned char *bytes);

const size_t GetPaddingSize(size_t width);

struct Bmp24Header {
public:
    Bmp24Header(size_t image_width, size_t image_height);
    Bmp24Header(unsigned char bmp24_header_bytes[utils::HEADER_SIZE], std::string_view path);
    unsigned char *GetBmp24HeaderBytes();

private:
    unsigned char bmp24_header_bytes_[utils::HEADER_SIZE] = {};
};

struct Bmp24DibHeader {
public:
    Bmp24DibHeader(size_t image_width, size_t image_height);
    explicit Bmp24DibHeader(unsigned char *bmp24_dib_header_bytes);
    unsigned char *GetBmp24DibHeaderBytes();
    size_t GetWidth() const;
    size_t GetHeight() const;

private:
    unsigned char bmp24_dib_header_bytes_[utils::DIB_HEADER_SIZE] = {};
};
}  // namespace bmp24

#endif  // BMP24_H