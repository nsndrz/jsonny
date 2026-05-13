#pragma once

#include <string>
#include <string_view>
#include <cstdint>


namespace jsonny {

enum class TokenType {
    None,
    LeftBrace,    // {
    RightBrace,   // }
    LeftBracket,  // [
    RightBracket, // ]
    Colon,        // :
    Comma,        // ,
    String,       // "..."
    Number,       // 123, 12.34
    True,         // true
    False,        // false
    Null          // null
};

struct Token {
    TokenType type;
    std::string_view value;
    double number_value = 0.0; 
};

class Lexer {
private:
    std::string_view input_;
    size_t pos_ = 0;

    void SkipWhitespace_();
    Token ParseString_();
    Token ParseNumber_();
    Token ParseLiteral_();

public:
    explicit Lexer(std::string_view input);

    Token Next();
    // Token Peek() const;
    bool IsEof() const { return pos_ >= input_.size(); }
};

}