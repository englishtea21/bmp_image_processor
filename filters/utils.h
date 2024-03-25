#ifndef FILTER_UTILS_H
#define FILTER_UTILS_H

#include <stddef.h>
#include <vector>
#include "../image/pixel.h"

namespace filters::utils::matrices
{
    const std::vector<double> GRAYSCALE_COLOR_RATIO = {0.114, 0.587, 0.299};
    const std::vector<std::vector<double>> SHARPENNING = {
        {0, -1, 0},
        {-1, 5, -1},
        {0, -1, 0}};
    const std::vector<std::vector<double>> EDGE_DETECTING = {
        {0, -1, 0},
        {-1, 4, -1},
        {0, -1, 0}};
} // namespace filters::utils::matrices

namespace filters::utils::pixels
{
    const Pixel<uint8_t> WHITE(255, 255, 255);
    const Pixel<uint8_t> BLACK(0, 0, 0);
} // namespace filters::utils::pixels

namespace filters::utils::exceptions
{
    const std::string INCORRECT_AMOUNT_OF_FILTER_ARGUMENTS = "Not exact amount of arguments for filter \"";
    const std::string INVALID_ARGUMENTS_FOR_FILTER = "Invalid arguments for filter \"";
    const std::string INVALID_ARGUMENT_TYPE_RANGE = "Invalid argument type range for filter \"";
    const std::string UNKNOWN_FILTER = "Unknown filter: \"";
} // namespace filters::utils::exceptions

#endif // FILTER_UTILS_H