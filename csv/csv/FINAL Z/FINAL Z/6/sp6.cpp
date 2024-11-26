#include <iostream>
#include <fstream>
#include <regex>
#include <string>
#include <map>
#include <vector>
#include <sstream>

enum TokenType {
    KEYWORD, IDENTIFIER, OPERATOR, LITERAL, UNKNOWN
};

struct Token {
    int line;
    std::string lexeme;
    TokenType type;
    std::string value;
};

// Map of basic keywords for the C language (expandable)
std::map<std::string, TokenType> keywords = {
    {"int", KEYWORD}, {"float", KEYWORD}, {"if", KEYWORD}, {"else", KEYWORD}
};

// Classifies the lexeme into the corresponding token type
TokenType classify(const std::string& lexeme) {
    if (keywords.find(lexeme) != keywords.end()) return KEYWORD;
    if (std::regex_match(lexeme, std::regex("[a-zA-Z_][a-zA-Z0-9_]*"))) return IDENTIFIER;
    if (std::regex_match(lexeme, std::regex("[+-/*=<>!]"))) return OPERATOR;
    if (std::regex_match(lexeme, std::regex("[0-9]+"))) return LITERAL;
    return UNKNOWN;
}

int main() {
    std::ifstream file("input.txt"); // Input C program file
    if (!file) {
        std::cerr << "Error opening input file.\n";
        return 1;
    }

    std::vector<Token> tokens;
    std::string line;
    int lineNo = 1;

    while (std::getline(file, line)) {
        std::istringstream stream(line);
        std::string lexeme;
        
        // Process each lexeme in the line
        while (stream >> lexeme) {
            Token token;
            token.line = lineNo;
            token.lexeme = lexeme;
            token.type = classify(lexeme);
            token.value = lexeme; // For simplicity, use lexeme as value
            tokens.push_back(token);
        }
        
        lineNo++;
    }

    // Display the tokens
    std::cout << "Lexeme\t\tToken Type\tToken Value\n";
    for (const auto& token : tokens) {
        std::cout << "Lexeme: " << token.lexeme << "\t"
                  << "Token Type: ";

        // Print the token type
        switch (token.type) {
            case KEYWORD: std::cout << "KEYWORD"; break;
            case IDENTIFIER: std::cout << "IDENTIFIER"; break;
            case OPERATOR: std::cout << "OPERATOR"; break;
            case LITERAL: std::cout << "LITERAL"; break;
            default: std::cout << "UNKNOWN"; break;
        }

        std::cout << "\tToken Value: " << token.value << std::endl;
    }

    file.close();
    return 0;
}
