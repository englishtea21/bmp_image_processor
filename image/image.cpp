#include <image.h>

ImageBMP::ImageBMP(size_t width, size_t height) {
    width_ = width;
    height_ = height;
    this->image_pixels_ = std::vector<std::vector<Color>>(height);
}
ImageBMP::ImageBMP(const std::vector<std::vector<Color>>& image_pixels){
    
}
ImageBMP(const ImageBMP& other);