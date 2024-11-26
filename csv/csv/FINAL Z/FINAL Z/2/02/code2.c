#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAX_SYMBOLS 100
#define MAX_LITERALS 100
#define MAX_POOLS 10
#define MAX_CODE_LINES 100
#define MAX_LC_ENTRIES 100

// Data Structures
typedef struct {
    char symbol[10];
    int address;
} Symbol;

typedef struct {
    char literal[10];
    int address;
} Literal;

typedef struct {
    int start_idx;
} Pool;

typedef struct {
    int line_no;
    int address;
    char instruction[50];  // Store the instruction for reference
} LC_Entry;

// Global Tables
Symbol symtab[MAX_SYMBOLS];
int symtab_count = 0;

Literal littab[MAX_LITERALS];
int littab_count = 0;

Pool pooltab[MAX_POOLS];
int pooltab_count = 1;

LC_Entry lc_table[MAX_LC_ENTRIES];
int lc_count = 0;

char intermediate_code[MAX_CODE_LINES][50];
int intermediate_count = 0;

int location_counter = 0;
int line_counter = 0;

// Function to add an entry to the LC table
void add_lc_entry(char *instruction) {
    lc_table[lc_count].line_no = ++line_counter;
    lc_table[lc_count].address = location_counter;
    strcpy(lc_table[lc_count].instruction, instruction);
    lc_count++;
}

// Function to add a symbol to the symbol table
void add_symbol(char *symbol, int address) {
    strcpy(symtab[symtab_count].symbol, symbol);
    symtab[symtab_count].address = address;
    symtab_count++;
}

// Function to add a literal to the literal table
void add_literal(char *literal) {
    strcpy(littab[littab_count].literal, literal);
    littab[littab_count].address = -1; // Address will be assigned during LTORG or END
    littab_count++;
}

// Function to process assembler directives and instructions
void process_line(char *line) {
    char token1[10], token2[10], token3[10];
    sscanf(line, "%s %s %s", token1, token2, token3);

    // Add to LC table before processing
    add_lc_entry(line);

    if (strcmp(token1, "START") == 0) {
        location_counter = atoi(token2);
    } else if (strcmp(token1, "END") == 0) {
        // Handle LTORG equivalent at the end
        for (int i = pooltab[pooltab_count - 1].start_idx; i < littab_count; i++) {
            littab[i].address = location_counter;
            sprintf(intermediate_code[intermediate_count++], "%d (LIT, %s)", location_counter, littab[i].literal);
            location_counter++;
        }
    } else if (strcmp(token1, "LTORG") == 0) {
        for (int i = pooltab[pooltab_count - 1].start_idx; i < littab_count; i++) {
            littab[i].address = location_counter;
            sprintf(intermediate_code[intermediate_count++], "%d (LIT, %s)", location_counter, littab[i].literal);
            location_counter++;
        }
        pooltab[pooltab_count++].start_idx = littab_count;
    } else if (strcmp(token2, "DC") == 0) {
        add_symbol(token1, location_counter);
        sprintf(intermediate_code[intermediate_count++], "%d (DC, %s)", location_counter, token3);
        location_counter++;
    } else if (strcmp(token2, "DS") == 0) {
        add_symbol(token1, location_counter);
        int size = atoi(token3);
        sprintf(intermediate_code[intermediate_count++], "%d (DS, %d)", location_counter, size);
        location_counter += size;
    } else if (strcmp(token1, "MOV") == 0 || strcmp(token1, "ADD") == 0 || strcmp(token1, "SUB") == 0) {
        sprintf(intermediate_code[intermediate_count++], "%d (%s, %s, %s)", location_counter, token1, token2, token3);
        location_counter++;
    } else if (token1[0] == '=') {
        add_literal(token1);
    } else {
        printf("Error: Undefined instruction %s\n", token1);
    }
}

// Function to display tables and intermediate code
void display(FILE *output) {
    fprintf(output, "\nLocation Counter (LC) Table:\n");
    fprintf(output, "-----------------------------------------\n");
    for (int i = 0; i < lc_count; i++) {
        fprintf(output, "| Line No: %2d | Address: %3d | Instruction: %s |\n", 
                lc_table[i].line_no, lc_table[i].address, lc_table[i].instruction);
        fprintf(output, "-----------------------------------------\n");
    }

    fprintf(output, "\nSymbol Table:\n");
    for (int i = 0; i < symtab_count; i++) {
        fprintf(output, "%s\t%d\n", symtab[i].symbol, symtab[i].address);
    }

    fprintf(output, "\nLiteral Table:\n");
    for (int i = 0; i < littab_count; i++) {
        fprintf(output, "%s\t%d\n", littab[i].literal, littab[i].address);
    }

    fprintf(output, "\nPool Table:\n");
    for (int i = 0; i < pooltab_count; i++) {
        fprintf(output, "%d\n", pooltab[i].start_idx);
    }

    fprintf(output, "\nIntermediate Code:\n");
    for (int i = 0; i < intermediate_count; i++) {
        fprintf(output, "%s\n", intermediate_code[i]);
    }
}

int main() {
    // Open the input file
    FILE *input = fopen("02input.asm", "r");
    if (input == NULL) {
        printf("Error: Unable to open input.asm\n");
        return 1;
    }

    // Open the output file
    FILE *output = fopen("02output.txt", "w");
    if (output == NULL) {
        printf("Error: Unable to create 02output.txt\n");
        fclose(input);
        return 1;
    }

    // Process each line in the input file
    char line[50];
    while (fgets(line, sizeof(line), input)) {
        line[strcspn(line, "\n")] = '\0'; // Remove newline character
        process_line(line);
    }

    // Display the tables and intermediate code
    display(output);

    // Close files
    fclose(input);
    fclose(output);

    return 0;
}