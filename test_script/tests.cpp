#include "../../../../contrib/catch/catch.hpp"
#include "../main/image_processor.h"

const std::string TEST_DATA_RELATIVE_PATH = "test_script/data/";

TEST_CASE("input_output", "[bmp24_io]") {
    const ImageBmp image_read = input_output::ReaderBmp24(TEST_DATA_RELATIVE_PATH + "lenna.bmp").Read();
    input_output::WriterBmp24(TEST_DATA_RELATIVE_PATH + "lenna_altered.bmp").Write(image_read);

    const ImageBmp image_written = input_output::ReaderBmp24(TEST_DATA_RELATIVE_PATH + "lenna_altered.bmp").Read();
    REQUIRE(image_read == image_written);
}

TEST_CASE("simple filters", "[filter]") {
    // crop
    ImageBmp lenna_cropped =
        filters::Crop(999, 1999).Apply(input_output::ReaderBmp24(TEST_DATA_RELATIVE_PATH + "lenna.bmp").Read());
    REQUIRE(lenna_cropped == input_output::ReaderBmp24(TEST_DATA_RELATIVE_PATH + "lenna_crop.bmp").Read());
    REQUIRE(filters::Crop(100, 1).Apply(lenna_cropped) ==
            input_output::ReaderBmp24(TEST_DATA_RELATIVE_PATH + "lenna_crop_crop.bmp").Read());
    REQUIRE(filters::Crop(50, 50).Apply(input_output::ReaderBmp24(TEST_DATA_RELATIVE_PATH + "flag.bmp").Read()) ==
            input_output::ReaderBmp24(TEST_DATA_RELATIVE_PATH + "flag_crop.bmp").Read());
}

TEST_CASE("pixelwise filters", "[pixelwise]") {
    // grayscale
    REQUIRE(filters::Grayscale().Apply(input_output::ReaderBmp24(TEST_DATA_RELATIVE_PATH + "lenna.bmp").Read()) ==
            input_output::ReaderBmp24(TEST_DATA_RELATIVE_PATH + "lenna_gs.bmp").Read());
    REQUIRE(filters::Grayscale().Apply(input_output::ReaderBmp24(TEST_DATA_RELATIVE_PATH + "flag.bmp").Read()) ==
            input_output::ReaderBmp24(TEST_DATA_RELATIVE_PATH + "flag_gs.bmp").Read());

    // negative
    REQUIRE(filters::Negative().Apply(input_output::ReaderBmp24(TEST_DATA_RELATIVE_PATH + "lenna.bmp").Read()) ==
            input_output::ReaderBmp24(TEST_DATA_RELATIVE_PATH + "lenna_neg.bmp").Read());
    REQUIRE(filters::Grayscale().Apply(
                filters::Grayscale().Apply(input_output::ReaderBmp24(TEST_DATA_RELATIVE_PATH + "lenna.bmp").Read())) ==
            input_output::ReaderBmp24(TEST_DATA_RELATIVE_PATH + "lenna_neg_neg.bmp").Read());
    REQUIRE(filters::Negative().Apply(input_output::ReaderBmp24(TEST_DATA_RELATIVE_PATH + "flag.bmp").Read()) ==
            input_output::ReaderBmp24(TEST_DATA_RELATIVE_PATH + "flag_neg.bmp").Read());
}

TEST_CASE("covvolutional filters", "[convolutional]") {
    // sharp
    ImageBmp lenna_sharped =
        filters::Sharpening().Apply(input_output::ReaderBmp24(TEST_DATA_RELATIVE_PATH + "lenna.bmp").Read());
    REQUIRE(lenna_sharped == input_output::ReaderBmp24(TEST_DATA_RELATIVE_PATH + "lenna_sharp.bmp").Read());
    REQUIRE(filters::Sharpening().Apply(lenna_sharped) ==
            input_output::ReaderBmp24(TEST_DATA_RELATIVE_PATH + "lenna_sharp_sharp.bmp").Read());
    REQUIRE(filters::Sharpening().Apply(input_output::ReaderBmp24(TEST_DATA_RELATIVE_PATH + "flag.bmp").Read()) ==
            input_output::ReaderBmp24(TEST_DATA_RELATIVE_PATH + "flag_sharp.bmp").Read());

    // edge detection
    ImageBmp flag_edged =
        filters::EdgeDetection(0.1).Apply(input_output::ReaderBmp24(TEST_DATA_RELATIVE_PATH + "flag.bmp").Read());
    REQUIRE(flag_edged == input_output::ReaderBmp24(TEST_DATA_RELATIVE_PATH + "flag_edge.bmp").Read());
    REQUIRE(filters::EdgeDetection(0.5).Apply(flag_edged) ==
            input_output::ReaderBmp24(TEST_DATA_RELATIVE_PATH + "flag_edge_edge.bmp").Read());

    // blur
    ImageBmp lenna_blured =
        filters::GaussianBlur(7.5).Apply(input_output::ReaderBmp24(TEST_DATA_RELATIVE_PATH + "lenna.bmp").Read());
    REQUIRE(lenna_blured == input_output::ReaderBmp24(TEST_DATA_RELATIVE_PATH + "lenna_blur.bmp").Read());
    REQUIRE(filters::GaussianBlur(3).Apply(lenna_blured) ==
            input_output::ReaderBmp24(TEST_DATA_RELATIVE_PATH + "lenna_blur_blur.bmp").Read());
}