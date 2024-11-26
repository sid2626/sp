#include <stdio.h>
#include <ctype.h>
#include <string.h>

#define KEYWORD_COUNT 32
#define MAX_BUFFER 100
#define MAX_LINE_LENGTH 256

// List of keywords in C
const char *keywords[KEYWORD_COUNT] = {
    "auto", "break", "case", "char", "const", "continue", "default", "do", "double", 
    "else", "enum", "extern", "float", "for", "goto", "if", "int", "long", "register", 
    "return", "short", "signed", "sizeof", "static", "struct", "switch", "typedef", 
    "union", "unsigned", "void", "volatile", "while"
};

// Checks if a string is a C keyword
int checkKeyword(const char *str) {
    for (int i = 0; i < KEYWORD_COUNT; i++) {
        if (strcmp(str, keywords[i]) == 0) {
            return 1;
        }
    }
    return 0;
}

// Determines if a character is a delimiter
int isDelimiterChar(char ch) {
    return (ch == ' ' || ch == '+' || ch == '-' || ch == '*' || ch == '/' || ch == ',' || 
            ch == ';' || ch == '>' || ch == '<' || ch == '=' || ch == '(' || ch == ')' || 
            ch == '[' || ch == ']' || ch == '{' || ch == '}' || ch == '\n' || ch == '\t');
}

// Determines if a character is an operator
int isOperatorChar(char ch) {
    return (ch == '+' || ch == '-' || ch == '*' || ch == '/' || ch == '>' || ch == '<' || 
            ch == '=' || ch == '&' || ch == '|' || ch == '!' || ch == '%');
}

// Checks if a string is a valid identifier
int isValidIdentifier(const char *str) {
    return !(isdigit(str[0]) || isDelimiterChar(str[0]));
}

// Determines if a string is a numeric constant
int isNumericConstant(const char *str) {
    int hasDecimalPoint = 0;
    for (int i = 0; i < strlen(str); i++) {
        if (str[i] == '.') {
            if (hasDecimalPoint) return 0;
            hasDecimalPoint = 1;
        } else if (!isdigit(str[i])) {
            return 0;
        }
    }
    return 1;
}

// Determines if a string is a multi-character operator
int isMultiCharOperator(const char *str) {
    return (strcmp(str, "++") == 0 || strcmp(str, "--") == 0 || strcmp(str, "==") == 0 || 
            strcmp(str, "!=") == 0 || strcmp(str, ">=") == 0 || strcmp(str, "<=") == 0 || 
            strcmp(str, "&&") == 0 || strcmp(str, "||") == 0 || strcmp(str, "+=") == 0 || 
            strcmp(str, "-=") == 0 || strcmp(str, "*=") == 0 || strcmp(str, "/=") == 0);
}

// Skips single and multi-line comments
void skipComments(char *str, int *index) {
    if (str[*index] == '/' && str[*index + 1] == '/') {
        while (str[*index] != '\n') (*index)++;
    } else if (str[*index] == '/' && str[*index + 1] == '*') {
        *index += 2;
        while (!(str[*index] == '*' && str[*index + 1] == '/')) {
            (*index)++;
        }
        *index += 2;
    }
}

// Tokenizes and categorizes a given line of code
void processTokens(FILE *output_file, char *str) {
    char buffer[MAX_BUFFER];
    int j = 0;
    for (int i = 0; i < strlen(str); i++) {
        // Skip comments
        if (str[i] == '/' && (str[i + 1] == '/' || str[i + 1] == '*')) {
            skipComments(str, &i);
            continue;
        }
        if (isDelimiterChar(str[i])) {
            if (j != 0) {
                buffer[j] = '\0';
                j = 0;
                // Categorize the token
                if (checkKeyword(buffer)) {
                    fprintf(output_file, "'%s' is a keyword\n", buffer);
                } else if (isNumericConstant(buffer)) {
                    fprintf(output_file, "'%s' is a numeric constant\n", buffer);
                } else if (isValidIdentifier(buffer)) {
                    fprintf(output_file, "'%s' is an identifier\n", buffer);
                }
            }
            // Check for multi-character operators
            if (isOperatorChar(str[i]) && isOperatorChar(str[i + 1])) {
                char op[3] = {str[i], str[i + 1], '\0'};
                if (isMultiCharOperator(op)) {
                    fprintf(output_file, "'%s' is a multi-character operator\n", op);
                    i++;
                }
            } else if (isOperatorChar(str[i])) {
                fprintf(output_file, "'%c' is a single-character operator\n", str[i]);
            } else if (!isspace(str[i])) {
                fprintf(output_file, "'%c' is a special character\n", str[i]);
            }
        } else if (str[i] == '"' || str[i] == '\'') {
            char quoteType = str[i];
            fprintf(output_file, "%c", quoteType);
            i++;
            while (str[i] != quoteType) {
                fprintf(output_file, "%c", str[i]);
                i++;
            }
            fprintf(output_file, "%c is a string/character constant\n", quoteType);
        } else {
            buffer[j++] = str[i];
        }
    }
}

int main() {
    char filename[100];
    char outputFilename[] = "assignment6.txt";
    FILE *file, *output_file;
    char line[MAX_LINE_LENGTH];

    // Prompt user for input file name
    printf("Enter the C source file name: ");
    scanf("%s", filename);

    // Open input file
    file = fopen(filename, "r");
    if (file == NULL) {
        printf("Error: Unable to open file %s\n", filename);
        return 1;
    }

    // Open output file
    output_file = fopen(outputFilename, "w");
    if (output_file == NULL) {
        printf("Error: Unable to open output file %s\n", outputFilename);
        fclose(file);
        return 1;
    }

    fprintf(output_file, "Lexical Analysis Results:\n");

    // Process each line in the file
    while (fgets(line, sizeof(line), file)) {
        processTokens(output_file, line);
    }

    fclose(file);
    fclose(output_file);

    printf("Lexical analysis results saved to %s\n", outputFilename);
    return 0;
}
