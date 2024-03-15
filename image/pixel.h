#pragma once

#include <cstdint>

template <typename T = uint8_t>
class Pixel {
public:
    Pixel() = default;
    Pixel(T blue, T green, T red);

    void SetPixel(T blue, T green, T red);

    template <typename U>
    Pixel<U> NormalizePixel(const uint8_t max_color = 255);

    template <typename U>
    Pixel<T> MultiplyPixel(U mult);

    Pixel<T>& operator+=(Pixel<T>& other);

    Pixel<T>& operator-(const Pixel<T>& lhs, const Pixel<T>& rhs);

private:
    T blue_;
    T green_;
    T red_;
};
