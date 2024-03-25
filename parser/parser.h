#ifndef PARSER_H
#define PARSER_H

#include <string>
#include <vector>

namespace parser {
struct Token {
    std::string name;
    std::vector<std::string> args;
    void Clear();
    bool IsEmpty() const;
};

std::vector<Token> Parse(int argc, char *argv[]);
}  // namespace parser

#endif  // PARSER_H