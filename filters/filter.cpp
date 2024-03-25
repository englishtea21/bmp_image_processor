#include "filter.h"

namespace filters {
Crop::Crop(size_t crop_width, size_t crop_height) {
    this->crop_width_ = crop_width;
    this->crop_height_ = crop_height;
}

ImageBMP Crop::Apply(const ImageBMP &image) {
    size_t actual_crop_width = std::clamp(crop_width_, static_cast<size_t>(0), image.GetWidth());
    size_t actual_crop_height = std::clamp(crop_height_, static_cast<size_t>(0), image.GetHeight());

    std::vector<std::vector<Pixel<uint8_t>>> new_data(actual_crop_height);

    for (size_t i = 0; i < actual_crop_height; ++i) {
        std::vector<Pixel<uint8_t>> row(actual_crop_width);
        for (size_t j = 0; j < actual_crop_width; ++j) {
            row[j] = std::move(image.GetImagePixel(i + image.GetHeight() - actual_crop_height, j));
        }
        new_data[i] = std::move(row);
    }
    return {new_data};
}

Pixel<uint8_t> Grayscale::GetPixel(const ImageBMP &image, size_t y, size_t x) const {
    Pixel<double> tmp_pixel(std::move(image.GetImagePixel(y, x)));

    tmp_pixel.SetPixel(filters::utils::matrices::GRAYSCALE_COLOR_RATIO[0] * static_cast<double>(tmp_pixel.GetBlue()) +
                       filters::utils::matrices::GRAYSCALE_COLOR_RATIO[1] * static_cast<double>(tmp_pixel.GetGreen()) +
                       filters::utils::matrices::GRAYSCALE_COLOR_RATIO[2] * static_cast<double>(tmp_pixel.GetRed()));

    return tmp_pixel;
}

Pixel<uint8_t> Negative::GetPixel(const ImageBMP &image, size_t y, size_t x) const {
    Pixel<double> tmp_pixel(filters::utils::pixels::WHITE);
    tmp_pixel -= image.GetImagePixel(y, x);

    return tmp_pixel;
}

Sharpening::Sharpening() : ConvolutionalFilter(filters::utils::matrices::SHARPENNING) {
}

EdgeDetection::EdgeDetection(double threshold) : ConvolutionalFilter(filters::utils::matrices::EDGE_DETECTING) {
    this->threshold_ = threshold;
}

ImageBMP EdgeDetection::Apply(const ImageBMP &image) {
    Grayscale grayscaler;
    ImageBMP image_tmp = std::move(grayscaler.Apply(image));

    image_tmp = std::move(ConvolutionalFilter::Apply(std::move(image_tmp)));

    for (size_t i = 0; i < image_tmp.GetHeight(); ++i) {
        for (size_t j = 0; j < image_tmp.GetWidth(); ++j) {
            image_tmp.SetPixel(i, j,
                               image_tmp.GetImagePixel(i, j).GetBlue() > this->threshold_
                                   ? filters::utils::pixels::WHITE
                                   : filters::utils::pixels::BLACK);
        }
    }
    return image_tmp;
}

GaussianBlur::GaussianBlur(double sigma) {
    this->kernel_optimal_size_ = static_cast<size_t>(6 * sigma);
    this->kernel_optimal_size_ += 1 ? kernel_optimal_size_ % 2 == 0 : 0;
    this->sigma_square_ = sigma * sigma;
    this->gaussian_denominator_ = 2 * std::numbers::pi * this->sigma_square_;

    conv_matrix_ = std::move(Generate1DGaussianKernel());
}

ImageBMP GaussianBlur::Apply(const ImageBMP &image) {
    std::vector<std::vector<Pixel<double>>> new_data_tmp(image.GetHeight());

    // apply kernel horizontally
    for (size_t i = 0; i < image.GetHeight(); ++i) {
        std::vector<Pixel<double>> row(image.GetWidth());
        for (size_t j = 0; j < image.GetWidth(); ++j) {
            Pixel<double> pixel = GetPixelViaConvolution<uint8_t>(image.GetImagePixels(), i, j);
            row[j] = std::move(pixel);
        }
        new_data_tmp[i] = std::move(row);
    }

    // apply kernel vertically
    for (size_t i = 0; i < image.GetHeight(); ++i) {
        for (size_t j = 0; j < image.GetWidth(); ++j) {
            Pixel<double> pixel = GetPixelViaConvolution<double>(new_data_tmp, i, j, true);
            new_data_tmp[i][j] = std::move(pixel);
        }
    }

    // finally multiply each pixel by common coefficient and normalize it
    std::vector<std::vector<Pixel<uint8_t>>> new_data(image.GetHeight());

    for (size_t i = 0; i < image.GetHeight(); ++i) {
        std::vector<Pixel<uint8_t>> row(image.GetWidth());
        for (size_t j = 0; j < image.GetWidth(); ++j) {
            row[j] = new_data_tmp[i][j].DividePixelBy(this->gaussian_denominator_).NormalizePixel();
        }
        new_data[i] = std::move(row);
    }

    return {new_data};
}

std::vector<std::vector<double>> GaussianBlur::Generate1DGaussianKernel() const {
    std::vector<std::vector<double>> gaussian_kernel_1d;
    gaussian_kernel_1d.push_back(std::vector<double>(this->kernel_optimal_size_));
    for (size_t i = 0; i < this->kernel_optimal_size_ / 2; ++i) {
        gaussian_kernel_1d[0][i] =
            1 / std::exp(std::pow(this->kernel_optimal_size_ / 2 - i, 2) / (2 * this->sigma_square_));
        gaussian_kernel_1d[0][this->kernel_optimal_size_ - i - 1] = gaussian_kernel_1d[0][i];
    }
    gaussian_kernel_1d[0][this->kernel_optimal_size_ / 2] = 1;
    return gaussian_kernel_1d;
}

Pixelization::Pixelization(size_t window_size) {
    this->window_size_ = window_size;
}

ImageBMP Pixelization::Apply(const ImageBMP &image) {
    std::vector<std::vector<Pixel<uint8_t>>> new_data(image.GetHeight());

    for (size_t i = 0; i < image.GetHeight(); i += window_size_) {
        std::vector<Pixel<uint8_t>> row(image.GetWidth());
        for (size_t j = 0; j < image.GetWidth(); j += window_size_) {
            std::fill(row.begin() + j, row.begin() + std::min(j + window_size_, image.GetWidth()),
                      GetPixel(image, i, j));
        }
        // new_data[i] = std::move(row);
        std::fill(new_data.begin() + i, new_data.begin() + std::min(i + window_size_, image.GetHeight()), row);
    }
    return {new_data};
}

Pixel<uint8_t> Pixelization::GetPixel(const ImageBMP &image, size_t y, size_t x) const {
    std::vector<const Pixel<uint8_t> *> pixel_candidates;
    pixel_candidates.reserve(window_size_ * window_size_);
    for (size_t i = std::clamp(static_cast<int64_t>(y) - static_cast<int64_t>(window_size_) / 2,
                               static_cast<int64_t>(0), static_cast<int64_t>(image.GetHeight()) - 1);
         i < std::clamp(y + window_size_ / 2, static_cast<size_t>(0), image.GetHeight() - 1); i++) {
        pixel_candidates.push_back(&image.GetImagePixel(i, x));
    }

    // find median pixel by every color
    auto median_it = pixel_candidates.begin() + pixel_candidates.size() / 2;
    // red
    std::nth_element(pixel_candidates.begin(), median_it, pixel_candidates.end(),
                     [](auto lhs, auto rhs) { return lhs->GetRed() < rhs->GetRed(); });
    uint8_t median_red = (*median_it)->GetRed();
    // green
    std::nth_element(pixel_candidates.begin(), median_it, pixel_candidates.end(),
                     [](auto lhs, auto rhs) { return lhs->GetGreen() < rhs->GetGreen(); });
    uint8_t median_green = (*median_it)->GetGreen();
    // blue
    std::nth_element(pixel_candidates.begin(), median_it, pixel_candidates.end(),
                     [](auto lhs, auto rhs) { return lhs->GetBlue() < rhs->GetBlue(); });
    uint8_t median_blue = (*median_it)->GetBlue();

    return {median_blue, median_green, median_red};
}

Posterization::Posterization(uint8_t levels) : colors_quantization_(3) {
    levels_ = levels;
}

void Posterization::MakeColorsQuantization(const std::vector<uint8_t> &min_colors,
                                           const std::vector<uint8_t> &max_colors) {
    std::vector<uint8_t> quantization_steps(3);
    for (size_t i = 0; i < 3; i++) {
        quantization_steps[i] = (max_colors[i] - min_colors[i]) / levels_;
    }
    for (size_t j = 0; j < 3; j++) {
        for (size_t i = 0; i < levels_; i++) {
            colors_quantization_[j].push_back(min_colors[j] + static_cast<uint8_t>(i) * quantization_steps[j]);
        }
    }

    // for (size_t j = 0; j < 3; j++)
    // {
    //     colors_quantization_[j].push_back(max_colors[j]);
    // }
}

uint8_t Posterization::QuantizeColor(uint8_t channel_color, size_t color_num) const {
    auto lower =
        std::lower_bound(colors_quantization_[color_num].begin(), colors_quantization_[color_num].end(), channel_color);

    if (lower == colors_quantization_[color_num].end()) {
        return colors_quantization_[color_num][levels_ - 1];
    }

    return *lower;
}

Pixel<uint8_t> Posterization::GetPixel(const ImageBMP &image, size_t y, size_t x) const {
    const Pixel<uint8_t> &tmp_pixel = image.GetImagePixel(y, x);
    return {QuantizeColor(tmp_pixel.GetBlue(), 0), QuantizeColor(tmp_pixel.GetGreen(), 1),
            QuantizeColor(tmp_pixel.GetRed(), 2)};
}

ImageBMP Posterization::Apply(const ImageBMP &image) {
    // min and max color channels values in BGR format
    std::vector<uint8_t> min_colors(3, filters::utils::pixels::WHITE.GetRed());
    std::vector<uint8_t> max_colors(3, filters::utils::pixels::BLACK.GetRed());

    for (size_t i = 0; i < image.GetHeight(); i++) {
        // min color
        min_colors[0] = std::min(std::min_element(image.GetImagePixels()[i].begin(), image.GetImagePixels()[i].end(),
                                                  [](auto lhs, auto rhs) { return lhs.GetBlue() < rhs.GetBlue(); })
                                     ->GetBlue(),
                                 min_colors[0]);
        min_colors[1] = std::min(std::min_element(image.GetImagePixels()[i].begin(), image.GetImagePixels()[i].end(),
                                                  [](auto lhs, auto rhs) { return lhs.GetGreen() < rhs.GetGreen(); })
                                     ->GetGreen(),
                                 min_colors[1]);
        min_colors[2] = std::min(std::min_element(image.GetImagePixels()[i].begin(), image.GetImagePixels()[i].end(),
                                                  [](auto lhs, auto rhs) { return lhs.GetRed() < rhs.GetRed(); })
                                     ->GetRed(),
                                 min_colors[2]);

        // max colors
        max_colors[0] = std::max(std::max_element(image.GetImagePixels()[i].begin(), image.GetImagePixels()[i].end(),
                                                  [](auto lhs, auto rhs) { return lhs.GetBlue() < rhs.GetBlue(); })
                                     ->GetBlue(),
                                 min_colors[0]);
        max_colors[1] = std::max(std::max_element(image.GetImagePixels()[i].begin(), image.GetImagePixels()[i].end(),
                                                  [](auto lhs, auto rhs) { return lhs.GetGreen() < rhs.GetGreen(); })
                                     ->GetGreen(),
                                 min_colors[1]);
        max_colors[2] = std::max(std::max_element(image.GetImagePixels()[i].begin(), image.GetImagePixels()[i].end(),
                                                  [](auto lhs, auto rhs) { return lhs.GetRed() < rhs.GetRed(); })
                                     ->GetRed(),
                                 min_colors[0]);
    }

    MakeColorsQuantization(min_colors, max_colors);

    ImageBMP tmp_image = PixelwiseFilter::Apply(image);

    GaussianBlur blur(1);

    return blur.Apply(tmp_image);
}

std::unique_ptr<Filter> CreateFilter(const parser::Token &token) {

    const std::string &filter = token.name;
    if (filter == "-crop") {
        if (token.args.size() != 2) {
            throw std::invalid_argument(filters::utils::exceptions::INCORRECT_AMOUNT_OF_FILTER_ARGUMENTS + filter +
                                        "\"");
        }

        size_t width_crop;
        size_t height_crop;
        try {
            width_crop = std::stoi(token.args.at(0));
            height_crop = std::stoi(token.args.at(1));
        } catch (const std::invalid_argument &) {
            throw std::invalid_argument(filters::utils::exceptions::INVALID_ARGUMENTS_FOR_FILTER + filter + "\"" +
                                        ":\n " + token.args.at(0) + "\n" + token.args.at(1));
        } catch (const std::out_of_range &) {
            throw std::invalid_argument(filters::utils::exceptions::INVALID_ARGUMENT_TYPE_RANGE + filter + "\"");
        }

        auto ptr = std::make_unique<Crop>(width_crop, height_crop);
        return ptr;
    } else if (filter == "-gs") {
        if (!token.args.empty()) {
            throw std::invalid_argument(filters::utils::exceptions::INCORRECT_AMOUNT_OF_FILTER_ARGUMENTS + filter +
                                        "\"");
        }
        auto ptr = std::make_unique<Grayscale>();
        return ptr;
    } else if (filter == "-neg") {
        if (!token.args.empty()) {
            throw std::invalid_argument(filters::utils::exceptions::INCORRECT_AMOUNT_OF_FILTER_ARGUMENTS + filter +
                                        "\"");
        }
        auto ptr = std::make_unique<Negative>();
        return ptr;
    } else if (filter == "-sharp") {
        if (!token.args.empty()) {
            throw std::invalid_argument(filters::utils::exceptions::INCORRECT_AMOUNT_OF_FILTER_ARGUMENTS + filter +
                                        "\"");
        }
        auto ptr = std::make_unique<Sharpening>();
        return ptr;
    } else if (filter == "-edge") {
        if (token.args.size() != 1) {
            throw std::invalid_argument(filters::utils::exceptions::INCORRECT_AMOUNT_OF_FILTER_ARGUMENTS + filter +
                                        "\"");
        }

        double threshold;
        try {
            threshold = std::stod(token.args.at(0));
        } catch (const std::invalid_argument &) {
            throw std::invalid_argument(filters::utils::exceptions::INVALID_ARGUMENTS_FOR_FILTER + filter + "\"" +
                                        ":\n " + token.args.at(0) + "\n" + token.args.at(1));
        } catch (const std::out_of_range &) {
            throw std::invalid_argument(filters::utils::exceptions::INVALID_ARGUMENT_TYPE_RANGE + filter + "\"");
        }

        auto ptr = std::make_unique<EdgeDetection>(threshold);
        return ptr;
    } else if (filter == "-blur") {
        if (token.args.size() != 1) {
            throw std::invalid_argument(filters::utils::exceptions::INCORRECT_AMOUNT_OF_FILTER_ARGUMENTS + filter +
                                        "\"");
        }

        double sigma;
        try {
            sigma = std::stod(token.args.at(0));
        } catch (const std::invalid_argument &) {
            throw std::invalid_argument(filters::utils::exceptions::INVALID_ARGUMENTS_FOR_FILTER + filter + "\"" +
                                        ":\n " + token.args.at(0) + "\n" + token.args.at(1));
        } catch (const std::out_of_range &) {
            throw std::invalid_argument(filters::utils::exceptions::INVALID_ARGUMENT_TYPE_RANGE + filter + "\"");
        }

        auto ptr = std::make_unique<GaussianBlur>(sigma);
        return ptr;
    } else if (filter == "-pixel") {
        if (token.args.size() != 1) {
            throw std::invalid_argument(filters::utils::exceptions::INCORRECT_AMOUNT_OF_FILTER_ARGUMENTS + filter +
                                        "\"");
        }

        size_t window_size;
        try {
            window_size = std::stoi(token.args.at(0));
        } catch (const std::invalid_argument &) {
            throw std::invalid_argument(filters::utils::exceptions::INVALID_ARGUMENTS_FOR_FILTER + filter + "\"" +
                                        ":\n " + token.args.at(0) + "\n" + token.args.at(1));
        } catch (const std::out_of_range &) {
            throw std::invalid_argument(filters::utils::exceptions::INVALID_ARGUMENT_TYPE_RANGE + filter + "\"");
        }

        auto ptr = std::make_unique<Pixelization>(window_size);
        return ptr;
    } else if (filter == "-poster") {
        if (token.args.size() != 1) {
            throw std::invalid_argument(filters::utils::exceptions::INCORRECT_AMOUNT_OF_FILTER_ARGUMENTS + filter +
                                        "\"");
        }

        size_t levels;
        try {
            levels = std::stoi(token.args.at(0));
        } catch (const std::invalid_argument &) {
            throw std::invalid_argument(filters::utils::exceptions::INVALID_ARGUMENTS_FOR_FILTER + filter + "\"" +
                                        ":\n " + token.args.at(0) + "\n" + token.args.at(1));
        } catch (const std::out_of_range &) {
            throw std::invalid_argument(filters::utils::exceptions::INVALID_ARGUMENT_TYPE_RANGE + filter + "\"");
        }

        auto ptr = std::make_unique<Posterization>(levels);
        return ptr;
    } else {
        throw std::invalid_argument(filters::utils::exceptions::INVALID_ARGUMENT_TYPE_RANGE + filter + "\"");
    }
}
}  // namespace filters