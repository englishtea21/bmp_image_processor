#ifndef READER_H
#define READER_H

#include <cerrno>
#include <fstream>
#include <string>
#include "../image/image.h"

namespace input_output {

class Reader {
public:
    Reader(const std::string& filename);

    ImageBMP Read();

private:
    std::string path_;

    std::size_t BytesToNum(unsigned char* bytes);
};

const size_t GetPaddingSize(size_t width);
}  // namespace input_output
#endif  // READER_H