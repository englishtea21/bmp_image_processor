#ifndef IMAGE_H
#define IMAGE_H

#include <cstdint>
#include <cstddef>
#include <vector>

#include <stdexcept>
#include "pixel.h"

class ImageBMP
{
public:
    ImageBMP() = default;
    ImageBMP(size_t width, size_t height);
    ImageBMP(const std::vector<std::vector<Pixel<uint8_t>>> &image_pixels);
    ImageBMP(const ImageBMP &other);
    ~ImageBMP() = default;

    size_t GetWidth() const;
    size_t GetHeight() const;
    const std::vector<std::vector<Pixel<uint8_t>>> &GetImagePixels() const;
    const Pixel<uint8_t> &GetImagePixel(size_t y, size_t x) const;

    // Color& GeImagePixel(size_t x, size_t y);
    void SetPixel(size_t x, size_t y, const Pixel<uint8_t> &color);

private:
    size_t width_ = 0;
    size_t height_ = 0;

    std::vector<std::vector<Pixel<uint8_t>>> image_pixels_;

    void CheckCoordsBoundings(size_t x, size_t y) const;
};

#endif // IMAGE_H