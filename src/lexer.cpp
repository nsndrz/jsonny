#include "lexer.hpp"

namespace jsonny {

void Lexer::SkipWhitespace_() {
    while (pos_ < input_.size() && std::isspace(static_cast<unsigned char>(input_[pos_]))) {
        ++pos_;
    }
}

Token Lexer::ParseString_() {
    size_t start = pos_ + 1;
    size_t end = start;

    while (end < input_.size() && input_[end] != '"') {
        ++end;
    }
    if (end >= input_.size()) {
        // Ошибка: незакрытая строка
        return { TokenType::None, "" };
    }

    std::string_view value = input_.substr(start, end - start);
    pos_ = end + 1;
    return { TokenType::String, value };
}

Token Lexer::ParseNumber_() {
    size_t start = pos_;

    if (input_[pos_] == '-') {
        ++pos_;
    }

    while (pos_ < input_.size() && std::isdigit(static_cast<unsigned char>(input_[pos_]))) {
        ++pos_;
    }

    if (pos_ < input_.size() && input_[pos_] == '.') {
        ++pos_;
        while (pos_ < input_.size() && std::isdigit(static_cast<unsigned char>(input_[pos_]))) {
            ++pos_;
        }
    }

    if (pos_ < input_.size() && (input_[pos_] == 'e' || input_[pos_] == 'E')) {
        ++pos_;
        if (pos_ < input_.size() && (input_[pos_] == '+' || input_[pos_] == '-')) {
            ++pos_;
        }
        while (pos_ < input_.size() && std::isdigit(static_cast<unsigned char>(input_[pos_]))) {
            ++pos_;
        }
    }

    std::string_view num_str = input_.substr(start, pos_ - start);
    double val = std::stod(std::string(num_str));
    return { TokenType::Number, num_str, val };
}

Token Lexer::ParseLiteral_() {
    size_t start = pos_;
    while (pos_ < input_.size() && std::isalpha(static_cast<unsigned char>(input_[pos_]))) {
        ++pos_;
    }

    std::string_view literal = input_.substr(start, pos_ - start);
    if (literal == "true") {
        return { TokenType::True, literal };
    } else if (literal == "false") {
        return { TokenType::False, literal };
    } else if (literal == "null") {
        return { TokenType::Null, literal };
    }
    return { TokenType::None, literal };
}

Token Lexer::Next() {
    SkipWhitespace_();

    if (pos_ >= input_.size()) {
        return { TokenType::None, "" };
    }

    char current = input_[pos_];
    switch (current) {
        case '{':
            ++pos_;
            return { TokenType::LeftBrace, std::string_view(&input_[pos_ - 1], 1) };
        case '}':
            ++pos_;
            return { TokenType::RightBrace, std::string_view(&input_[pos_ - 1], 1) };
        case '[':
            ++pos_;
            return { TokenType::LeftBracket, std::string_view(&input_[pos_ - 1], 1) };
        case ']':
            ++pos_;
            return { TokenType::RightBracket, std::string_view(&input_[pos_ - 1], 1) };
        case ':':
            ++pos_;
            return { TokenType::Colon, std::string_view(&input_[pos_ - 1], 1) };
        case ',':
            ++pos_;
            return { TokenType::Comma, std::string_view(&input_[pos_ - 1], 1) };
        case '"':
            return ParseString_();
        default:
            if (std::isdigit(static_cast<unsigned char>(current)) || current == '-') {
                return ParseNumber_();
            }
            if (std::isalpha(static_cast<unsigned char>(current))) {
                return ParseLiteral_();
            }
            return { TokenType::None, "" };
    }
}

Token Lexer::Peek() const {
    size_t saved_pos = pos_;

    Lexer temp(input_);
    temp.pos_ = saved_pos;
    return temp.Next();
}

}