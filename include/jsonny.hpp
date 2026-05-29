#pragma once

#include <cstdint>
#include <vector>


namespace jsonny {

class Lexer;

enum class JsonType { Null, Bool, Number, String, Array, Object };

static const uint32_t INVALID_INDEX = 0;

struct JsonNode {
    JsonType type;
    uint32_t parent;
    uint32_t name_offset;
    uint32_t first_child;
    uint32_t next_sibling;
    union {
        bool bool_val;
        double number_val;
        uint32_t string_offset;
    };

    JsonNode() : type(JsonType::Null), parent(INVALID_INDEX), name_offset(0),
                 first_child(INVALID_INDEX), next_sibling(INVALID_INDEX), number_val(0.0) {}
};

class JsonDocument {
private:
    std::vector<JsonNode> nodes_;
    std::string string_buffer_;
    uint32_t root_idx_;

    uint32_t CreateNode_(JsonType type);
    bool ParseValue_(Lexer& lexer, uint32_t node_idx);
    bool ParseObject_(Lexer& lexer, uint32_t node_idx);
    bool ParseArray_(Lexer& lexer, uint32_t node_idx);

public:
    JsonDocument() : root_idx_(1) {};

    bool ParseString(const std::string& input);
    
    uint32_t GetRootIdx() const { return root_idx_; };
    const JsonNode& GetNode(uint32_t idx) const { return nodes_[idx]; }
    uint32_t FindChildByName(uint32_t parent_idx, const std::string& name) const;

    std::string GetString(uint32_t node_idx) const;
    int GetInt(uint32_t node_idx) const;
    float GetFloat(uint32_t node_idx) const;
    bool GetBool(uint32_t node_idx) const;
    JsonType GetType(uint32_t node_idx) const { return nodes_[node_idx].type; }

    void DebugPrint() const;
};

}