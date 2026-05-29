#include <iostream>
#include <string>
#include <cstring>

#include "lexer.hpp"
#include "jsonny.hpp"

namespace jsonny {

uint32_t JsonDocument::CreateNode_(JsonType type) {
    JsonNode node;
    node.type = type;
    nodes_.push_back(node);
    return static_cast<uint32_t>(nodes_.size() - 1);
}

bool JsonDocument::ParseString(const std::string& input) {
    Lexer lexer(input);

    nodes_.clear();
    string_buffer_.clear();
    root_idx_ = INVALID_INDEX;

    Token token = lexer.Next();
    if (token.type != TokenType::LeftBrace && token.type != TokenType::LeftBracket) {
        return false;
    }

    root_idx_ = CreateNode_(token.type == TokenType::LeftBrace ? JsonType::Object : JsonType::Array);
 
    if (token.type == TokenType::LeftBrace) {
        if (!ParseObject_(lexer, root_idx_)) return false;
    } else {
        if (!ParseArray_(lexer, root_idx_)) return false;
    }

    return true;
}

bool JsonDocument::ParseValue_(Lexer& lexer, uint32_t node_idx) {
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
            if (!ParseObject_(lexer, node_idx)) return false;
            break;
        }
        case TokenType::LeftBracket: {
            nodes_[node_idx].type = JsonType::Array;
            if (!ParseArray_(lexer, node_idx)) return false;
            break;
        }
        default:
            return false;
    }
    return true;
}

bool JsonDocument::ParseObject_(Lexer& lexer, uint32_t node_idx) {
    uint32_t last_child = INVALID_INDEX;

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
        
        uint32_t child_idx = CreateNode_(JsonType::Null);
        nodes_[child_idx].parent = node_idx;
        nodes_[child_idx].name_offset = static_cast<uint32_t>(name_offset);

        if (last_child == INVALID_INDEX) {
            nodes_[node_idx].first_child = child_idx;
        } else {
            nodes_[last_child].next_sibling = child_idx;
        }
        last_child = child_idx;

        token = lexer.Next();
        if (token.type != TokenType::Colon) {
            return false;
        }

        if (!ParseValue_(lexer, child_idx)) {
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

bool JsonDocument::ParseArray_(Lexer& lexer, uint32_t node_idx) {
    uint32_t last_child = INVALID_INDEX;

    while (true) {
        Token token = lexer.Peek();
        
        if (token.type == TokenType::RightBracket) {
            lexer.Next();
            return true;
        }

        uint32_t child_idx = CreateNode_(JsonType::Null);
        nodes_[child_idx].parent = node_idx;

        if (last_child == INVALID_INDEX) {
            nodes_[node_idx].first_child = child_idx;
        } else {
            nodes_[last_child].next_sibling = child_idx;
        }
        last_child = child_idx;

        if (!ParseValue_(lexer, child_idx)) {
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

uint32_t JsonDocument::FindChildByName(uint32_t parent_idx, const std::string& name) const {
    if (parent_idx >= nodes_.size()) return INVALID_INDEX;
    
    uint32_t current = nodes_[parent_idx].first_child;
    while (current != INVALID_INDEX) {
        const auto& node = nodes_[current];
        const char* key_str = &string_buffer_[node.name_offset];
        if (std::strcmp(key_str, name.c_str()) == 0) {
            return current;
        }
        current = node.next_sibling;
    }
    return INVALID_INDEX;
}

std::string JsonDocument::GetString(uint32_t node_idx) const {
    if (node_idx >= nodes_.size() || nodes_[node_idx].type != JsonType::String) {
        return "";
    }
    return std::string(&string_buffer_[nodes_[node_idx].string_offset]);
}

int JsonDocument::GetInt(uint32_t node_idx) const {
    if (node_idx >= nodes_.size() || nodes_[node_idx].type != JsonType::Number) {
        return 0;
    }
    return static_cast<int>(nodes_[node_idx].number_val);
}

float JsonDocument::GetFloat(uint32_t node_idx) const {
    if (node_idx >= nodes_.size() || nodes_[node_idx].type != JsonType::Number) {
        return 0.0f;
    }
    return static_cast<float>(nodes_[node_idx].number_val);
}

bool JsonDocument::GetBool(uint32_t node_idx) const {
    if (node_idx >= nodes_.size() || nodes_[node_idx].type != JsonType::Bool) {
        return false;
    }
    return nodes_[node_idx].bool_val;
}

void JsonDocument::DebugPrint() const {
    std::cout << "--- JSON DEBUG DUMP ---" << std::endl;
    std::cout << "Total nodes: " << nodes_.size() << std::endl;
    
    for (size_t i = 0; i < nodes_.size(); ++i) {
        const auto& node = nodes_[i];

        std::cout << "Node [" << i << "] Parent: " << node.parent;
        std::cout << " | Type: " << static_cast<int>(node.type);
        std::cout << " | FirstChild: " << node.first_child;
        std::cout << " | NextSibling: " << node.next_sibling;

        if (node.name_offset != 0 || i == root_idx_) {
             if (i != root_idx_ || node.name_offset != 0) {
                 const char* nameStr = &string_buffer_[node.name_offset];
                 std::cout << " KEY: " << nameStr;
             }
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