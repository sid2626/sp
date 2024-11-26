#include <iostream> 
#include <sstream>
#include <string> 
#include <vector> 
#include <map> 
#include <fstream> 
#include <iomanip> 
#include <algorithm>  // Required for std::remove 
using namespace std; 
// Data structures for tables 
struct Literal { 
string value; 
int address; 
}; 
map<string, int> symbol_table; 
vector<Literal> literal_table; 
vector<int> pool_table; 
vector<pair<int, string>> intermediate_code; 
vector<string> symbol_list; 
 
map<string, pair<string, int>> mot = { 
    {"START", {"AD", 1}}, 
    {"END", {"AD", 2}}, 
    {"ORIGIN", {"AD", 3}}, 
    {"LTORG", {"AD", 4}}, 
    {"MOVER", {"IS", 1}}, 
    {"ADD", {"IS", 2}}, 
    {"SUB", {"IS", 3}}, 
    {"STOP", {"IS", 4}}, 
    {"DC",{"DL",1}}, 
    {"DS",{"DL",2}} 
}; 
 
map<string, int> register_table = { 
    {"AREG", 1}, 
    {"BREG", 2}, 
    {"CREG", 3}, 
    {"DREG", 4} 
}; 
 
int lc = 0; 
int literal_index = 0; 
 
void processLine(const string& line) { 
    string tokens[3]; 
    int tokenIndex = 0; 
 
    // Tokenize the line 
    size_t start = 0; 
    size_t end = line.find(' '); 
    while (end != string::npos) { 
        tokens[tokenIndex++] = line.substr(start, end - start); 
        start = end + 1; 
        end = line.find(' ', start); 
    } 
    tokens[tokenIndex] = line.substr(start, end); 
 
    // Check for label 
    if (mot.find(tokens[0]) == mot.end()) { 
        // If symbol doesn't exist in the table, add it to both symbol_table and symbol_list 
        if (symbol_table.find(tokens[0]) == symbol_table.end()) { 
            symbol_table[tokens[0]] = lc; 
            symbol_list.push_back(tokens[0]); 
        } 
        tokens[0] = tokens[1]; 
        tokens[1] = tokens[2]; 
        tokens[2] = ""; 
    } 
 
    string mnemonic = tokens[0]; 
 
    if (mnemonic == "START") { 
        lc = stoi(tokens[1]); 
        intermediate_code.push_back({lc, "(AD,1) (C," + tokens[1] + ")"}); 
    } 
    else if (mnemonic == "END" || mnemonic == "LTORG") { 
        if (mnemonic == "LTORG") { 
            intermediate_code.push_back({lc, "(AD,4)"}); 
        } else { 
            intermediate_code.push_back({lc, "(AD,2)"}); 
        } 
 
        // Assign addresses to literals 
        while (literal_index < literal_table.size()) { 
            literal_table[literal_index].address = lc; 
            lc++; 
            literal_index++; 
        } 
        pool_table.push_back(literal_index); 
    } 
    else if (mnemonic == "ORIGIN") { 
        lc = stoi(tokens[1]); 
        intermediate_code.push_back({lc, "(AD,3) (C," + tokens[1] + ")"}); 
    } 
    else if (mot.find(mnemonic) != mot.end()) { 
        string opClass = mot[mnemonic].first; 
        int opcode = mot[mnemonic].second; 
 
        string operand1 = tokens[1]; 
        operand1.erase(std::remove(operand1.begin(), operand1.end(), ','), operand1.end()); 
 
        if (register_table.find(operand1) != register_table.end()) { 
            int reg_code = register_table[operand1]; 
            string operand2 = tokens[2]; 
 
            // Check if operand2 (symbol) exists in the symbol table 
            if (operand2[0] == '=') { 
                // It's a literal 
                literal_table.push_back({operand2, -1}); 
                int literal_ref = literal_table.size(); 
                intermediate_code.push_back({lc, "(IS," + to_string(opcode) + ") (R," + to_string(reg_code) + ") (L," + to_string(literal_ref) + ")"}); 
            } else { 
                // Check if symbol exists in symbol_table; if not, add it 
                if (symbol_table.find(operand2) == symbol_table.end()) { 
                    symbol_table[operand2] = lc;  // Assign current lc if symbol not already there 
                    symbol_list.push_back(operand2); 
                } 
                // Find the index of the symbol in symbol_list 
                int symbol_index = find(symbol_list.begin(), symbol_list.end(), operand2) - symbol_list.begin(); 
                intermediate_code.push_back({lc, "(IS," + to_string(opcode) + ") (R," + to_string(reg_code) + ") (S," + to_string(symbol_index + 1) + ")"}); 
            } 
        } 
        lc++; 
    } 
    else { 
        cerr << "Error: Invalid instruction " << mnemonic << endl; 
        exit(1); 
    } 
} 
 
