#include "lexer.hpp"
#include "jsonny.hpp"


namespace jsonny {

bool JsonDocument::ParseString(const std::string& input) {
    Lexer lexer(input);
    
    Token token = lexer.Next();
    if (token.type != TokenType::LeftBrace) {
        return false;
    }
    
    
    
    return true;
}

}