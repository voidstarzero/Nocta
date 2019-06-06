#pragma once

#include <string>
#include <vector>

#include "Token.hpp"

class Scanner
{
    const std::string m_source;
    std::vector<Token> m_tokens;

    int start = 0, current = 0;
    int line = 1, col = 1;

    bool is_at_end();
    char advance();
    void scan_token();
    void add_token(Token_Type t);

    bool match(char expected);
    char peek();
    char peek_next();

    void string();
    void number();
    void identifier();

public:
    Scanner(std::string source) : m_source{source} {}

    void scan_tokens();
    const std::vector<Token>& get_tokens();
};
