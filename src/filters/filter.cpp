#include "filter.h"

namespace filters {

Crop::Crop(size_t crop_width, size_t crop_height) {
    this->crop_width_ = crop_width;
    this->crop_height_ = crop_height;
}

ImageBmp Crop::Apply(const ImageBmp &image) {
    size_t actual_crop_width = std::clamp(crop_width_, static_cast<size_t>(0), image.GetWidth());
    size_t actual_crop_height = std::clamp(crop_height_, static_cast<size_t>(0), image.GetHeight());

    std::vector<std::vector<Pixel<uint8_t>>> new_data(actual_crop_height);

    for (size_t i = 0; i < actual_crop_height; ++i) {
        std::vector<Pixel<uint8_t>> row(actual_crop_width);
        for (size_t j = 0; j < actual_crop_width; ++j) {
            row[j] = image.GetImagePixel(i + image.GetHeight() - actual_crop_height, j);
        }
        new_data[i] = std::move(row);
    }
    return ImageBmp{new_data};
}

Pixel<uint8_t> Grayscale::GetPixel(const ImageBmp &image, size_t y, size_t x) const {
    Pixel<double> tmp_pixel(image.GetImagePixel(y, x));

    tmp_pixel.SetPixel(
        std::round(filters::utils::matrices::GRAYSCALE_COLOR_RATIO[0] * static_cast<double>(tmp_pixel.GetBlue()) +
                   filters::utils::matrices::GRAYSCALE_COLOR_RATIO[1] * static_cast<double>(tmp_pixel.GetGreen()) +
                   filters::utils::matrices::GRAYSCALE_COLOR_RATIO[2] * static_cast<double>(tmp_pixel.GetRed())));

    return tmp_pixel.NormalizePixel();
}

Pixel<uint8_t> Negative::GetPixel(const ImageBmp &image, size_t y, size_t x) const {
    Pixel<double> tmp_pixel(filters::utils::pixels::WHITE);
    tmp_pixel -= Pixel<double>(image.GetImagePixel(y, x));

    return Pixel<uint8_t>(tmp_pixel);
}

Sharpening::Sharpening() : ConvolutionalFilter(filters::utils::matrices::SHARPENNING, Pixel<double>::MultiplyPixelBy) {
}

EdgeDetection::EdgeDetection(double threshold)
    : ConvolutionalFilter(filters::utils::matrices::EDGE_DETECTING, Pixel<double>::MultiplyPixelBy) {
    this->threshold_ = threshold * bmp24::utils::COLOR_CHANNEL_MAX_VALUE;
}

ImageBmp EdgeDetection::Apply(const ImageBmp &image) {
    Grayscale grayscaler;
    ImageBmp image_tmp = grayscaler.Apply(image);

    image_tmp = ConvolutionalFilter::Apply(std::move(image_tmp));

    for (size_t i = 0; i < image_tmp.GetHeight(); ++i) {
        for (size_t j = 0; j < image_tmp.GetWidth(); ++j) {
            image_tmp.SetPixel(i, j,
                               (image_tmp.GetImagePixel(i, j).GetBlue() > threshold_ ? filters::utils::pixels::WHITE
                                                                                     : filters::utils::pixels::BLACK));
        }
    }

    return image_tmp;
}

GaussianBlur::GaussianBlur(double sigma) : ConvolutionalFilter(Pixel<double>::DividePixelBy) {
    this->kernel_optimal_size_ = static_cast<size_t>(
        std::ceil(static_cast<double>(filters::utils::gaussian_blur::ENOUGH_KERNEL_SIZE_IN_SIGMAS) * sigma));
    this->kernel_optimal_size_ += kernel_optimal_size_ % 2 == 0 ? 1 : 0;
    this->sigma_square_ = sigma * sigma;
    this->gaussian_denominator_ = 2 * std::numbers::pi * sigma * sigma;

    conv_matrix_ = Generate1DGaussianKernel();
}

ImageBmp GaussianBlur::Apply(const ImageBmp &image) {
    std::vector<std::vector<Pixel<double>>> new_data_tmp(image.GetHeight());

    // apply kernel horizontally
    for (size_t i = 0; i < image.GetHeight(); ++i) {
        std::vector<Pixel<double>> row;
        row.reserve(image.GetWidth());
        for (size_t j = 0; j < image.GetWidth(); ++j) {
            row.push_back(GetPixelViaConvolution<uint8_t>(image.GetImagePixels(), i, j, true));
        }
        new_data_tmp[i] = std::move(row);
    }

    // apply kernel vertically
    for (size_t i = 0; i < image.GetHeight(); ++i) {
        for (size_t j = 0; j < image.GetWidth(); ++j) {
            new_data_tmp[i][j] = GetPixelViaConvolution<double>(new_data_tmp, j, i, false);
        }
    }

    // finally multiply each pixel by common coefficient and normalize it
    std::vector<std::vector<Pixel<uint8_t>>> new_data(image.GetHeight());

    for (size_t i = 0; i < image.GetHeight(); ++i) {
        std::vector<Pixel<uint8_t>> row;
        row.reserve(image.GetWidth());
        for (size_t j = 0; j < image.GetWidth(); ++j) {
            row.push_back(
                Pixel<double>::DividePixelBy(new_data_tmp[i][j], this->gaussian_denominator_).NormalizePixel());
        }
        new_data[i] = std::move(row);
    }

    return ImageBmp{new_data};
}

std::vector<std::vector<double>> GaussianBlur::Generate1DGaussianKernel() const {
    std::vector<std::vector<double>> gaussian_kernel_1d;
    gaussian_kernel_1d.push_back(std::vector<double>(this->kernel_optimal_size_));
    for (size_t i = 0; i < this->kernel_optimal_size_ / 2; ++i) {
        gaussian_kernel_1d[0][i] =
            std::exp(std::pow(this->kernel_optimal_size_ / 2 - i, 2) / (2 * this->sigma_square_));
        gaussian_kernel_1d[0][this->kernel_optimal_size_ - i - 1] = gaussian_kernel_1d[0][i];
    }
    gaussian_kernel_1d[0][this->kernel_optimal_size_ / 2] = 1;
    return gaussian_kernel_1d;
}

Pixelization::Pixelization(size_t window_size) {
    this->window_size_ = window_size;
}

ImageBmp Pixelization::Apply(const ImageBmp &image) {
    std::vector<std::vector<Pixel<uint8_t>>> new_data(image.GetHeight());

    for (size_t i = 0; i < image.GetHeight(); i += window_size_) {
        std::vector<Pixel<uint8_t>> row(image.GetWidth());
        for (size_t j = 0; j < image.GetWidth(); j += window_size_) {
            std::fill(std::next(row.begin(), static_cast<std::ptrdiff_t>(j)),
                      std::next(row.begin(), static_cast<std::ptrdiff_t>(std::min(j + window_size_, image.GetWidth()))),
                      GetPixel(image, i, j));
        }
        // new_data[i] = std::move(row);
        std::fill(
            std::next(new_data.begin(), static_cast<std::ptrdiff_t>(i)),
            std::next(new_data.begin(), static_cast<std::ptrdiff_t>(std::min(i + window_size_, image.GetHeight()))),
            std::move(row));
    }
    return ImageBmp{new_data};
}

Pixel<uint8_t> Pixelization::GetPixel(const ImageBmp &image, size_t y, size_t x) const {
    std::vector<const Pixel<uint8_t> *> pixel_candidates;
    pixel_candidates.reserve(window_size_ * window_size_);
    for (size_t i = std::clamp(static_cast<int64_t>(y) - static_cast<int64_t>(window_size_) / 2,
                               static_cast<int64_t>(0), static_cast<int64_t>(image.GetHeight()) - 1);
         i < std::clamp(y + window_size_ / 2, static_cast<size_t>(0), image.GetHeight() - 1); i++) {
        pixel_candidates.push_back(&image.GetImagePixel(i, x));
    }

    // find median pixel by every color
    auto median_it = std::next(pixel_candidates.begin(), static_cast<std::ptrdiff_t>(pixel_candidates.size() / 2));
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

    return Pixel<uint8_t>{median_blue, median_green, median_red};
}

Posterization::Posterization(uint8_t levels) : colors_quantization_(bmp24::utils::COLOR_CHANNELS_AMOUNT) {
    levels_ = levels;
}

void Posterization::MakeColorsQuantization(const ImageBmp &image) {
    size_t amount_of_pixels = image.GetHeight() * image.GetWidth();

    std::vector<const Pixel<uint8_t> *> image_pixels;
    image_pixels.reserve(amount_of_pixels);
    for (size_t i = 0; i < image.GetHeight(); i++) {
        for (size_t j = 0; j < image.GetWidth(); j++) {
            image_pixels.push_back(&image.GetImagePixel(i, j));
        }
    }

    std::vector<uint8_t (Pixel<uint8_t>::*)() const> colors_getters{
        &Pixel<uint8_t>::GetBlue,
        &Pixel<uint8_t>::GetGreen,
        &Pixel<uint8_t>::GetRed,
    };
    size_t curr_color_percentile = 0;

    for (size_t i = 0; i < bmp24::utils::COLOR_CHANNELS_AMOUNT; i++) {
        for (size_t level_num = 0; level_num < levels_; level_num++) {
            curr_color_percentile =
                std::clamp(amount_of_pixels / levels_ * (level_num + 1), static_cast<size_t>(0), amount_of_pixels - 1);
            std::nth_element(
                image_pixels.begin(),
                std::next(image_pixels.begin(), static_cast<std::ptrdiff_t>(curr_color_percentile)), image_pixels.end(),
                [&](auto lhs, auto rhs) { return (lhs->*colors_getters[i])() < (rhs->*colors_getters[i])(); });
            colors_quantization_[i].push_back((image_pixels[curr_color_percentile]->*colors_getters[i])());
        };
    }
}

uint8_t Posterization::QuantizeColor(uint8_t channel_color, size_t color_num) const {
    auto lower =
        std::upper_bound(colors_quantization_[color_num].begin(), colors_quantization_[color_num].end(), channel_color);

    if (lower == colors_quantization_[color_num].end()) {
        return colors_quantization_[color_num][levels_ - 1];
    }

    return *lower;
}

Pixel<uint8_t> Posterization::GetPixel(const ImageBmp &image, size_t y, size_t x) const {
    const Pixel<uint8_t> &tmp_pixel = image.GetImagePixel(y, x);
    return {QuantizeColor(tmp_pixel.GetBlue(), 0), QuantizeColor(tmp_pixel.GetGreen(), 1),
            QuantizeColor(tmp_pixel.GetRed(), 2)};
}

ImageBmp Posterization::Apply(const ImageBmp &image) {
    MakeColorsQuantization(image);

    // return GaussianBlur(0.5).Apply(PixelwiseFilter::Apply(image));
    return PixelwiseFilter::Apply(image);
}

std::unique_ptr<Filter> CreateFilter(const parser::Token &token) {

    const std::string &filter = token.name;
    if (filter == "-crop") {
        if (token.args.size() != 2) {
            throw std::invalid_argument(filters::utils::exceptions::INCORRECT_AMOUNT_OF_FILTER_ARGUMENTS + filter +
                                        "\"");
        }

        size_t width_crop = 0;
        size_t height_crop = 0;
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

        double threshold = 0;
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

        double sigma = 0;
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

        size_t window_size = 0;
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

        size_t levels = 0;
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