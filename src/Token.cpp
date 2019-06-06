#include "Token.hpp"

std::string Token::to_string() const
{
    return std::string{token_name(type)} + ' ' + text;
}
