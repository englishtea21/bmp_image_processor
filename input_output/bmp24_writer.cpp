#include "bmp24_writer.h"

namespace input_output {

WriterBmp24::WriterBmp24(std::string path) : path_(std::move(path)) {
}

void WriteBMPHeader(unsigned char *bmp_header, size_t size) {
    bmp_header[bmp24::utils::FORMAT_FIRST_POSITION] = bmp24::utils::BMP_SIGN[0];
    bmp_header[bmp24::utils::FORMAT_SECOND_POSITION] = bmp24::utils::BMP_SIGN[1];
    bmp24::NumToBytes(size, bmp_header + bmp24::utils::FILE_SIZE_POSITION);
    bmp_header[bmp24::utils::PIXS_ARRAY_OFFSET_POSITION] = bmp24::utils::HEADER_SIZE + bmp24::utils::DIB_HEADER_SIZE;
}

void WriteDIBHeader(unsigned char *dib_header, size_t width, size_t height) {
    bmp24::NumToBytes(bmp24::utils::DIB_HEADER_SIZE, dib_header + bmp24::utils::DIB_HEADER_SIZE_POSITION);
    bmp24::NumToBytes(width, dib_header + bmp24::utils::WIDTH_POSITION);
    bmp24::NumToBytes(height, dib_header + bmp24::utils::HEIGHT_POSITION);
    dib_header[bmp24::utils::PLANES_POSITION] = bmp24::utils::DEFAULT_NUMBER_OF_PLANES;
    dib_header[bmp24::utils::BITS_PER_PIXEL_POSITION] = bmp24::utils::BITS_PER_PIXEL;
    bmp24::NumToBytes(width * height * bmp24::utils::BYTES_PER_PIXEL,
                      dib_header + bmp24::utils::IMAGE_DATA_SIZE_POSITION);
}

void WriterBmp24::Write(const ImageBmp &image_bmp) {
    std::ofstream img_file;
    img_file.open(path_, std::ios::out | std::ios::binary | std::ios::trunc);
    if (!img_file.is_open()) {
        throw std::invalid_argument(bmp24::utils::exceptions::FILE_OPENNING_ERROR + path_);
    }
    if (errno == EACCES) {
        throw std::invalid_argument(bmp24::utils::exceptions::FILE_PERMISSIONS_ERROR + path_);
    }

    bmp24::Bmp24Header bmp24_header{image_bmp.GetWidth(), image_bmp.GetHeight()};
    bmp24::Bmp24DibHeader bmp24_dib_header{image_bmp.GetWidth(), image_bmp.GetHeight()};

    const size_t padding_size = bmp24::GetPaddingSize(image_bmp.GetWidth());

    img_file.write(reinterpret_cast<char *>(bmp24_header.GetBmp24HeaderBytes()), bmp24::utils::HEADER_SIZE);
    img_file.write(reinterpret_cast<char *>(bmp24_dib_header.GetBmp24DibHeaderBytes()), bmp24::utils::DIB_HEADER_SIZE);

    unsigned char empty_pix[bmp24::utils::BYTES_PER_PIXEL] = {0, 0, 0};
    for (size_t i = 0; i < image_bmp.GetHeight(); ++i) {
        for (size_t j = 0; j < image_bmp.GetWidth(); ++j) {
            uint8_t pix[] = {image_bmp.GetImagePixel(i, j).GetBlue(), image_bmp.GetImagePixel(i, j).GetGreen(),
                             image_bmp.GetImagePixel(i, j).GetRed()};
            img_file.write(reinterpret_cast<char *>(pix), bmp24::utils::BYTES_PER_PIXEL);
        }
        img_file.write(reinterpret_cast<char *>(empty_pix), padding_size);
    }
    img_file.close();
}
}  // namespace input_output