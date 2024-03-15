#include "pixel.h"

template <typename T>
Pixel<T>::Pixel(T blue, T green, T red) {
    this->blue_ = blue;
    this->green_ = green;
    this->red_ = red;
}

template <typename T>
void Pixel<T>::SetPixel(T blue, T green, T red) {
    this->blue_ = blue;
    this->green_ = green;
    this->red_ = red;
}

template <typename T>
template <typename U>
Pixel<U> Pixel<T>::NormalizePixel(const uint8_t max_color) {
    Pixel<U> normalized_pixel;

    normalized_pixel.blue_ = std::clamp(this->blue_, 0, max_color);
    normalized_pixel.green_ = std::clamp(green_, 0, max_color);
    normalized_pixel.red_ = std::clamp(red_, 0, max_color);

    return normalized_pixel;
}

template <typename T>
template <typename U>
Pixel<T> Pixel<T>::MultiplyPixel(U mult) {
    Pixel<U> pixel_copy{static_cast<U>(blue_), static_cast<U>(green_), static_cast<U>(red_)};
    pixel_copy.blue_ *= mult;
    pixel_copy.green_ *= mult;
    pixel_copy.red_ *= mult;

    return pixel_copy.NormalizePixel();
}

template <typename T>
Pixel<T>& Pixel<T>::operator+=(Pixel<T>& other) {
    blue_ += other.blue_;
    green_ += other.green_;
    red_ += other.red_;

    return *this;
}

template <typename T>
Pixel<T> Pixel<T>::operator-(const Pixel<T>& lhs, const Pixel<T>& rhs) {
}
