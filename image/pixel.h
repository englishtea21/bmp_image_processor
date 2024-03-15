#pragma once

#include <cstdint>

template <typename T = uint8_t>
class Pixel {
public:
    Pixel() = default;
    Pixel(T blue, T green, T red);

    void SetPixel(T blue, T green, T red);

    T GetBlue();
    T GetGreen();
    T GetRed();

    template <typename U>
    Pixel<U> NormalizePixel(const uint8_t max_color = 255);

    template <typename U>
    Pixel<T> MultiplyPixel(U mult);

    Pixel<T>& operator+=(const Pixel<T> other) const;

    Pixel<T>& operator-=(const Pixel<T>& other) const;

private:
    T blue_;
    T green_;
    T red_;
};
