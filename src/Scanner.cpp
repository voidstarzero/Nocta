#include "Scanner.hpp"

#include "Nocta.hpp"

#include <string_view>

// TODO: break core parsing functions (peek/advance/etc.) into own text processing class

bool Scanner::is_at_end()
{
    return current >= m_source.length();
}

char Scanner::advance()
{
    col++;
    return m_source[current++];
}

void Scanner::add_token(Token_Type t)
{
    m_tokens.emplace_back(t, m_source.substr(start, current - start), line, col);
}

bool Scanner::match(char expected)
{
    if (is_at_end()) return false;
    if (m_source[current] != expected) return false;

    col++;
    current++;
    return true;
}

char Scanner::peek()
{
    if (is_at_end()) return '\0';
    return m_source[current];
}

static bool is_digit(char c)
{
    return c >= '0' && c <= '9';
}

static bool is_alpha(char c) {
    return (c >= 'a' && c <= 'z')
            || (c >= 'A' && c <= 'Z')
            || c == '_';
}

void Scanner::scan_token()
{
    char c = advance();
    switch (c) {
    case '^':
        add_token(match('^') ? T_HAT_HAT :
                  match('=') ? T_HAT_EQUAL :
                  T_HAT);
        break;
    case '~':
        add_token(T_TILDE);
        break;
    case '<':
        add_token(match('<') ? (match('=') ? T_LESS_LESS_EQUAL :
                                T_LESS_LESS) :
                  match('=') ? T_LESS_EQUAL :
                  T_LESS);
        break;
    case '=':
        add_token(match('=') ? T_EQUAL_EQUAL :
                  match('>') ? T_EQUAL_GREAT :
                  T_EQUAL);
        break;
    case '>':
        add_token(match('>') ? (match('=') ? T_GREAT_GREAT_EQUAL :
                                T_GREAT_GREAT) :
                  match('=') ? T_GREAT_EQUAL :
                  T_GREAT);
        break;
    case '|':
        add_token(match('|') ? T_PIPE_PIPE :
                  match('=') ? T_PIPE_EQUAL :
                  T_PIPE);
        break;
    case '-':
        add_token(match('-') ? T_DASH_DASH :
                  match('>') ? T_DASH_GREAT :
                  match('=') ? T_DASH_EQUAL :
                  T_DASH);
        break;
    case ',':
        add_token(T_COMMA);
        break;
    case ';':
        add_token(T_SEMICOLON);
        break;
    case ':':
        add_token(match(':') ? T_COLON_COLON :
                  T_COLON);
        break;
    case '!':
        add_token(match('!') ? T_EXCLAM_EXCLAM :
                  match('=') ? T_EXCLAM_EQUAL :
                  T_EXCLAM);
        break;
    case '?':
        add_token(match('?') ? T_QUEST_QUEST :
                  T_QUEST);
        break;
    case '/':
        if (match('/')) {
            // Deal with comments
            while (peek() != '\n' && !is_at_end()) advance();
        }
        else {
            add_token(match('=') ? T_SLASH_EQUAL :
                      T_SLASH);
        }
        break;
    case '.':
        add_token(match('.') ? T_DOT_DOT :
                  T_DOT);
        break;
    case '(':
        add_token(T_LEFT_PAREN);
        break;
    case ')':
        add_token(T_RIGHT_PAREN);
        break;
    case '[':
        add_token(match(']') ? T_DBL_SQUARE :
                  T_LEFT_SQUARE);
        break;
    case ']':
        add_token(T_RIGHT_SQUARE);
        break;
    case '{':
        add_token(T_LEFT_BRACE);
        break;
    case '}':
        add_token(T_RIGHT_BRACE);
        break;
    case '*':
        add_token(match('=') ? T_STAR_EQUAL :
                  T_STAR);
        break;
    case '&':
        add_token(match('&') ? T_AMP_AMP :
                  match('=') ? T_AMP_EQUAL :
                  T_AMP);
        break;
    case '#':
        add_token(T_HASH);
        break;
    case '%':
        add_token(match('=') ? T_PERC_EQUAL :
                  T_PERC);
        break;
    case '+':
        add_token(match('+') ? T_PLUS_PLUS :
                  match('=') ? T_PLUS_EQUAL :
                  T_PLUS);
        break;
    case '"':
        string();
        break;
    case '\t':
        col += 3;
    case ' ':
        col += 1;
        break; // Ignore whitespace
    case '\n':
        line++;
        col = 0;
        break;
    default:
        if (is_digit(c)) {
            number();
        }
        else if (is_alpha(c)) {
            identifier();
        }
        else {
            error(line, col, "Unexpected character");
        }
        break;
    }
}

void Scanner::string()
{
    // TODO: force an unclosed string to be followed by another string
    // TODO: merge adjacent string literals

    while (peek() != '"' && !is_at_end()) {
        if (peek() == '\n') {
            line++;
            col = 0;
            break;
        }
        advance();
    }

    if (is_at_end()) {
        error(line, col, "Unterminated string");
        return;
    }

    // Eat the closing " or \n
    advance();

    add_token(T_STRING);
}

char Scanner::peek_next()
{
    if (current + 1 >= m_source.length()) {
        return '\0';
    }
    return m_source[current + 1];
}

void Scanner::number()
{
    while (is_digit(peek())) {
        advance();
    }

    // Look for decimal point
    if (peek() == '.' && is_digit(peek_next())) {
        // Consume the .
        advance();

        while (is_digit(peek())) {
            advance();
        }

        add_token(T_FLOAT);
    }
    else {
        add_token(T_INT);
    }
}

void Scanner::identifier()
{
    char next = peek();
    while (is_alpha(next) || is_digit(next)) {
        advance();
        next = peek();
    }

    // See if the identifier is a keyword
    std::string_view word = m_source;
    word = word.substr(start, current - start);

    Token_Type type = keyword_type(word);
    add_token(type == T_EOF ? T_IDENT : type);
}

void Scanner::scan_tokens()
{
    while (!is_at_end()) {
        start = current;
        scan_token();
    }

    m_tokens.emplace_back(T_EOF, "", line, col);
}

const std::vector<Token>& Scanner::get_tokens()
{
    return m_tokens;
}
