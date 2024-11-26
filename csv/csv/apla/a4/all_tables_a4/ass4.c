#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAX_MACROS 10
#define MAX_INSTRUCTIONS 100
#define MAX_PARAMS 10
#define MAX_NAME_LENGTH 20

// Macro Name Table (MNT) entry structure
typedef struct {
    char macro_name[MAX_NAME_LENGTH];
    int mdt_index; // Starting index in MDT
    int param_count;
    int num_positional_params; // #PP
    int num_keyword_params; // #KP
    int num_expansion_time_vars; // #EV
    int kpd_ptr; // KPDTP
    int sst_ptr; // SSTP
} MNTEntry;

// Macro Definition Table (MDT) entry structure
typedef struct {
    char opcode[MAX_NAME_LENGTH];
    char operands[100];
} MDTEntry;

// Parameter Name Table (PNT) entry structure
typedef struct {
    char parameter[MAX_NAME_LENGTH];
    char placeholder[5];
} PNTEntry;

// Global tables and counters
MNTEntry MNT[MAX_MACROS];
MDTEntry MDT[MAX_INSTRUCTIONS];
PNTEntry PNT[MAX_MACROS][MAX_PARAMS];
int mnt_count = 0, mdt_count = 0, pnt_count[MAX_MACROS] = {0};

// Function prototypes
void define_macro(char *macro_name, char params[][MAX_NAME_LENGTH], int param_count, char instructions[][100], int instruction_count);
void print_MNT();
void print_MDT();
void print_PNT(int macro_index);
void replace_word(char *instruction, const char *word, const char *placeholder);

// Function to replace a word in an instruction with a placeholder
void replace_word(char *instruction, const char *word, const char *placeholder) {
    char temp[100];
    char *pos = strstr(instruction, word);

    while (pos) {
        if ((pos == instruction || *(pos - 1) == ' ') && 
            (*(pos + strlen(word)) == ' ' || *(pos + strlen(word)) == '\0')) {
            strncpy(temp, instruction, pos - instruction);
            temp[pos - instruction] = '\0';
            strcat(temp, placeholder);
            strcat(temp, pos + strlen(word));
            strcpy(instruction, temp);
        }
        pos = strstr(pos + 1, word);
    }
}

// Function to define a macro with parameter substitution
void define_macro(char *macro_name, char params[][MAX_NAME_LENGTH], int param_count, char instructions[][100], int instruction_count) {
    if (mnt_count >= MAX_MACROS || mdt_count + instruction_count >= MAX_INSTRUCTIONS) {
        printf("Error: Maximum limit of macros or instructions reached.\n");
        return;
    }

    // Fill MNT entry
    strcpy(MNT[mnt_count].macro_name, macro_name);
    MNT[mnt_count].mdt_index = mdt_count;
    MNT[mnt_count].param_count = param_count;
    MNT[mnt_count].num_positional_params = param_count; // Assuming all are positional
    MNT[mnt_count].num_keyword_params = 0; // Set to 0 for now
    MNT[mnt_count].num_expansion_time_vars = 0; // Set to 0 for now
    MNT[mnt_count].kpd_ptr = -1; // No keyword parameters
    MNT[mnt_count].sst_ptr = -1; // No sequence symbols

    // Fill Parameter Name Table (PNT)
    for (int i = 0; i < param_count; i++) {
        strcpy(PNT[mnt_count][i].parameter, params[i]);
        sprintf(PNT[mnt_count][i].placeholder, "#%d", i + 1);
    }
    pnt_count[mnt_count] = param_count;

    // Fill MDT entries
    for (int i = 0; i < instruction_count; i++) {
        char updated_instruction[100];
        strcpy(updated_instruction, instructions[i]);
        
        char opcode[MAX_NAME_LENGTH] = "";
        char operands[100] = "";

        // Correctly parse the instruction into opcode and operands
        sscanf(updated_instruction, "%s %[^\n]", opcode, operands);

        // Replace parameters with placeholders
        for (int j = 0; j < param_count; j++) {
            replace_word(updated_instruction, PNT[mnt_count][j].parameter, PNT[mnt_count][j].placeholder);
        }

        // Store in MDT - remember to correctly assign the extracted parts
        strcpy(MDT[mdt_count].opcode, opcode);
        strcpy(MDT[mdt_count].operands, operands);
        mdt_count++;
    }
    mnt_count++;
}

