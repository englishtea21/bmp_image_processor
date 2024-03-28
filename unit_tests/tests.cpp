#include "../../../contrib/catch/catch.hpp"
#include "test_helper.h"

const std::string TEST_DATA_RELATIVE_PATH = "../test_script/data/";

TEST_CASE("input_output", "[bmp24_io]") {
    const ImageBmp image_read = input_output::ReaderBmp24(TEST_DATA_RELATIVE_PATH + "lenna.bmp").Read();
    input_output::WriterBmp24(TEST_DATA_RELATIVE_PATH + "lenna_altered.bmp").Write(image_read);

    const ImageBmp image_written = input_output::ReaderBmp24(TEST_DATA_RELATIVE_PATH + "lenna_altered.bmp").Read();
    REQUIRE(image_read == image_written);
}

TEST_CASE("simple filters", "[filter]") {
    // crop
    SECTION("[crop]") {
        ImageBmp lenna_cropped = filters::Crop(999, 1999).Apply(                       // NOLINT
            input_output::ReaderBmp24(TEST_DATA_RELATIVE_PATH + "lenna.bmp").Read());  // NOLINT
        REQUIRE(lenna_cropped == input_output::ReaderBmp24(TEST_DATA_RELATIVE_PATH + "lenna_crop.bmp").Read());
        REQUIRE(filters::Crop(100, 1).Apply(lenna_cropped) ==                                        // NOLINT
                input_output::ReaderBmp24(TEST_DATA_RELATIVE_PATH + "lenna_crop_crop.bmp").Read());  // NOLINT
        REQUIRE(filters::Crop(50, 50).Apply(                                                         // NOLINT
                    input_output::ReaderBmp24(TEST_DATA_RELATIVE_PATH + "flag.bmp").Read()) ==       // NOLINT
                input_output::ReaderBmp24(TEST_DATA_RELATIVE_PATH + "flag_crop.bmp").Read());        // NOLINT
    }
}

TEST_CASE("pixelwise filters", "[pixelwise]") {
    // grayscale
    SECTION("[grayscale]") {  // NOLINT

        REQUIRE(test_helper::CalcImagesDistance(
                    filters::Grayscale().Apply(input_output::ReaderBmp24(TEST_DATA_RELATIVE_PATH + "lenna.bmp").Read()),
                    input_output::ReaderBmp24(TEST_DATA_RELATIVE_PATH + "lenna_gs.bmp").Read()) <
                1 * bmp24::utils::COLOR_CHANNEL_MAX_VALUE);  // NOLINT
        REQUIRE(test_helper::CalcImagesDistance(
                    filters::Grayscale().Apply(input_output::ReaderBmp24(TEST_DATA_RELATIVE_PATH + "lenna.bmp").Read()),
                    input_output::ReaderBmp24(TEST_DATA_RELATIVE_PATH + "lenna_gs_gs.bmp").Read()) <
                1 * bmp24::utils::COLOR_CHANNEL_MAX_VALUE);  // NOLINT
        REQUIRE(test_helper::CalcImagesDistance(
                    filters::Grayscale().Apply(input_output::ReaderBmp24(TEST_DATA_RELATIVE_PATH + "flag.bmp").Read()),
                    input_output::ReaderBmp24(TEST_DATA_RELATIVE_PATH + "flag_gs.bmp").Read()) <
                1 * bmp24::utils::COLOR_CHANNEL_MAX_VALUE);  // NOLINT
    }
    // negative
    SECTION("[negative]") {  // NOLINT
        REQUIRE(test_helper::CalcImagesDistance(
                    filters::Negative().Apply(input_output::ReaderBmp24(TEST_DATA_RELATIVE_PATH + "lenna.bmp").Read()),
                    input_output::ReaderBmp24(TEST_DATA_RELATIVE_PATH + "lenna_neg.bmp").Read()) <
                1 * bmp24::utils::COLOR_CHANNEL_MAX_VALUE);  // NOLINT
        REQUIRE(test_helper::CalcImagesDistance(
                    filters::Negative().Apply(input_output::ReaderBmp24(TEST_DATA_RELATIVE_PATH + "lenna.bmp").Read()),
                    input_output::ReaderBmp24(TEST_DATA_RELATIVE_PATH + "lenna_neg_neg.bmp").Read()) <
                1 * bmp24::utils::COLOR_CHANNEL_MAX_VALUE);  // NOLINT
        REQUIRE(test_helper::CalcImagesDistance(
                    filters::Negative().Apply(input_output::ReaderBmp24(TEST_DATA_RELATIVE_PATH + "flag.bmp").Read()),
                    input_output::ReaderBmp24(TEST_DATA_RELATIVE_PATH + "flag_neg.bmp").Read()) <
                1 * bmp24::utils::COLOR_CHANNEL_MAX_VALUE);  // NOLINT
    }
}

