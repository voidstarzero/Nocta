#pragma once

#include <cstdint>
#include <string>

#include "Token_Type.hpp"

struct Token
{
    const Token_Type type;
    const std::string text;
    const int line, col; // (line, col) of start

    Token(Token_Type t, std::string lexeme, int l, int c)
        : type{t}, text{lexeme}, line{l}, col{c}
        {}

    std::string to_string() const;
};
