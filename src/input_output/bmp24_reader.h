#ifndef BMP24_READER_H
#define BMP24_READER_H

#include <cerrno>
#include <fstream>
#include <string>
#include "../image/image.h"
#include "bmp24.h"

namespace input_output {

class ReaderBmp24 {
public:
    explicit ReaderBmp24(const std::string &filename);

    ImageBmp Read();

private:
    std::string path_;
};
}  // namespace input_output

#endif  // BMP24_READER_H