// Function to print MNT in a tabular format
void print_MNT() {
    printf("\nMacro Name Table (MNT):\n");
    printf("----------------------------------------------------------------------------------\n");
    printf("| Macro Name       | MDT Index | Param Count | #PP  | #KP  | #EV  | KPDTP | SSTP |\n");
    printf("----------------------------------------------------------------------------------\n");
    for (int i = 0; i < mnt_count; i++) {
        printf("| %-15s | %-9d | %-11d | %-4d | %-4d | %-4d | %-6d | %-5d |\n", 
               MNT[i].macro_name, MNT[i].mdt_index, MNT[i].param_count, 
               MNT[i].num_positional_params, MNT[i].num_keyword_params, 
               MNT[i].num_expansion_time_vars, MNT[i].kpd_ptr, MNT[i].sst_ptr);
    }
    printf("----------------------------------------------------------------------------------\n");
}

// Function to print MDT in a tabular format
void print_MDT() {
    printf("\nMacro Definition Table (MDT):\n");
    printf("-------------------------------------------------------\n");
    printf("| Opcode          | Operands       |\n");
    printf("-------------------------------------------------------\n");
    for (int i = 0; i < mdt_count; i++) {
        printf("| %-15s | %-15s |\n", MDT[i].opcode, MDT[i].operands);
    }
    printf("-------------------------------------------------------\n");
}

// Function to print PNT for a specific macro
void print_PNT(int macro_index) {
    printf("\nParameter Name Table (PNT) for macro '%s':\n", MNT[macro_index].macro_name);
    printf("-----------------------------------------------------\n");
    printf("| Parameter        | Placeholder |\n");
    printf("-----------------------------------------------------\n");
    for (int i = 0; i < pnt_count[macro_index]; i++) {
        printf("| %-15s | %-12s |\n", PNT[macro_index][i].parameter, PNT[macro_index][i].placeholder);
    }
    printf("-----------------------------------------------------\n");
}

// Function to read assembly code from a file and process macros
void process_assembly_file(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        printf("Error: Unable to open file %s\n", filename);
        return;
    }

    char line[200];
    char macro_name[MAX_NAME_LENGTH];
    char params[MAX_PARAMS][MAX_NAME_LENGTH];
    char instructions[MAX_INSTRUCTIONS][100];
    int instruction_count = 0;
    int param_count = 0;

    while (fgets(line, sizeof(line), file)) {
        // Remove newline character from the end of the line
        line[strcspn(line, "\n")] = 0;

        // Check for MACRO definition
        if (sscanf(line, "MACRO %s", macro_name) == 1) {
            param_count = 0;
            // Read parameters
            char *param_str = strchr(line, ' ') + strlen(macro_name) + 1;
            char *token = strtok(param_str, ", ");
            while (token != NULL && param_count < MAX_PARAMS) {
                strcpy(params[param_count++], token);
                token = strtok(NULL, ", ");
            }
            instruction_count = 0;
            // Read instructions until MEND
            while (fgets(line, sizeof(line), file)) {
                if (strstr(line, "MEND")) break;
                strcpy(instructions[instruction_count++], line);
            }
            // Define the macro
            define_macro(macro_name, params, param_count, instructions, instruction_count);
        }
    }

    fclose(file);
}

// Main function for testing
int main() {
    // Process assembly code from file
    process_assembly_file("assembly_code.asm");

    // Print the generated tables
    print_MNT();
    print_MDT();
    print_PNT(0); // Print PNT for the first macro

    return 0;
}
