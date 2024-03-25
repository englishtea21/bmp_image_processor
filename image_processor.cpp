#include "image_processor.h"

int main(int argc, char *argv[]) {
    try {
        std::vector<parser::Token> tokens = GetTokens(argc, argv);
        ImageBmp image_bmp = GetImage(tokens[0].name);
        ApplyFilters(image_bmp, tokens);
        WriteImage(image_bmp, tokens[1].name);
    } catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
    }
    return 0;
}

std::vector<parser::Token> GetTokens(int argc, char *argv[]) {
    std::vector<parser::Token> tokens = parser::Parse(argc, argv);
    if (tokens.size() < 2) {
        throw std::invalid_argument("More arguments expected");
    }
    return tokens;
}
ImageBmp GetImage(const std::string &path) {
    input_output::ReaderBmp24 reader(path);
    ImageBmp image_bmp = reader.Read();
    return image_bmp;
}
void WriteImage(const ImageBmp &image_bmp, const std::string &path) {
    input_output::WriterBmp24 writer(path);
    writer.Write(image_bmp);
}
void ApplyFilters(ImageBmp &image, const std::vector<parser::Token> &tokens) {
    for (size_t i = 2; i < tokens.size(); i++) {
        image = filters::CreateFilter(tokens[i])->Apply(image);
    }
}