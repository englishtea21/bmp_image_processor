#ifndef FILTER_H
#define FILTER_H

#include <algorithm>
#include <cmath>
#include <memory>
#include <numbers>
#include <stdexcept>
#include <functional>
#include "utils.h"
#include "../input_output/utils.h"
#include "../image/image.h"
#include "../parser/parser.h"

namespace filters {
class Filter {
public:
    Filter() = default;
    virtual ~Filter() = default;
    virtual ImageBmp Apply(const ImageBmp &image) = 0;
};

class PixelwiseFilter : public Filter {
public:
    PixelwiseFilter() = default;
    ImageBmp Apply(const ImageBmp &image) override {
        std::vector<std::vector<Pixel<uint8_t>>> new_data(image.GetHeight());

        for (size_t i = 0; i < image.GetHeight(); ++i) {
            std::vector<Pixel<uint8_t>> row(image.GetWidth());
            for (size_t j = 0; j < image.GetWidth(); ++j) {
                row[j] = std::move(GetPixel(image, i, j));
            }
            new_data[i] = std::move(row);
        }
        return ImageBmp{new_data};
    }

protected:
    ~PixelwiseFilter() override = default;
    virtual Pixel<uint8_t> GetPixel(const ImageBmp &image, size_t y, size_t x) const = 0;
};

template <typename ComputationType>
class ConvolutionalFilter : public Filter {

protected:
    using PixelOperation =
        std::function<Pixel<ComputationType>(const Pixel<ComputationType> &pixel, ComputationType conv_matrix_value)>;

    std::vector<std::vector<ComputationType>> conv_matrix_;

    PixelOperation pixel_operation_;
    ConvolutionalFilter() = delete;
    ~ConvolutionalFilter() override = default;
    explicit ConvolutionalFilter(std::vector<std::vector<ComputationType>> conv_matrix,
                                 PixelOperation pixel_operation = Pixel<ComputationType>::MultiplyPixelBy)
        : conv_matrix_(conv_matrix), pixel_operation_(pixel_operation) {
    }
    explicit ConvolutionalFilter(PixelOperation pixel_operation = Pixel<ComputationType>::MultiplyPixelBy)
        : pixel_operation_(pixel_operation) {
    }

