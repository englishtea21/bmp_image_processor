#include "writer.h"

input_output::Writer::Writer(std::string path) : path_(std::move(path)) {
}

template <typename T>
void input_output::Writer::NumToBytes(T num, unsigned char* bytes) {
    // записываем в младший разряд (самый правый)
    *bytes = num;
    for (size_t i = 0; i < image::utils::SHIFT_BITS.size(); ++i) {
        *(bytes + i + 1) = (num >> image::utils::SHIFT_BITS[i]) & 0xff;
    }
}

void input_output::Writer::WriteBMPHeader(unsigned char* bmp_header, size_t size) {
    bmp_header[image::utils::FORMAT_FIRST_POSITION] = image::utils::BMP_SIGNAME[image::utils::FORMAT_FIRST_POSITION];
    bmp_header[image::utils::FORMAT_SECOND_POSITION] = image::utils::BMP_SIGNAME[image::utils::FORMAT_SECOND_POSITION];
    NumToBytes(size, bmp_header + image::utils::PIXS_ARRAY_OFFSET_POSITION);
    bmp_header[image::utils::PIXS_ARRAY_OFFSET_POSITION] =
        image::utils::BMP_HEADER_SIZE + image::utils::DIB_HEADER_SIZE;
}

void input_output::Writer::Write(const ImageBMP& image_bmp) {
    std::ofstream img_file;
    img_file.open(path_, std::ios::out | std::ios::binary);
    if (!img_file.is_open()) {
        throw std::invalid_argument(std::string("Cannot open file: ") + path_);
    }
    if (errno == EACCES) {
        throw std::invalid_argument(std::string("Permission denied to file: ") + path_);
    }
    const size_t size = image::utils::BMP_HEADER_SIZE + image::utils::DIB_HEADER_SIZE +
                        image_bmp.GetWidth() * image_bmp.GetWidth() * image::utils::BYTES_PER_PIXEL +
                        GetPaddingSize(image_bmp.GetWidth()) * image_bmp.GetHeight();

    unsigned char bmp_header[image::utils::BMP_HEADER_SIZE] = {};
    std::fill(bmp_header, bmp_header + image::utils::BMP_HEADER_SIZE, 0);
    WriteBMPHeader(bmp_header, size);

    unsigned char dib_header[image::utils::DIB_HEADER_SIZE] = {};
    std::fill(dib_header, dib_header + image::utils::DIB_HEADER_SIZE, 0);
    WriteDIBHeader(dib_header, image_bmp.GetWidth(), image_bmp.GetHeight());

    img_file.write(reinterpret_cast<char*>(bmp_header), image::utils::BMP_HEADER_SIZE);
    img_file.write(reinterpret_cast<char*>(dib_header), image::utils::DIB_HEADER_SIZE);

    unsigned char empty_pix[image::utils::BYTES_PER_PIXEL] = {0, 0, 0};
    for (size_t i = 0; i < image_bmp.GetHeight(); i++) {
        for (size_t j = 0; j < image_bmp.GetWidth(); j++) {
            unsigned char blue = static_cast<unsigned char>(image_bmp.GeImagePixel(i, j).blue_);
            unsigned char green = static_cast<unsigned char>(image_bmp.GeImagePixel(i, j).green_);
            unsigned char red = static_cast<unsigned char>(image_bmp.GeImagePixel(i, j).red_);
            unsigned char pix[] = {blue, green, red};
            img_file.write(reinterpret_cast<char*>(pix), image::utils::BYTES_PER_PIXEL);
        }
        img_file.write(reinterpret_cast<char*>(empty_pix), GetPaddingSize(image_bmp.GetWidth()));
    }
}