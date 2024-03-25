#include "bmp24.h"

namespace bmp24
{

    template <typename T>
    void NumToBytes(T num, unsigned char *bytes)
    {
        // записываем в младший разряд (самый правый)
        *bytes = num;
        for (size_t i = 0; i < bmp24::utils::SHIFT_BITS.size(); ++i)
        {
            *(bytes + i + 1) = (num >> bmp24::utils::SHIFT_BITS[i]) & 0xff;
        }
    }

    size_t BytesToNum(const unsigned char *bytes)
    {
        size_t number = *bytes;
        for (size_t i = 0; i < bmp24::utils::SHIFT_BITS.size(); ++i)
        {
            number += *(bytes + i + 1) << bmp24::utils::SHIFT_BITS[i];
        }
        return number;
    }

    const size_t GetPaddingSize(size_t width)
    {
        return (bmp24::utils::PADDING_SIZE - ((width * bmp24::utils::BYTES_PER_PIXEL) % bmp24::utils::PADDING_SIZE)) % bmp24::utils::PADDING_SIZE;
    }

    BMP24Header::BMP24Header(size_t image_width, size_t image_height)
    {
        std::fill(bmp24_header_bytes_, bmp24_header_bytes_ + bmp24::utils::HEADER_SIZE, 0);
        bmp24_header_bytes_[bmp24::utils::FORMAT_FIRST_POSITION] = bmp24::utils::BMP_SIGN[0];
        bmp24_header_bytes_[bmp24::utils::FORMAT_SECOND_POSITION] = bmp24::utils::BMP_SIGN[1];

        const size_t padding_size = GetPaddingSize(image_width);
        const size_t file_size = bmp24::utils::HEADER_SIZE + bmp24::utils::DIB_HEADER_SIZE +
                                 (padding_size + image_width) * image_height * bmp24::utils::BYTES_PER_PIXEL;
        NumToBytes(file_size, bmp24_header_bytes_ + bmp24::utils::FILE_SIZE_POSITION);
        NumToBytes(bmp24::utils::HEADER_SIZE + bmp24::utils::DIB_HEADER_SIZE, bmp24_header_bytes_ + bmp24::utils::PIXS_ARRAY_OFFSET_POSITION);
    }
    BMP24Header::BMP24Header(unsigned char bmp24_header_bytes[bmp24::utils::HEADER_SIZE], std::string_view path)
    {
        if (bmp24_header_bytes[0] != bmp24::utils::BMP_SIGN[0] || bmp24_header_bytes[1] != bmp24::utils::BMP_SIGN[1])
        {
            throw std::invalid_argument(bmp24::utils::exceptions::FILE_TYPE_ERROR + std::string(path));
        }
    }
    unsigned char *BMP24Header::GetBMP24HeaderBytes()
    {
        return bmp24_header_bytes_;
    }

    BMP24DIB_Header::BMP24DIB_Header(size_t image_width, size_t image_height)
    {
        // std::fill(bmp24_dib_header_bytes_, bmp24_dib_header_bytes_ + bmp24::utils::HEADER_SIZE, 0);

        NumToBytes(bmp24::utils::DIB_HEADER_SIZE, bmp24_dib_header_bytes_ + bmp24::utils::DIB_HEADER_SIZE_POSITION);
        NumToBytes(image_width, bmp24_dib_header_bytes_ + bmp24::utils::WIDTH_POSITION);
        NumToBytes(image_height, bmp24_dib_header_bytes_ + bmp24::utils::HEIGHT_POSITION);
        NumToBytes(bmp24::utils::DEFAULT_NUMBER_OF_PLANES, bmp24_dib_header_bytes_ + bmp24::utils::PLANES_POSITION);
        NumToBytes(bmp24::utils::BITS_PER_PIXEL, bmp24_dib_header_bytes_ + bmp24::utils::BITS_PER_PIXEL_POSITION);
        NumToBytes(image_width * image_height * bmp24::utils::BYTES_PER_PIXEL, bmp24_dib_header_bytes_ + bmp24::utils::IMAGE_DATA_SIZE_POSITION);
    }
    BMP24DIB_Header::BMP24DIB_Header(unsigned char *bmp24_dib_header_bytes)
    {
        std::copy(bmp24_dib_header_bytes, bmp24_dib_header_bytes + bmp24::utils::DIB_HEADER_SIZE, bmp24_dib_header_bytes_);
    }
    unsigned char *BMP24DIB_Header::GetBMP24DIB_HeaderBytes()
    {
        return bmp24_dib_header_bytes_;
    }
    size_t BMP24DIB_Header::GetWidth() const
    {
        return BytesToNum(bmp24_dib_header_bytes_ + bmp24::utils::WIDTH_POSITION);
    }
    size_t BMP24DIB_Header::GetHeight() const
    {
        return BytesToNum(bmp24_dib_header_bytes_ + bmp24::utils::HEIGHT_POSITION);
    }
}