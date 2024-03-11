#include "image_processor.h"

int main(int argc, char* argv[]) {
    try {
        std::vector<parser::Token> tokens = GetTokens(argc, argv);
        ImageBMP image_bmp = GetImage(tokens[0].name);
        // применяем фильтры
        ApplyFilters(image_bmp, tokens);
        WriteImage(image_bmp, tokens[1].name);
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
    }
    return 0;
}

std::vector<parser::Token> GetTokens(int argc, char* argv[]) {
    std::vector<parser::Token> tokens = parser::Parse(argc, argv);
    if (tokens.size() < 2) {
        throw std::invalid_argument("More arguments expected");
    }
    return tokens;
}
ImageBMP GetImage(const std::string& path) {
    input_output::Reader reader(path);
    ImageBMP image_bmp = reader.Read();
}
void WriteImage(const ImageBMP& image_bmp, const std::string& path) {
    input_output::Writer writer(path);
    writer.Write(image_bmp);
}
void ApplyFilters(ImageBMP& image, const std::vector<parser::Token>& tokens) {
    for (size_t i = 2; i < tokens.size(); i++) {
        image = filters::CreateFilter(tokens[i])->Apply(image);
    }
}