#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_IDENTIFIER_LEN 31
#define MAX_NUMBER_LEN 31
#define MAX_STRING_LEN 1023
#define KEYWORDS_COUNT 32

// Token types
typedef enum {
    TOKEN_EOF = 0,
    TOKEN_IDENTIFIER,
    TOKEN_KEYWORD,
    TOKEN_NUMBER,
    TOKEN_STRING,
    TOKEN_CHAR,
    TOKEN_OPERATOR,
    TOKEN_SEPARATOR,
    TOKEN_PREPROCESSOR,
    TOKEN_COMMENT
} TokenType;

// Token structure
typedef struct {
    TokenType type;
    char lexeme[MAX_IDENTIFIER_LEN + 1];
    int line;
    int column;
} Token;

// Keywords array
const char* keywords[] = {
    "auto", "break", "case", "char",
    "const", "continue", "default", "do",
    "double", "else", "enum", "extern",
    "float", "for", "goto", "if",
    "int", "long", "register", "return",
    "short", "signed", "sizeof", "static",
    "struct", "switch", "typedef", "union",
    "unsigned", "void", "volatile", "while"
};

// Lexer structure
typedef struct {
    FILE* source;
    int line;
    int column;
    int last_column;
} Lexer;

// Initialize lexer
Lexer* init_lexer(const char* filename) {
    Lexer* lexer = (Lexer*)malloc(sizeof(Lexer));
    if (!lexer) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(1);
    }
    
    lexer->source = fopen(filename, "r");
    if (!lexer->source) {
        fprintf(stderr, "Cannot open file %s\n", filename);
        free(lexer);
        exit(1);
    }
    
    lexer->line = 1;
    lexer->column = 0;
    lexer->last_column = 0;
    return lexer;
}

// Free lexer resources
void free_lexer(Lexer* lexer) {
    if (lexer) {
        if (lexer->source) {
            fclose(lexer->source);
        }
        free(lexer);
    }
}

// Get next character
char get_char(Lexer* lexer) {
    char c = fgetc(lexer->source);
    if (c == '\n') {
        lexer->line++;
        lexer->last_column = lexer->column;
        lexer->column = 0;
    } else {
        lexer->column++;
    }
    return c;
}

// Put character back
void unget_char(Lexer* lexer, char c) {
    ungetc(c, lexer->source);
    if (c == '\n') {
        lexer->line--;
        lexer->column = lexer->last_column;
    } else {
        lexer->column--;
    }
}

// Skip whitespace
void skip_whitespace(Lexer* lexer) {
    char c;
    while ((c = get_char(lexer)) != EOF && isspace(c));
    if (c != EOF) {
        unget_char(lexer, c);
    }
}

// Skip single-line comment
void skip_single_line_comment(Lexer* lexer) {
    char c;
    while ((c = get_char(lexer)) != EOF && c != '\n');
    if (c == '\n') {
        unget_char(lexer, c);
    }
}

// Skip multi-line comment
void skip_multi_line_comment(Lexer* lexer) {
    char c, prev = 0;
    while ((c = get_char(lexer)) != EOF) {
        if (prev == '*' && c == '/') break;
        prev = c;
    }
}

// Check if string is a keyword
int is_keyword(const char* str) {
    for (int i = 0; i < KEYWORDS_COUNT; i++) {
        if (strcmp(str, keywords[i]) == 0) {
            return 1;
        }
    }
    return 0;
}

