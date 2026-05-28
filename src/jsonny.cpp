#include <iostream>

#include "lexer.hpp"
#include "jsonny.hpp"


namespace jsonny {

uint32_t JsonDocument::CreateNode(JsonType type) {
    JsonNode node;
    node.type = type;
    node.parent = 0;
    node.name_offset = 0;
    node.string_offset = 0;
    node.number_val = 0.0; 

    nodes_.push_back(node);
    return static_cast<uint32_t>(nodes_.size() - 1);
}

bool JsonDocument::ParseString(const std::string& input) {
    Lexer lexer(input);

    nodes_.clear();
    string_buffer_.clear();

    Token token = lexer.Next();
    if (token.type != TokenType::LeftBrace) {
        return false;
    }

    uint32_t root_idx = CreateNode(JsonType::Object);
    root_node_ = nodes_[root_idx];
    nodes_[root_idx].parent = 0;

    if (!ParseObject(lexer, root_idx)) {
        return false;
    }

    return true;
}

bool JsonDocument::ParseValue(Lexer& lexer, uint32_t node_idx) {
    Token token = lexer.Next();

    switch (token.type) {
        case TokenType::String: {
            nodes_[node_idx].type = JsonType::String;

            size_t offset = string_buffer_.size();
            string_buffer_.append(token.value.data(), token.value.size());
            string_buffer_.push_back('\0');
            nodes_[node_idx].string_offset = static_cast<uint32_t>(offset);
            break;
        }
        case TokenType::Number: {
            nodes_[node_idx].type = JsonType::Number;
            nodes_[node_idx].number_val = token.number_value;
            break;
        }
        case TokenType::True:
        case TokenType::False: {
            nodes_[node_idx].type = JsonType::Bool;
            nodes_[node_idx].bool_val = (token.type == TokenType::True);
            break;
        }
        case TokenType::Null: {
            nodes_[node_idx].type = JsonType::Null;
            break;
        }
        case TokenType::LeftBrace: {
            nodes_[node_idx].type = JsonType::Object;
            if (!ParseObject(lexer, node_idx)) return false;
            break;
        }
        case TokenType::LeftBracket: {
            nodes_[node_idx].type = JsonType::Array;
            if (!ParseArray(lexer, node_idx)) return false;
            break;
        }
        default:
            return false;
    }
    return true;
}

bool JsonDocument::ParseObject(Lexer& lexer, uint32_t node_idx) {
    while (true) {
        Token token = lexer.Next();

        if (token.type == TokenType::RightBrace) {
            return true;
        }

        if (token.type != TokenType::String) {
            return false;
        }

        size_t name_offset = string_buffer_.size();
        string_buffer_.append(token.value.data(), token.value.size());
        string_buffer_.push_back('\0');
        
        uint32_t child_idx = CreateNode(JsonType::Null);
        nodes_[child_idx].parent = node_idx;
        nodes_[child_idx].name_offset = static_cast<uint32_t>(name_offset);

        token = lexer.Next();
        if (token.type != TokenType::Colon) {
            return false;
        }

        if (!ParseValue(lexer, child_idx)) {
            return false;
        }

        token = lexer.Next();
        if (token.type == TokenType::Comma) {
            continue;
        } else if (token.type == TokenType::RightBrace) {
            return true;
        } else {
            return false;
        }
    }
}

bool JsonDocument::ParseArray(Lexer& lexer, uint32_t node_idx) {
    while (true) {
        Token token = lexer.Peek();
        
        if (token.type == TokenType::RightBracket) {
            lexer.Next();
            return true;
        }

        uint32_t child_idx = CreateNode(JsonType::Null);
        nodes_[child_idx].parent = node_idx;

        if (!ParseValue(lexer, child_idx)) {
            return false;
        }

        token = lexer.Next();
        if (token.type == TokenType::Comma) {
            continue;
        } else if (token.type == TokenType::RightBracket) {
            return true;
        } else {
            return false;
        }
    }
}

void JsonDocument::DebugPrint() const {
    std::cout << "--- JSON DEBUG DUMP ---" << std::endl;
    std::cout << "Total nodes: " << nodes_.size() << std::endl;
    
    for (size_t i = 0; i < nodes_.size(); ++i) {
        const auto& node = nodes_[i];

        std::cout << "Node [" << i << "] Parent: " << node.parent;
        std::cout << " | Type: " << static_cast<int>(node.type);

        if (i != 0) {
            const char* nameStr = &string_buffer_[node.name_offset];
            std::cout << " KEY: " << nameStr;
        }

        switch (node.type) {
            case JsonType::String:
                std::cout << " | STR: " << &string_buffer_[node.string_offset];
                break;
            case JsonType::Number:
                std::cout << " | NUM: " << node.number_val;
                break;
            case JsonType::Bool:
                std::cout << " | BOOL: " << (node.bool_val ? "true" : "false");
                break;
            case JsonType::Null:
                std::cout << " | NULL";
                break;
            default:
                break;
        }
        std::cout << std::endl;
    }
    std::cout << "-----------------------" << std::endl;
}

}