void generateMachineCode() { 
    cout << "\nMachine Code:\n"; 
     
    for (const auto& ic : intermediate_code) { 
        int address = ic.first;      // Location counter (address) 
        string code = ic.second;     // IC code 
        stringstream ss(code); 
        string token; 
 
        // Extract (IS, X), (R, Y), (S, Z) or (L, Z) 
        ss >> token;  // The first part, e.g., (IS,1) 
        if (token.find("IS") != string::npos) { 
            // Imperative Statement (IS) 
            int opcode = stoi(token.substr(4, token.size() - 5));  // Extract opcode from (IS,X) 
 
            ss >> token;  // Next token, e.g., (R,1) or (C,5) 
            int reg_code = 0;  // Default to no register 
            if (token.find("R") != string::npos) { 
                reg_code = stoi(token.substr(3, token.size() - 4));  // Extract register code 
                ss >> token;  // Move to next token 
            } 
 
            // Handle symbol (S, X) or literal (L, X) 
            int operand_addr = 0; 
            if (token.find("S") != string::npos) { 
                int symbol_index = stoi(token.substr(3, token.size() - 4));  // Extract symbol index 
                operand_addr = symbol_table[symbol_list[symbol_index - 1]];  // Get the symbol's address 
            } else if (token.find("L") != string::npos) { 
                int literal_index = stoi(token.substr(3, token.size() - 4));  // Extract literal index 
                operand_addr = literal_table[literal_index - 1].address;      // Get the literal's address 
            } else if (token.find("C") != string::npos) { 
                operand_addr = stoi(token.substr(3, token.size() - 4));  // For constants 
            } 
 
            // Print the machine code: address, opcode, register code, operand address 
            cout << address << " " << setw(2) << setfill('0') << opcode << " " 
                 << reg_code << " " << setw(2) << setfill('0') << operand_addr << endl; 
        } 
        else if (token.find("DL") != string::npos) { 
            // Declarative Statement (DL) for DC (Define Constant) or DS (Define Storage) 
            int dl_code = stoi(token.substr(4, token.size() - 5));  // Extract DL code 
            if (dl_code == 1) {  // DC 
                ss >> token;  // (C, value) 
                int constant = stoi(token.substr(3, token.size() - 4)); 
                cout << address << " " << setw(2) << setfill('0') << dl_code << " 00 "  
                     << setw(2) << setfill('0') << constant << endl; 
            } else if (dl_code == 2) {  // DS (reserve space) 
                // We may not need to output anything special for DS since it's just reserving space 
                cout << address << " " << setw(2) << setfill('0') << dl_code << " 00 00" << endl; 
            } 
        } 
        // We skip (AD, X) since Assembler Directives don't generate machine code 
    } 
} 
 
 
 
int main() { 
    // Open the input file 
    ifstream inputFile("in.txt"); 
 
    if (!inputFile) { 
        cerr << "Error: Could not open input file!" << endl; 
        return 1; 
    } 
 
    string line; 
    pool_table.push_back(0); 
 
    // Read each line from the input file 
    while (getline(inputFile, line)) { 
        processLine(line); 
    } 
 
    inputFile.close(); 
 
    // Output Symbol Table 
    cout << "\nSymbol Table:\n"; 
    for (const auto& entry : symbol_table) { 
        cout << entry.first << "\t" << entry.second << endl; 
    } 
 
    // Output Literal Table 
    cout << "\nLiteral Table:\n"; 
    cout << "Index\tLiteral\tAddress\n"; 
    for (size_t i = 0; i < literal_table.size(); i++) { 
        cout << i << "\t" << literal_table[i].value << "\t" << literal_table[i].address << endl; 
    } 
 
    // Output Pool Table 
    cout << "\nPool Table:\n"; 
    for (int index : pool_table) { 
        cout << index << endl; 
    } 
 
    // Output Intermediate Code 
    cout << "\nIntermediate Code:\n"; 
    for (const auto& ic : intermediate_code) { 
        cout << ic.first << "\t" << ic.second << endl; 
    } 
 
generateMachineCode(); 
return 0; 
} 
   