    template <typename InputType>
    Pixel<ComputationType> GetPixelViaConvolution(const std::vector<std::vector<Pixel<InputType>>> &image_bmp_matrix,
                                                  size_t y, size_t x, bool transpose_matrix = false) const {
        Pixel<ComputationType> pixel = Pixel<ComputationType>{filters::utils::pixels::BLACK};

        if (!transpose_matrix) {
            for (size_t i = 0; i < conv_matrix_.size(); ++i) {
                for (size_t j = 0; j < conv_matrix_.front().size(); ++j) {
                    size_t clamped_y = static_cast<size_t>(
                        std::clamp(static_cast<int64_t>(i + y) - static_cast<int64_t>(conv_matrix_.size() / 2),
                                   static_cast<int64_t>(0), static_cast<int64_t>(image_bmp_matrix.size()) - 1));
                    size_t clamped_x = static_cast<size_t>(
                        std::clamp(static_cast<int64_t>(j + x) - static_cast<int64_t>(conv_matrix_.size() / 2),
                                   static_cast<int64_t>(0), static_cast<int64_t>(image_bmp_matrix.front().size()) - 1));
                    Pixel<ComputationType> tmp_pixel{image_bmp_matrix[clamped_y][clamped_x]};

                    // pixel += tmp_pixel.MultiplyPixelBy(conv_matrix_[i][j]);
                    pixel += pixel_operation_(tmp_pixel, conv_matrix_[i][j]);
                }
            }
        } else {
            for (size_t j = 0; j < conv_matrix_.size(); ++j) {
                for (size_t i = 0; i < conv_matrix_.front().size(); ++i) {
                    size_t clamped_y = static_cast<size_t>(
                        std::clamp(static_cast<int64_t>(i + y) - static_cast<int64_t>(conv_matrix_.size() / 2),
                                   static_cast<int64_t>(0), static_cast<int64_t>(image_bmp_matrix.size()) - 1));
                    size_t clamped_x = static_cast<size_t>(
                        std::clamp(static_cast<int64_t>(j + x) - static_cast<int64_t>(conv_matrix_.size() / 2),
                                   static_cast<int64_t>(0), static_cast<int64_t>(image_bmp_matrix.front().size()) - 1));
                    Pixel<ComputationType> tmp_pixel{image_bmp_matrix[clamped_x][clamped_y]};

                    pixel += pixel_operation_(tmp_pixel, conv_matrix_[j][i]);
                }
            }
        }

        return pixel;
    }

public:
    ImageBmp Apply(const ImageBmp &image) override {
        std::vector<std::vector<Pixel<uint8_t>>> new_data(image.GetHeight());

        for (size_t i = 0; i < image.GetHeight(); ++i) {
            std::vector<Pixel<uint8_t>> row(image.GetWidth());
            for (size_t j = 0; j < image.GetWidth(); ++j) {
                row[j] = std::move(GetPixelViaConvolution(image.GetImagePixels(), i, j).NormalizePixel());
            }
            new_data[i] = std::move(row);
        }
        return ImageBmp{new_data};
    }
};

class Crop : public Filter {
public:
    Crop(size_t crop_width, size_t crop_height);
    ImageBmp Apply(const ImageBmp &image) override;

private:
    size_t crop_width_;
    size_t crop_height_;
};

class Grayscale : public PixelwiseFilter {
public:
    // ImageBmp Apply(const ImageBmp &image) const override;
    Grayscale() = default;

protected:
    Pixel<uint8_t> GetPixel(const ImageBmp &image, size_t y, size_t x) const override;
};

class Negative : public PixelwiseFilter {
protected:
    // ImageBmp Apply(const ImageBmp &image) const override;
    Pixel<uint8_t> GetPixel(const ImageBmp &image, size_t y, size_t x) const override;
};

class Sharpening : public ConvolutionalFilter<double> {
public:
    // ImageBmp Apply(const ImageBmp &image) const override;
    Sharpening();
};

class EdgeDetection : public ConvolutionalFilter<double> {
public:
    explicit EdgeDetection(double threshold);
    ImageBmp Apply(const ImageBmp &image) override;

private:
    double threshold_;
};

class GaussianBlur : public ConvolutionalFilter<double> {
public:
    explicit GaussianBlur(double sigma);
    ImageBmp Apply(const ImageBmp &image) override;

protected:
private:
    size_t kernel_optimal_size_;
    double sigma_square_;
    double gaussian_denominator_;

    // generate 1d gaussian kernel in horizontal form
    std::vector<std::vector<double>> Generate1DGaussianKernel() const;
};

// Based on averaging the pixels variation using median among pixels set
class Pixelization : public PixelwiseFilter {
public:
    explicit Pixelization(size_t window_size);
    ImageBmp Apply(const ImageBmp &image) override;
    Pixel<uint8_t> GetPixel(const ImageBmp &image, size_t y, size_t x) const override;

private:
    size_t window_size_;
};

// Based on color quantization by percentiles
class Posterization : public PixelwiseFilter {
public:
    explicit Posterization(uint8_t levels);
    Pixel<uint8_t> GetPixel(const ImageBmp &image, size_t y, size_t x) const override;
    ImageBmp Apply(const ImageBmp &image) override;

private:
    void MakeColorsQuantization(const ImageBmp &image);
    uint8_t QuantizeColor(uint8_t channel_color, size_t color_num) const;

    uint8_t levels_;
    // color quantization levels for each color in BGR format
    std::vector<std::vector<uint8_t>> colors_quantization_;
};

std::unique_ptr<Filter> CreateFilter(const parser::Token &token);
}  // namespace filters

#endif  // FILTER_H