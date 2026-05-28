#pragma once

#include <cstdint>
#include <vector>


namespace jsonny {

class Lexer;

enum class JsonType { Null, Bool, Number, String, Array, Object };

struct JsonNode {
    JsonType type;

    uint32_t parent;
    uint32_t name_offset;

    union {
        bool bool_val;
        double number_val;
        uint32_t string_offset;
    };
};

class JsonDocument {
private:
    std::vector<JsonNode> nodes_;
    std::string string_buffer_;
    JsonNode root_node_;

public:
    bool ParseString(const std::string& input);
    const JsonNode& root() const { return root_node_; };

    uint32_t CreateNode(JsonType type);
    bool ParseValue(Lexer& lexer, uint32_t node_idx);
    bool ParseObject(Lexer& lexer, uint32_t node_idx);
    bool ParseArray(Lexer& lexer, uint32_t node_idx);

    void DebugPrint() const;
};

}