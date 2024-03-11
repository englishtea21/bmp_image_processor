#include "color.h"

Color::Color(uint8_t blue, uint8_t green, uint8_t red) {
    this->blue_ = blue;
    this->green_ = green;
    this->red_ = red;
}

void Color::SetColor(uint8_t blue, uint8_t green, uint8_t red) {
    this->blue_ = blue;
    this->green_ = green;
    this->red_ = red;
}