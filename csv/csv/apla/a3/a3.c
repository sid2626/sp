#include <stdio.h>
#include <string.h>

struct MOTEntry {
    char mnemonic[10];
    char opcode[4];
    int size;
    char type[15];
};

struct LCEntry {
    int address;
    int value;
};

struct LiteralEntry {
    char value[10];
    int index;
};

struct ICEntry {
    char address[10];
    char code[50];
};

int getRegisterValue(char reg);
int isLiteral(const char *operand);
void machine_code(struct ICEntry *icTable, int icTableIndex, struct LiteralEntry *literalTable, int literalTableIndex, struct LCEntry *lcTable, int lcTableIndex);

int main() {
    FILE *file;
    char line[100];
    file = fopen("A2.txt", "r");
    if (file == NULL) {
        printf("File could not be opened.\n");
        return 1;
    }
    printf("Content of the 'ASM txt' file:\n");
    printf("------------------------------------\n");
    while (fgets(line, sizeof(line), file) != NULL) {
        printf("%s", line);
    }
    printf("\n");
    printf("------------------------------------\n");
    fclose(file);

    struct MOTEntry motTable[] = {
        {"START", "001", 1, "AD"},
        {"MOV", "01", 2, "IS"},
        {"ADD", "02", 2, "IS"},
        {"LTORG", "002", -1, "AD"},
        {"MUL", "03", 2, "IS"},
        {"ADD", "02", 2, "IS"},
        {"LTORG", "002", -1, "AD"},
        {"MOV", "01", 2, "IS"},
        {"ADD", "02", 2, "AD"},
        {"END", "003", 2, "AD"}
    };

    printf("\nMOT Table:\n");
    printf("-------------------------------------------\n");
    printf("| %-8s | %-6s | %-4s | %-12s |\n", "Mnemonic", "Opcode", "Size", "Type");
    printf("|----------|--------|------|--------------|\n");
    for (int i = 0; i < sizeof(motTable) / sizeof(motTable[0]); i++) {
        printf("| %-8s | %-6s | %-4d | %-12s |\n",
               motTable[i].mnemonic, motTable[i].opcode, motTable[i].size, motTable[i].type);
    }

    struct ICEntry icTable[100];
    int icTableIndex = 0;
    int addressCounter = 1000;

    struct LiteralEntry literalTable[100];
    int literalTableIndex = 0;

    int poolTable[100];
    int poolTableIndex = 0;

    struct LCEntry lcTable[100];
    int lcTableIndex = 0;
    int lcValue = 200;

    file = fopen("A2.txt", "r");
    if (file == NULL) {
        printf("File could not be opened.\n");
        return 1;
    }

    int literalCounter = 0;

    while (fgets(line, sizeof(line), file) != NULL) {
        icTable[icTableIndex].address[0] = '\0';

        char *operand = strchr(line, ',');
        if (operand) {
            *operand = '\0';
            operand++;
        }

        for (int i = 0; i < sizeof(motTable) / sizeof(motTable[0]); i++) {
            if (strstr(line, motTable[i].mnemonic)) {
                strcpy(icTable[icTableIndex].code, "");
                strcat(icTable[icTableIndex].code, "(");
                strcat(icTable[icTableIndex].code, motTable[i].type);
                strcat(icTable[icTableIndex].code, ",");
                strcat(icTable[icTableIndex].code, motTable[i].opcode);
                strcat(icTable[icTableIndex].code, ")");

                if (operand) {
                    int regValue = getRegisterValue(*operand);
                    if (regValue != 0) {
                        sprintf(icTable[icTableIndex].code + strlen(icTable[icTableIndex].code), " (R,%d)", regValue);
                    }
                }

                if (strcmp(motTable[i].type, "AD") == 0) {
                    sprintf(icTable[icTableIndex].code + strlen(icTable[icTableIndex].code), ",(%s)", motTable[i].opcode);
                    if (strcmp(motTable[i].mnemonic, "LTORG") != 0) {
                        sprintf(icTable[icTableIndex].address, "(C,%d)", addressCounter);
                        poolTable[poolTableIndex++] = literalCounter;
                    }
                }

                if (operand && isLiteral(operand)) {
                    strcpy(literalTable[literalTableIndex].value, operand + 1);
                    literalTable[literalTableIndex].index = literalCounter;
                    literalTableIndex++;
                    literalCounter++;
                }

                if (strcmp(motTable[i].mnemonic, "LTORG") == 0) {
                    while (poolTableIndex > 0) {
                        literalTable[poolTable[--poolTableIndex]].index = literalCounter;
                    }
                }

                addressCounter += motTable[i].size;
                break;
            }
        }

        lcTable[lcTableIndex].address = lcValue;
        lcTable[lcTableIndex].value = lcValue;

        for (int i = 0; i < sizeof(motTable) / sizeof(motTable[0]); i++) {
            if (strstr(line, motTable[i].mnemonic)) {
                lcValue += motTable[i].size;
                break;
            }
        }

        icTableIndex++;
        lcTableIndex++;
    }

    fclose(file);

    printf("\nIntermediate Code (IC) Table:\n");
    printf("---------------------------------------------------------\n");
    for (int i = 0; i < icTableIndex; i++) {
        printf("| %-25s | %-25s |\n", icTable[i].address, icTable[i].code);
    }

    printf("\nLC Table:\n");
    printf("-----------------\n");
    for (int i = 0; i < lcTableIndex; i++) {
        printf("| Address: %4d | Value: %4d |\n", lcTable[i].address, lcTable[i].value);
    }

    printf("\nLiteral Table:\n");
    printf("-----------------\n");
    for (int i = 0; i < literalTableIndex; i++) {
        printf("| Index: %4d | Value: %-5s |\n", literalTable[i].index, literalTable[i].value);
    }

    printf("\nPool Table:\n");
    printf("-----------------\n");
    for (int i = 0; i < poolTableIndex; i++) {
        printf("| Index: %4d |\n", poolTable[i]);
    }

   machine_code(icTable, icTableIndex, literalTable, literalTableIndex, lcTable, lcTableIndex);

    return 0;
}

int getRegisterValue(char reg) {
    if (reg == 'A') {
        return 1;
    } else if (reg == 'B') {
        return 2;
    } else if (reg == 'C') {
        return 3;
    }
    return 0;
}

int isLiteral(const char *operand) {
    return (operand[0] == '=');
}


void machine_code(struct ICEntry *icTable, int icTableIndex, struct LiteralEntry *literalTable, int literalTableIndex, struct LCEntry *lcTable, int lcTableIndex) {
    printf("\nMachine Code:\n");
    for (int i = 0; i < icTableIndex; i++) {
        char *code = icTable[i].code;
        char machineCode[20] = "";
        char mnemonic[10] = "";

        if (strstr(code, "(IS,")) {
            char opcode[3];
            sscanf(code, "(IS,%2s)", opcode);
            strcpy(machineCode, opcode);

            // Always add the LC value for IS instructions
            sprintf(machineCode + strlen(machineCode), " %d", lcTable[i].value);

            printf("%s\n", machineCode);
        } else if (strstr(code, "(AD,")) {
            sscanf(code, "(AD,%*[^,],%[^)])", mnemonic);
            // Remove the parentheses from the mnemonic
            mnemonic[strcspn(mnemonic, "(")] = '\0';
            printf("No machine code\n");
        }
    }
}
