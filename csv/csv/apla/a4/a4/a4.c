#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAX_PARAMS 10
#define MAX_LINES 50
#define MAX_NAME 30
#define MAX_LINE_LENGTH 100

// Macro Name Table (MNT) structure
typedef struct {
    char name[MAX_NAME];
    int mdt_index;
} MNT_Entry;

// Parameter Table (PNTAB) structure
typedef struct {
    char param_name[MAX_NAME];
} PNTAB_Entry;

// Macro Definition Table (MDT) structure
typedef struct {
    char definition[MAX_LINE_LENGTH];
} MDT_Entry;

// Global tables
MNT_Entry MNT[MAX_LINES];
PNTAB_Entry PNTAB[MAX_PARAMS];
MDT_Entry MDT[MAX_LINES];

// Global counters
int mnt_count = 0;
int pntab_count = 0;
int mdt_count = 0;

void process_macro(FILE *input_file) {
    char line[MAX_LINE_LENGTH];
    char macro_name[MAX_NAME];
    int in_macro = 0;

    while (fgets(line, sizeof(line), input_file)) {
        // Remove newline character if present
        line[strcspn(line, "\n")] = 0;

        if (strncmp(line, "MACRO", 5) == 0) {
            in_macro = 1;
            continue;
        }

        if (in_macro) {
            if (strncmp(line, "MEND", 4) == 0) {
                in_macro = 0;
                continue;
            }

            if (mnt_count == 0) {
                // First line after MACRO is the macro name and parameters
                char *token = strtok(line, " \t");
                strcpy(macro_name, token);
                strcpy(MNT[mnt_count].name, macro_name);
                MNT[mnt_count].mdt_index = mdt_count;
                mnt_count++;

                // Process parameters
                while ((token = strtok(NULL, " ,\t")) != NULL) {
                    if (token[0] == '&') {
                        strcpy(PNTAB[pntab_count].param_name, token);
                        pntab_count++;
                    }
                }
            } else {
                // Add line to MDT
                strcpy(MDT[mdt_count].definition, line);
                mdt_count++;
            }
        }
    }
}

void print_tables() {
    printf("Macro Name Table (MNT):\n");
    printf("Name\tMDT Index\n");
    for (int i = 0; i < mnt_count; i++) {
        printf("%s\t%d\n", MNT[i].name, MNT[i].mdt_index);
    }

    printf("\nParameter Table (PNTAB):\n");
    for (int i = 0; i < pntab_count; i++) {
        printf("%s\n", PNTAB[i].param_name);
    }

    printf("\nMacro Definition Table (MDT):\n");
    printf("Index\tDefinition\n");
    for (int i = 0; i < mdt_count; i++) {
        printf("%d\t%s\n", i, MDT[i].definition);
    }
}

int main() {
    FILE *input_file = fopen("macro_input.asm", "r");
    if (input_file == NULL) {
        printf("Error opening file.\n");
        return 1;
    }

    process_macro(input_file);
    fclose(input_file);

    print_tables();

    return 0;
}
