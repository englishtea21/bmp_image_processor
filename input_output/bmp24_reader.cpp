#include "bmp24_reader.h"

input_output::ReaderBmp24::ReaderBmp24(const std::string &filename) {
    path_ = filename;
}

ImageBmp input_output::ReaderBmp24::Read() {
    unsigned char bmp24_header_bytes[bmp24::utils::HEADER_SIZE];
    unsigned char bmp24_dib_header_bytes[bmp24::utils::DIB_HEADER_SIZE];

    std::ifstream img_file;
    img_file.open(path_, std::ios::in | std::ios::binary);
    if (!img_file.is_open()) {
        throw std::invalid_argument(bmp24::utils::exceptions::FILE_OPENNING_ERROR + path_);
    }
    if (errno == EACCES) {
        throw std::invalid_argument(bmp24::utils::exceptions::FILE_PERMISSIONS_ERROR + path_);
    }
    img_file.read(reinterpret_cast<char *>(bmp24_header_bytes), bmp24::utils::HEADER_SIZE);
    // implicit header checking
    bmp24::Bmp24Header bmp_header(std::move(bmp24_header_bytes), path_);
    try {
        img_file.read(reinterpret_cast<char *>(bmp24_dib_header_bytes), bmp24::utils::DIB_HEADER_SIZE);
        bmp24::Bmp24DibHeader bmp24_dib_header(std::move(bmp24_dib_header_bytes));

        size_t image_width = bmp24_dib_header.GetWidth();
        size_t image_height = bmp24_dib_header.GetHeight();

        std::vector<std::vector<Pixel<uint8_t>>> data(image_height);
        std::vector<Pixel<uint8_t>> row(image_width);

        const int padding_size = bmp24::GetPaddingSize(image_width);

        for (size_t i = 0; i < image_height; ++i) {
            for (size_t j = 0; j < image_width; ++j) {
                unsigned char pix[bmp24::utils::BYTES_PER_PIXEL];
                img_file.read(reinterpret_cast<char *>(pix), bmp24::utils::BYTES_PER_PIXEL);
                row[j].SetPixel(static_cast<uint8_t>(pix[0]), static_cast<uint8_t>(pix[1]),
                                static_cast<uint8_t>(pix[2]));
            }
            data[i] = row;
            img_file.ignore(padding_size);
        }
        img_file.close();

        return {data};
    } catch (std::exception &e) {
        throw std::invalid_argument(bmp24::utils::exceptions::FILE_READING_ERROR + path_ + "\n" +
                                    static_cast<std::string>(e.what()));
    }
}