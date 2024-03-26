#include "parser.h"

namespace parser {
void Token::Clear() {
    name.clear();
    args.clear();
}
bool Token::IsEmpty() const {
    return name.empty() && args.empty();
}
std::vector<Token> Parse(int argc, char *argv[]) {
    std::vector<Token> tokens;
    Token curr;
    // пропускаем первый аргумент - это просто название программы
    for (size_t i = 1; i < static_cast<size_t>(argc); ++i) {
        std::string str(argv[i]);
        if (i <= 2) {
            // если обрабатываем первые два аргумента - пути до файлов
            curr.name = str;
            tokens.push_back(curr);
            curr.Clear();
        } else {
            if (str.front() == '-') {
                // если начали обрабатывать новый фильтр
                if (!curr.IsEmpty()) {
                    tokens.push_back(curr);
                    curr.Clear();
                }
                // имя нового фильтра
                curr.name = str;
            } else {
                // прододжаем работу с аргументами одного фильтра
                curr.args.push_back(str);
            }

            if (i == static_cast<size_t>(argc - 1)) {
                tokens.push_back(curr);
            }
        }
    }

    return tokens;
}
}  // namespace parser