#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_LINE 1024
#define MAX_PARAMS 3
#define MAX_PARAM_LEN 32

// Function to trim whitespace from a string
void trim(char *str) {
    char *start = str;
    char *end = str + strlen(str) - 1;
    
    while (isspace(*start)) start++;
    while (end > start && isspace(*end)) end--;
    
    *(end + 1) = '\0';
    memmove(str, start, strlen(start) + 1);
}

// Function to parse macro parameters from definition
int parse_macro_def(char *line, char params[MAX_PARAMS][MAX_PARAM_LEN]) {
    char *token;
    int count = 0;
    
    // Skip macro name
    token = strtok(line, ",");
    if (token) {
        char *space = strchr(token, ' ');
        if (space) token = space + 1;
        trim(token);
        if (token[0] == '&') {
            strncpy(params[count], token + 1, MAX_PARAM_LEN - 1);
            params[count][MAX_PARAM_LEN - 1] = '\0';
            count++;
        }
    }
    
    // Get remaining parameters
    while ((token = strtok(NULL, ",")) && count < MAX_PARAMS) {
        trim(token);
        if (token[0] == '&') {
            strncpy(params[count], token + 1, MAX_PARAM_LEN - 1);
            params[count][MAX_PARAM_LEN - 1] = '\0';
            count++;
        }
    }
    
    return count;
}

// Function to parse macro call parameters
int parse_macro_call(char *line, char values[MAX_PARAMS][MAX_PARAM_LEN]) {
    char *token;
    int count = 0;
    
    // Skip macro name
    token = strtok(line, ",");
    if (token) {
        char *space = strchr(token, ' ');
        if (space) token = space + 1;
        trim(token);
        strncpy(values[count], token, MAX_PARAM_LEN - 1);
        values[count][MAX_PARAM_LEN - 1] = '\0';
        count++;
    }
    
    // Get remaining parameters
    while ((token = strtok(NULL, ",")) && count < MAX_PARAMS) {
        trim(token);
        strncpy(values[count], token, MAX_PARAM_LEN - 1);
        values[count][MAX_PARAM_LEN - 1] = '\0';
        count++;
    }
    
    return count;
}

// Function to store macro body
typedef struct {
    char lines[MAX_PARAMS][MAX_LINE];
    int line_count;
} MacroBody;

// Function to replace parameters in a line
void replace_params(char *line, char params[MAX_PARAMS][MAX_PARAM_LEN], 
                   char values[MAX_PARAMS][MAX_PARAM_LEN], int param_count) {
    char result[MAX_LINE];
    strncpy(result, line, MAX_LINE);
    
    for (int i = 0; i < param_count; i++) {
        char param[MAX_PARAM_LEN + 2];
        snprintf(param, sizeof(param), "&%s", params[i]);
        
        char *pos = strstr(result, param);
        while (pos) {
            // Replace parameter with value
            memmove(pos + strlen(values[i]), 
                   pos + strlen(param), 
                   strlen(pos + strlen(param)) + 1);
            memcpy(pos, values[i], strlen(values[i]));
            pos = strstr(result, param);
        }
    }
    
    strcpy(line, result);
}

int main() {
    const char *input_file = "input.asm";
    const char *output_file = "output.asm";
    
    FILE *input = fopen(input_file, "r");
    if (!input) {
        printf("Error opening input file: %s\n", input_file);
        return 1;
    }
    
    FILE *output = fopen(output_file, "w");
    if (!output) {
        fclose(input);
        printf("Error creating output file: %s\n", output_file);
        return 1;
    }
    
    char line[MAX_LINE];
    int in_macro = 0;
    char macro_params[MAX_PARAMS][MAX_PARAM_LEN] = {0};
    int param_count = 0;
    MacroBody macro_body = {0};
    
    while (fgets(line, sizeof(line), input)) {
        char temp_line[MAX_LINE];
        strncpy(temp_line, line, MAX_LINE);
        trim(temp_line);
        
        if (strncmp(temp_line, "ADD_VAL", 7) == 0 && !in_macro) {
            // Handle macro call
            char values[MAX_PARAMS][MAX_PARAM_LEN] = {0};
            int value_count = parse_macro_call(temp_line, values);
            
            // Output expanded macro with replaced parameters
            for (int i = 0; i < macro_body.line_count; i++) {
                char expanded_line[MAX_LINE];
                strncpy(expanded_line, macro_body.lines[i], MAX_LINE);
                replace_params(expanded_line, macro_params, values, param_count);
                fprintf(output, "%s\n", expanded_line);
            }
        } else if (strncmp(temp_line, "MACRO", 5) == 0) {
            in_macro = 1;
            // Get next line for macro definition
            if (fgets(line, sizeof(line), input)) {
                strncpy(temp_line, line, MAX_LINE);
                trim(temp_line);
                param_count = parse_macro_def(temp_line, macro_params);
            }
            macro_body.line_count = 0;
        } else if (strncmp(temp_line, "MEND", 4) == 0) {
            in_macro = 0;
        } else if (in_macro && strlen(temp_line) > 0) {
            // Store macro body
            strncpy(macro_body.lines[macro_body.line_count++], temp_line, MAX_LINE);
        } else if (!in_macro) {
            fputs(line, output);
        }
    }
    
    fclose(input);
    fclose(output);
    printf("Macro expansion completed. Output written to %s\n", output_file);
    
    return 0;
}