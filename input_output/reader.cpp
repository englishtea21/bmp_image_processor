#include "reader.h"
#include "utils.h"

input_output::Reader::Reader(const std::string& filename) {
    path_ = filename;
}

const size_t input_output::GetPaddingSize(size_t width) {
    return (image::utils::PADDING_SIZE - (width * image::utils::BYTES_PER_PIXEL) % image::utils::PADDING_SIZE) %
           image::utils::PADDING_SIZE;
}

size_t input_output::Reader::BytesToNum(unsigned char* bytes) {
    size_t number = *bytes;
    for (size_t i = 0; i < image::utils::SHIFT_BITS.size(); ++i) {
        number += *(bytes + i + 1) << image::utils::SHIFT_BITS[i];
    }
    return number;
}

ImageBMP input_output::Reader::Read() {
    unsigned char bmp_header[image::utils::BMP_HEADER_SIZE];
    unsigned char dib_header[image::utils::DIB_HEADER_SIZE];

    std::ifstream img_file;
    img_file.open(path_, std::ios::in | std::ios::binary);
    if (!img_file.is_open()) {
        throw std::invalid_argument(std::string("Cannot open file: ") + path_);
    }
    if (errno == EACCES) {
        throw std::invalid_argument(std::string("Permission denied to file: ") + path_);
    }
    img_file.read(reinterpret_cast<char*>(bmp_header), image::utils::BMP_HEADER_SIZE);
    if (bmp_header[0] != image::utils::BMP_SIGNAME[0] || bmp_header[1] != image::utils::BMP_SIGNAME[1]) {
        throw std::invalid_argument(std::string("Not a BMP file: ") + path_);
    }

    try {
        img_file.read(reinterpret_cast<char*>(dib_header), image::utils::DIB_HEADER_SIZE);
        size_t width = BytesToNum(dib_header + image::utils::WIDTH_POSITION);
        size_t height = BytesToNum(dib_header + image::utils::HEIGHT_POSITION);
        ImageBMP image_bmp(width, height);
        std::vector<std::vector<Color>> data(height);
        std::vector<Color> row(width);
        for (size_t i = 0; i < height; ++i) {
            for (size_t j = 0; j < width; ++j) {
                unsigned char pix[image::utils::BITS_PER_PIXEL];
                img_file.read(reinterpret_cast<char*>(pix), image::utils::BYTES_PER_PIXEL);
                row[j].SetPixel(static_cast<T>(pix[0]), static_cast<T>(pix[1]),
                                static_cast<T>(pix[2]));
            }
            data[i] = row;
            img_file.ignore(GetPaddingSize(width));
        }
        image_bmp = ImageBMP(data);
        img_file.close();
        return image_bmp;

    } catch (std::exception& e) {
        throw std::invalid_argument("Error reading file: " + path_ + "\n" + static_cast<std::string>(e.what()));
    }
}