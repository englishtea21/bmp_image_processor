#include "image.h"

ImageBmp::ImageBmp(size_t width, size_t height) {
    width_ = width;
    height_ = height;
    this->image_pixels_ = std::vector<std::vector<Pixel<uint8_t>>>(height);
}

ImageBmp::ImageBmp(const std::vector<std::vector<Pixel<uint8_t>>> &image_pixels) {
    this->height_ = image_pixels.size();
    this->width_ = image_pixels.front().size();

    this->image_pixels_ = image_pixels;
}

ImageBmp::ImageBmp(const ImageBmp &other) : ImageBmp(other.GetImagePixels()) {
}

bool ImageBmp::operator==(const ImageBmp &other) const {
    return this->image_pixels_ == other.image_pixels_;
}

size_t ImageBmp::GetWidth() const {
    return this->width_;
}

size_t ImageBmp::GetHeight() const {
    return this->height_;
}

const std::vector<std::vector<Pixel<uint8_t>>> &ImageBmp::GetImagePixels() const {
    return this->image_pixels_;
}

void ImageBmp::CheckCoordsBoundings(size_t y, size_t x) const {
    if (y < 0 || x < 0 || y >= this->height_ || x >= this->width_) {
        std::invalid_argument("Invalid pixel coords");
    }
}

const Pixel<uint8_t> &ImageBmp::GetImagePixel(size_t y, size_t x) const {
    CheckCoordsBoundings(y, x);
    return this->image_pixels_[y][x];
}

void ImageBmp::SetPixel(size_t y, size_t x, const Pixel<uint8_t> &color) {
    CheckCoordsBoundings(y, x);
    this->image_pixels_[y][x] = color;
}