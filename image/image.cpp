#include <image.h>

ImageBMP::ImageBMP(size_t width, size_t height) {
    width_ = width;
    height_ = height;
    this->image_pixels_ = std::vector<std::vector<Pixel<uint8_t>>>(height);
}

ImageBMP::ImageBMP(const std::vector<std::vector<Pixel<uint8_t>>>& image_pixels) {
    this->height_ = image_pixels.size();
    this->width_ = image_pixels.front().size();

    this->image_pixels_ = image_pixels;
}

ImageBMP::ImageBMP(const ImageBMP& other) : ImageBMP(other.GetImagePixels()) {
}

size_t ImageBMP::GetWidth() const {
    return this->width_;
}

size_t ImageBMP::GetHeight() const {
    return this->height_;
}

const std::vector<std::vector<Pixel<uint8_t>>>& ImageBMP::GetImagePixels() const {
    return this->image_pixels_;
}

void ImageBMP::CheckCoordsBoundings(size_t x, size_t y) const {
    if (x < 0 || y < 0 || x > this->width_ || y > this->height_) {
        std::invalid_argument("Invalid pixel coords");
    }
}

const Pixel<uint8_t>& ImageBMP::GeImagePixel(size_t x, size_t y) const {
    CheckCoordsBoundings(x, y);
    return this->image_pixels_[x][y];
}

void ImageBMP::SetPixel(size_t x, size_t y, const Pixel<uint8_t>& color) {
    CheckCoordsBoundings(x, y);
    this->image_pixels_[x][y] = color;
}