#ifndef WRITER_H
#define WRITER_H

#include <algorithm>
#include <fstream>
#include <string>

#include "../image/image.h"
#include "reader.h"
#include "utils.h"

namespace input_output {
class Writer {
public:
    Writer(std::string path);
    void Write(const ImageBMP& image_bmp);

private:
    void WriteBMPHeader(unsigned char* bmp_header, size_t size);
    void WriteDIBHeader(unsigned char* dib_header, size_t width, size_t height);

    template <typename T>
    void NumToBytes(T num, unsigned char* bytes);

    std::string path_;
};
}  // namespace input_output

#endif  // WRITER_H