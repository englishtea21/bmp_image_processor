#ifndef BMP24_WRITER_H
#define BMP24_WRITER_H

#include <algorithm>
#include <fstream>
#include <string>

#include "bmp24_reader.h"

namespace input_output {
class WriterBMP24 {
public:
    WriterBMP24(std::string path);
    void Write(const ImageBMP &image_bmp);

private:
    std::string path_;
};
}  // namespace input_output

#endif  // BMP24_WRITER_H