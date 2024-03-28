#ifndef PIXEL_H
#define PIXEL_H

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <tuple>
#include "../input_output/utils.h"

// Declaration
template <typename T>
class Pixel {
    static_assert((std::is_same<double, T>() || std::is_same<long double, T>() || std::is_same<int64_t, T>() ||
                   std::is_same<uint8_t, T>()),
                  "Only double, long double, int64_t, uint8_T types allowed!");

public:
    Pixel();
    Pixel(T blue, T green, T red);

    template <typename U>
    explicit Pixel(const Pixel<U> &pixel);

    Pixel &operator=(const Pixel &other);
    Pixel<T> &operator+=(const Pixel<T> &other);
    Pixel<T> &operator-=(const Pixel<T> &other);
    bool operator==(const Pixel<T> &other) const;

    ~Pixel() = default;

    void SetPixel(T blue, T green, T red);
    void SetPixel(T all_colors_value);

    T GetBlue() const;
    T GetGreen() const;
    T GetRed() const;

    Pixel<uint8_t> NormalizePixel();

    static Pixel<T> MultiplyPixelBy(const Pixel<T> &pixel, T mult);

    static Pixel<T> DividePixelBy(const Pixel<T> &pixel, T div);

private:
    T blue_;
    T green_;
    T red_;
};

// Implementation
// template <typename T>
// template <typename U>
// Pixel<U> Pixel<T>::MultiplyPixelBy(U mult) const {
//     // checking the usage of allowed types
//     Pixel<U> pixel_copy{static_cast<U>(blue_) * mult, static_cast<U>(green_) * mult, static_cast<U>(red_) * mult};

//     return pixel_copy;
// }

// template <typename T>
// template <typename U>
// Pixel<U> Pixel<T>::DividePixelBy(U div) const {
//     // checking the usage of allowed types
//     Pixel<U> pixel_copy{static_cast<U>(blue_) / div, static_cast<U>(green_) / div, static_cast<U>(red_) / div};

//     return pixel_copy;
// }

template <typename T>
// by default initialize pixel with zero num values for allowed numeric types
Pixel<T>::Pixel() : blue_(0), green_(0), red_(0) {
}

template <typename T>
Pixel<T>::Pixel(T blue, T green, T red) {
    this->blue_ = blue;
    this->green_ = green;
    this->red_ = red;
}

template <typename T>
template <typename U>
Pixel<T>::Pixel(const Pixel<U> &pixel)
    : Pixel<T>(static_cast<T>(pixel.GetBlue()), static_cast<T>(pixel.GetGreen()), static_cast<T>(pixel.GetRed())) {
}

template <typename T>
Pixel<T> &Pixel<T>::operator=(const Pixel<T> &other) {
    this->blue_ = other.blue_;
    this->green_ = other.green_;
    this->red_ = other.red_;
    return *this;
}

template <typename T>
Pixel<T> &Pixel<T>::operator+=(const Pixel<T> &other) {
    blue_ += other.blue_;
    green_ += other.green_;
    red_ += other.red_;
    return *this;
}

template <typename T>
Pixel<T> &Pixel<T>::operator-=(const Pixel<T> &other) {
    blue_ -= other.blue_;
    green_ -= other.green_;
    red_ -= other.red_;
    return *this;
}

template <typename T>
bool Pixel<T>::operator==(const Pixel<T> &other) const {
    return std::tie(blue_, green_, red_) == std::tie(other.blue_, other.green_, other.red_);
}

template <typename T>
void Pixel<T>::SetPixel(T blue, T green, T red) {
    this->blue_ = blue;
    this->green_ = green;
    this->red_ = red;
}

template <typename T>
void Pixel<T>::SetPixel(T all_colors_value) {
    this->blue_ = all_colors_value;
    this->green_ = all_colors_value;
    this->red_ = all_colors_value;
}

template <typename T>
auto Pixel<T>::GetBlue() const -> T {
    return blue_;
}

template <typename T>
auto Pixel<T>::GetGreen() const -> T {
    return green_;
}

template <typename T>
auto Pixel<T>::GetRed() const -> T {
    return red_;
}

template <typename T>
Pixel<uint8_t> Pixel<T>::NormalizePixel() {
    if constexpr (std::is_same_v<T, double> || std::is_same_v<T, long double>) {
        return Pixel<uint8_t>{
            static_cast<uint8_t>(std::round(std::clamp(static_cast<double>(blue_), static_cast<double>(0),
                                                       static_cast<double>(bmp24::utils::COLOR_CHANNEL_MAX_VALUE)))),
            static_cast<uint8_t>(std::round(std::clamp(static_cast<double>(green_), static_cast<double>(0),
                                                       static_cast<double>(bmp24::utils::COLOR_CHANNEL_MAX_VALUE)))),
            static_cast<uint8_t>(std::round(std::clamp(static_cast<double>(red_), static_cast<double>(0),
                                                       static_cast<double>(bmp24::utils::COLOR_CHANNEL_MAX_VALUE))))};
    }
    return Pixel<uint8_t>{static_cast<uint8_t>(std::clamp(static_cast<double>(blue_), static_cast<double>(0),
                                                          static_cast<double>(bmp24::utils::COLOR_CHANNEL_MAX_VALUE))),
                          static_cast<uint8_t>(std::clamp(static_cast<double>(green_), static_cast<double>(0),
                                                          static_cast<double>(bmp24::utils::COLOR_CHANNEL_MAX_VALUE))),
                          static_cast<uint8_t>(std::clamp(static_cast<double>(red_), static_cast<double>(0),
                                                          static_cast<double>(bmp24::utils::COLOR_CHANNEL_MAX_VALUE)))};
}

template <typename T>
Pixel<T> Pixel<T>::MultiplyPixelBy(const Pixel<T> &pixel, T mult) {
    return Pixel<T>{pixel.blue_ * mult, pixel.green_ * mult, pixel.red_ * mult};
}

template <typename T>
Pixel<T> Pixel<T>::DividePixelBy(const Pixel<T> &pixel, T div) {
    return Pixel<T>{pixel.blue_ / div, pixel.green_ / div, pixel.red_ / div};
}

#endif  // PIXEL_H