#ifndef IMAGE_H
#define IMAGE_H

#include <cstdint>
#include <cstddef>
#include <vector>

#include "color.h"

class ImageBMP {
public:
    ImageBMP() = default;
    ImageBMP(size_t width, size_t height);
    ImageBMP(const std::vector<std::vector<Color>>& image_pixels);
    ImageBMP(const ImageBMP& other);
    ~ImageBMP() = default;

    size_t GetWidth() const;
    size_t GetHeight() const;
    const const std::vector<std::vector<Color>>& GetImagePixels() const;
    const Color& GeImagePixel(size_t x, size_t y) const;

    Color& GeImagePixel(size_t x, size_t y);
    void SetPixel(size_t x, size_t y, const Color& color);

private:
    size_t width_ = 0;
    size_t height_ = 0;

    std::vector<std::vector<Color>> imgae_pixels_;

    void CheckCoordsBoundings(size_t x, size_t y) const;
};

#endif  // IMAGE_H