TEST_CASE("convolutional filters", "[convolutional]") {
    // sharp
    SECTION("[sharp]") {  // NOLINT
        ImageBmp lenna_sharped =
            filters::Sharpening().Apply(input_output::ReaderBmp24(TEST_DATA_RELATIVE_PATH + "lenna.bmp").Read());
        REQUIRE(test_helper::CalcImagesDistance(
                    lenna_sharped,
                    input_output::ReaderBmp24(TEST_DATA_RELATIVE_PATH + "lenna_sharp.bmp").Read()) <
                1 * bmp24::utils::COLOR_CHANNEL_MAX_VALUE);  // NOLINT
        REQUIRE(test_helper::CalcImagesDistance(
                    filters::Sharpening().Apply(lenna_sharped),
                    input_output::ReaderBmp24(TEST_DATA_RELATIVE_PATH + "lenna_sharp_sharp.bmp").Read()) <
                1 * bmp24::utils::COLOR_CHANNEL_MAX_VALUE);  // NOLINT
        REQUIRE(test_helper::CalcImagesDistance(
                    filters::Sharpening().Apply(input_output::ReaderBmp24(TEST_DATA_RELATIVE_PATH + "flag.bmp").Read()),
                    input_output::ReaderBmp24(TEST_DATA_RELATIVE_PATH + "flag_sharp.bmp").Read()) <
                1 * bmp24::utils::COLOR_CHANNEL_MAX_VALUE);  // NOLINT
    }
    // edge detection
    SECTION("[edge detection]") {                                                     // NOLINT
        ImageBmp flag_edged = filters::EdgeDetection(0.1).Apply(                      // NOLINT
            input_output::ReaderBmp24(TEST_DATA_RELATIVE_PATH + "flag.bmp").Read());  // NOLINT
        REQUIRE(test_helper::CalcImagesDistance(
                    flag_edged,
                    input_output::ReaderBmp24(TEST_DATA_RELATIVE_PATH + "flag_edge.bmp").Read()) <
                1 * bmp24::utils::COLOR_CHANNEL_MAX_VALUE);  // NOLINT
        REQUIRE(test_helper::CalcImagesDistance(
                    filters::EdgeDetection(0.5).Apply(flag_edged),  // NOLINT
                    input_output::ReaderBmp24(TEST_DATA_RELATIVE_PATH + "flag_edge_edge.bmp").Read()) <
                1 * bmp24::utils::COLOR_CHANNEL_MAX_VALUE);  // NOLINT
    }
    // blur
    SECTION("[blur]") {                                                                // NOLINT
        ImageBmp lenna_blured = filters::GaussianBlur(7.5).Apply(                      // NOLINT
            input_output::ReaderBmp24(TEST_DATA_RELATIVE_PATH + "lenna.bmp").Read());  // NOLINT
        REQUIRE(test_helper::CalcImagesDistance(
                    lenna_blured,
                    input_output::ReaderBmp24(TEST_DATA_RELATIVE_PATH + "lenna_blur.bmp").Read()) <
                2 * bmp24::utils::COLOR_CHANNEL_MAX_VALUE);  // NOLINT
        REQUIRE(test_helper::CalcImagesDistance(
                    filters::GaussianBlur(3).Apply(lenna_blured),
                    input_output::ReaderBmp24(TEST_DATA_RELATIVE_PATH + "lenna_blur_blur.bmp").Read()) <
                2 * bmp24::utils::COLOR_CHANNEL_MAX_VALUE);  // NOLINT
    }
}