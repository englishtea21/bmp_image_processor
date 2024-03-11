#pragma once

#include <cstdint>

struct Color {
    Color() = default;
    Color(uint8_t blue, uint8_t green, uint8_t red);

    uint8_t blue_;
    uint8_t green_;
    uint8_t red_;

    void SetColor(uint8_t blue, uint8_t green, uint8_t red);
};
