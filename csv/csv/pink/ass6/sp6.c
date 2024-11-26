#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

const char *keywords[] = {
    "auto", "break", "case", "char", "const", "continue", "default",
    "do", "double", "else", "enum", "extern", "float", "for", "goto", "if",
    "int", "long", "register", "return", "short", "signed", "sizeof", "static",
    "struct", "switch", "typedef", "union", "unsigned", "void", "volatile", "while"
};

const char *operators[] = {
    "==", "&&", "||", "++", "--", "<=", "!=", "=", ">", "<", "!", "%",
    "*", "+", "-", "/", "&&", "||"
};

const char *delimiters[] = {
    ";", ",", "(", ")", "{", "}", "[", "]"
};

char *symbol_table[1000];
int symbol_table_size = 0;
char *constant_table[1000];
int constant_table_size = 0;

int get_keyword_index(char *lexeme) {
    for (int i = 0; i < sizeof(keywords) / sizeof(keywords[0]); i++) {
        if (strcmp(lexeme, keywords[i]) == 0) {
            return i;
        }
    }
    return -1;
}

int get_identifier_index(char *lexeme) {
    for (int i = 0; i < symbol_table_size; i++) {
        if (strcmp(lexeme, symbol_table[i]) == 0) {
            return i;
        }
    }
    symbol_table[symbol_table_size++] = strdup(lexeme);
    return symbol_table_size - 1;
}

int get_constant_index(char *lexeme) {
    for (int i = 0; i < constant_table_size; i++) {
        if (strcmp(lexeme, constant_table[i]) == 0) {
            return i;
        }
    }
    constant_table[constant_table_size++] = strdup(lexeme);
    return constant_table_size - 1;
}

int get_operator_index(char *op) {
    for (int i = 0; i < sizeof(operators) / sizeof(operators[0]); i++) {
        if (strcmp(op, operators[i]) == 0) {
            return i;
        }
    }
    return -1;
}

int get_delimiter_index(char *delimiter) {
    for (int i = 0; i < sizeof(delimiters) / sizeof(delimiters[0]); i++) {
        if (strcmp(delimiter, delimiters[i]) == 0) {
            return i;
        }
    }
    return -1;
}

// Tokenizing function
void analyze_file(char *file_name) {
    FILE *fp = fopen(file_name, "r");
    if (fp == NULL) {
        printf("Error: Could not open file %s\n", file_name);
        return;
    }

    char line[1024];
    int line_no = 0;

    while (fgets(line, sizeof(line), fp)) {
        line_no++;
        char *token = strtok(line, " \t\n\r");
        
        while (token != NULL) {
            int i = 0;
            while (token[i] != '\0') {
                char ch = token[i];
                
                // Check for operators
                if (strchr("+-*/=><!%&|", ch) != NULL) {
                    char op[3] = {ch, '\0', '\0'};
                    if (i < strlen(token) - 1 && strchr("=><!&|", token[i+1]) != NULL) {
                        op[1] = token[++i];
                    }
                    int operator_index = get_operator_index(op);
                    if (operator_index != -1) {
                        printf("%d | %s | Operator | %d\n", line_no, op, operator_index);
                    }
                }
                // Check for delimiters
                else if (strchr(";(),{}[]", ch) != NULL) {
                    char delim[2] = {ch, '\0'};
                    int delimiter_index = get_delimiter_index(delim);
                    if (delimiter_index != -1) {
                        printf("%d | %c | Delimiter | %d\n", line_no, ch, delimiter_index);
                    }
                }
                // Check for identifiers and keywords
                else if (isalpha(ch) || ch == '_') {
                    char identifier[100];
                    int j = 0;
                    while (isalnum(token[i]) || token[i] == '_') {
                        identifier[j++] = token[i++];
                    }
                    identifier[j] = '\0';
                    int keyword_index = get_keyword_index(identifier);
                    if (keyword_index != -1) {
                        printf("%d | %s | Keyword | %d\n", line_no, identifier, keyword_index);
                    } else {
                        int identifier_index = get_identifier_index(identifier);
                        printf("%d | %s | Identifier | %d\n", line_no, identifier, identifier_index);
                    }
                    i--; // Adjust loop counter
                }
                // Check for constants
                else if (isdigit(ch)) {
                    char constant[100];
                    int j = 0;
                    while (isdigit(token[i])) {
                        constant[j++] = token[i++];
                    }
                    constant[j] = '\0';
                    int constant_index = get_constant_index(constant);
                    printf("%d | %s | Constant | %d\n", line_no, constant, constant_index);
                    i--; // Adjust loop counter
                }
                i++;
            }
            token = strtok(NULL, " \t\n\r");
        }
    }

    fclose(fp);

    // Print symbol, constant, operator, and delimiter tables
    printf("\nSymbol Table:\n");
    for (int i = 0; i < symbol_table_size; i++) {
        printf("%d | %s\n", i, symbol_table[i]);
    }

    printf("\nConstant Table:\n");
    for (int i = 0; i < constant_table_size; i++) {
        printf("%d | %s\n", i, constant_table[i]);
    }

    printf("\nOperators:\n");
    for (int i = 0; i < sizeof(operators) / sizeof(operators[0]); i++) {
        printf("%d | %s\n", i, operators[i]);
    }

    printf("\nDelimiters:\n");
    for (int i = 0; i < sizeof(delimiters) / sizeof(delimiters[0]); i++) {
        printf("%d | %s\n", i, delimiters[i]);
    }

    printf("\nKeywords:\n");
    for (int i = 0; i < sizeof(keywords) / sizeof(keywords[0]); i++) {
        printf("%d | %s\n", i, keywords[i]);
    }
}

int main() {
    analyze_file("input.c");
    return 0;
}
