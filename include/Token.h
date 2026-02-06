#pragma once

#include <string>
#include <cstdint>

namespace Script {

enum class TokenType {
    // Literals
    INT_LITERAL,
    STRING_LITERAL,
    TRUE,
    FALSE,
    
    // Identifiers and Keywords
    IDENTIFIER,
    
    // Data Types
    INT8,
    UINT8,
    INT16,
    UINT16,
    INT32,
    UINT32,
    INT64,
    UINT64,
    STRING,
    BOOL,
    VOID,
    
    // Control Flow
    IF,
    ELSE,
    WHILE,
    FOR,
    RETURN,
    
    // Operators
    PLUS,           // +
    MINUS,          // -
    MULTIPLY,       // *
    DIVIDE,         // /
    MODULO,         // %
    ASSIGN,         // =
    PLUS_ASSIGN,    // +=
    MINUS_ASSIGN,   // -=
    MULT_ASSIGN,    // *=
    DIV_ASSIGN,     // /=
    
    // Comparison
    EQUAL,          // ==
    NOT_EQUAL,      // !=
    LESS_THAN,      // <
    GREATER_THAN,   // >
    LESS_EQUAL,     // <=
    GREATER_EQUAL,  // >=
    
    // Logical
    AND,            // &&
    OR,             // ||
    NOT,            // !
    
    // Delimiters
    LPAREN,         // (
    RPAREN,         // )
    LBRACE,         // {
    RBRACE,         // }
    SEMICOLON,      // ;
    COMMA,          // ,
    
    // Special
    END_OF_FILE,
    UNKNOWN
};

struct Token {
    TokenType type;
    std::string lexeme;
    int line;
    int column;
    
    // For literals
    int64_t intValue = 0;
    std::string stringValue;
    
    Token(TokenType t = TokenType::UNKNOWN, const std::string& lex = "", int ln = 0, int col = 0)
        : type(t), lexeme(lex), line(ln), column(col) {}
    
    std::string toString() const;
};

} // namespace Script