// Get next token
Token get_next_token(Lexer* lexer) {
    Token token = {TOKEN_EOF, "", 0, 0};
    char c;
    int i;
    
    skip_whitespace(lexer);
    c = get_char(lexer);
    if (c == EOF) {
        return token;
    }
    
    token.line = lexer->line;
    token.column = lexer->column;
    
    // Handle preprocessor directives
    if (c == '#') {
        token.type = TOKEN_PREPROCESSOR;
        i = 0;
        token.lexeme[i++] = c;
        while ((c = get_char(lexer)) != EOF && c != '\n' && i < MAX_IDENTIFIER_LEN) {
            token.lexeme[i++] = c;
        }
        token.lexeme[i] = '\0';
        if (c != EOF) unget_char(lexer, c);
        return token;
    }
    
    // Handle comments
    if (c == '/') {
        char next = get_char(lexer);
        if (next == '/') {
            skip_single_line_comment(lexer);
            return get_next_token(lexer);
        } else if (next == '*') {
            skip_multi_line_comment(lexer);
            return get_next_token(lexer);
        } else {
            unget_char(lexer, next);
            token.type = TOKEN_OPERATOR;
            token.lexeme[0] = c;
            token.lexeme[1] = '\0';
            return token;
        }
    }
    
    // Handle identifiers and keywords
    if (isalpha(c) || c == '_') {
        i = 0;
        do {
            if (i < MAX_IDENTIFIER_LEN) token.lexeme[i++] = c;
            c = get_char(lexer);
        } while (isalnum(c) || c == '_');
        token.lexeme[i] = '\0';
        unget_char(lexer, c);
        
        token.type = is_keyword(token.lexeme) ? TOKEN_KEYWORD : TOKEN_IDENTIFIER;
        return token;
    }
    
    // Handle numbers
    if (isdigit(c)) {
        i = 0;
        do {
            if (i < MAX_NUMBER_LEN) token.lexeme[i++] = c;
            c = get_char(lexer);
        } while (isdigit(c) || c == '.' || c == 'e' || c == 'E' || 
                (c == '-' && (token.lexeme[i-1] == 'e' || token.lexeme[i-1] == 'E')));
        token.lexeme[i] = '\0';
        unget_char(lexer, c);
        token.type = TOKEN_NUMBER;
        return token;
    }
    
    // Handle strings
    if (c == '"') {
        i = 0;
        while ((c = get_char(lexer)) != EOF && c != '"') {
            if (c == '\\') {
                if (i < MAX_STRING_LEN) token.lexeme[i++] = c;
                c = get_char(lexer);
            }
            if (i < MAX_STRING_LEN) token.lexeme[i++] = c;
        }
        token.lexeme[i] = '\0';
        token.type = TOKEN_STRING;
        return token;
    }
    
    // Handle character literals
    if (c == '\'') {
        i = 0;
        while ((c = get_char(lexer)) != EOF && c != '\'') {
            if (c == '\\') {
                if (i < MAX_IDENTIFIER_LEN) token.lexeme[i++] = c;
                c = get_char(lexer);
            }
            if (i < MAX_IDENTIFIER_LEN) token.lexeme[i++] = c;
        }
        token.lexeme[i] = '\0';
        token.type = TOKEN_CHAR;
        return token;
    }
    
    // Handle operators
    if (strchr("+-*/%=<>!&|^~", c)) {
        token.type = TOKEN_OPERATOR;
        token.lexeme[0] = c;
        char next = get_char(lexer);
        
        // Check for two-character operators
        if ((c == '+' && next == '+') || (c == '-' && next == '-') ||
            (c == '=' && next == '=') || (c == '!' && next == '=') ||
            (c == '<' && next == '=') || (c == '>' && next == '=') ||
            (c == '&' && next == '&') || (c == '|' && next == '|') ||
            (c == '<' && next == '<') || (c == '>' && next == '>')) {
            token.lexeme[1] = next;
            token.lexeme[2] = '\0';
        } else {
            unget_char(lexer, next);
            token.lexeme[1] = '\0';
        }
        return token;
    }
    
    // Handle separators
    if (strchr("(){};,[]", c)) {
        token.type = TOKEN_SEPARATOR;
        token.lexeme[0] = c;
        token.lexeme[1] = '\0';
        return token;
    }
    
    // Handle unknown characters
    token.type = TOKEN_EOF;
    token.lexeme[0] = c;
    token.lexeme[1] = '\0';
    return token;
}

int main(int argc, char* argv[]) {
    const char* filename = "test.c"; // Replace "test.c" with your file name
    Lexer* lexer = init_lexer(filename);

    Token token;
    
    do {
        token = get_next_token(lexer);
        printf("Line %d, Column %d: ", token.line, token.column);
        
        switch (token.type) {
            case TOKEN_EOF:
                printf("EOF\n");
                break;
            case TOKEN_IDENTIFIER:
                printf("IDENTIFIER: %s\n", token.lexeme);
                break;
            case TOKEN_KEYWORD:
                printf("KEYWORD: %s\n", token.lexeme);
                break;
            case TOKEN_NUMBER:
                printf("NUMBER: %s\n", token.lexeme);
                break;
            case TOKEN_STRING:
                printf("STRING: %s\n", token.lexeme);
                break;
            case TOKEN_CHAR:
                printf("CHAR: %s\n", token.lexeme);
                break;
            case TOKEN_OPERATOR:
                printf("OPERATOR: %s\n", token.lexeme);
                break;
            case TOKEN_SEPARATOR:
                printf("SEPARATOR: %s\n", token.lexeme);
                break;
            case TOKEN_PREPROCESSOR:
                printf("PREPROCESSOR: %s\n", token.lexeme);
                break;
            case TOKEN_COMMENT:
                printf("COMMENT: %s\n", token.lexeme);
                break;
            default:
                printf("UNKNOWN\n");
        }
    } while (token.type != TOKEN_EOF);
    
    free_lexer(lexer);
    return 0;
}
