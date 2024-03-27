#include "../catch/catch.hpp"
#include "../main/image_processor.h"

// TEST_CASE("hehe") {
//     REQUIRE(1 == 1);
// }

TEST_CASE("input_output", "[bmp24_io]") {
    const ImageBmp image_read = input_output::ReaderBmp24("./data/lenna.bmp").Read();
    input_output::WriterBmp24("./data/lenna_altered.bmp").Write(image_read);

    const ImageBmp image_written = input_output::ReaderBmp24("./data/lenna_altered.bmp").Read();
    REQUIRE(image_read == image_written);
}