/*
 * Click nbfs://nbhost/SystemFileSystem/Templates/Licenses/license-default.txt to change this license
 * Click nbfs://nbhost/SystemFileSystem/Templates/Classes/Class.java to edit this template
 */



import java.util.*;
import java.io.*;
public class MacroP1 {
public static void main(String[] args) throws IOException {
BufferedReader br = new BufferedReader(new FileReader("macro_input.asm"));
FileWriter mnt = new FileWriter("mnt.txt");
FileWriter mdt = new FileWriter("mdt.txt");
FileWriter kpdt = new FileWriter("kpdt.txt");
FileWriter pnt = new FileWriter("pntab.txt");
FileWriter evnt = new FileWriter("evntab.txt");
FileWriter sstab = new FileWriter("sstab.txt");
FileWriter ssntab = new FileWriter("ssntab.txt");
FileWriter ir = new FileWriter("intermediate.txt");
LinkedHashMap<String, Integer> pntab = new LinkedHashMap<>();
LinkedHashMap<String, Integer> evntab = new LinkedHashMap<>();
List<String> sstabList = new ArrayList<>();
List<String> ssntabList = new ArrayList<>();
boolean inMacro = false;
String currentMacroName = null;
String line;
int mdtp = 1, kpdtp = 0, paramNo = 1, pp = 0, kp = 0, flag = 0;
while ((line = br.readLine()) != null) {
String parts[] = line.split("\\s+");
if (parts.length == 0) continue; // Skip empty lines
if (parts[0].equalsIgnoreCase("MACRO")) {
if (inMacro) {
System.out.println("Error: Previous macro not ended with MEND before starting a newmacro.");
continue;
}
flag = 1;
inMacro = true;
line = br.readLine();
if (line == null) {
System.out.println("Error: No macro definition found after MACRO directive.");
continue;
}
parts = line.split("\\s+");
currentMacroName = parts[0];
if (parts.length <= 1) {
System.out.println("Error: Macro definition for " + currentMacroName + " is missing parameters.");
mnt.write(String.format("%-10s %-10d %-10d %-10d %-10d %-10s %-10s %-10s\n"
,
currentMacroName, pp, kp, mdtp, (kp == 0 ? kpdtp : (kpdtp + 1)),
"null"
,
"null"
,
"null"));
continue;
}
// Process parameters and store in EVNTAB
for (int i = 1; i < parts.length; i++) {
parts[i] = parts[i].replaceAll("[&,]"
,
"");
if (parts[i].contains("=")) {
++kp;
String keywordParam[] = parts[i].split("=");
pntab.put(keywordParam[0], paramNo++);
if (keywordParam.length == 2) {
kpdt.write(keywordParam[0] + "\t" + keywordParam[1] + "\n");
} else {
kpdt.write(keywordParam[0] + "\t-\n");
}
} else {
pntab.put(parts[i], paramNo++);
pp++;
}
}
// Write MNT entry, EVNTAB only if there are expansion variables
mnt.write(String.format("%-10s %-10d %-10d %-10d %-10d %-10s %-10s %-10s\n"
,
currentMacroName, pp, kp, mdtp, (kp == 0 ? kpdtp : (kpdtp + 1)),
(evntab.isEmpty() ? "null" : evntab.toString()),
"null"
,
"null"));
kpdtp = kpdtp + kp;
} else if (parts[0].equalsIgnoreCase("MEND")) {
if (!inMacro) {
System.out.println("Error: MEND directive found without a matching MACRO directive.");
continue;
}
mdt.write(line + "\n");
flag = kp = pp = 0;
mdtp++;
paramNo = 1;
// Writing the PNTAB (Parameter Name Table)
pnt.write(currentMacroName + ":\t");
for (String param : pntab.keySet()) {
pnt.write(param + "\t");
}
pnt.write("\n");
// Writing the EVNTAB (if there are expansion variables)
if (!evntab.isEmpty()) {
evnt.write(currentMacroName + ":\t");
for (String param : evntab.keySet()) {
evnt.write(param + "\t");
}
} else {
evnt.write("\n");
evnt.write(currentMacroName + ":\tnull\n"); // Write null if no EV declared
}
pntab.clear();
evntab.clear();
inMacro = false;
} else if (flag == 1) {
for (int i = 0; i < parts.length; i++) {
if (parts[i].contains("&")) {
parts[i] = parts[i].replaceAll("[&,]"
,
"");
mdt.write("(P," + pntab.get(parts[i]) + ")\t");
} else {
mdt.write(parts[i] + "\t");
// Handle sequence symbols (dummy labels)
if (parts[i].matches("^L[0-9]+$")) {
sstabList.add(parts[i]);
ssntabList.add(currentMacroName);
}
}
}
mdt.write("\n");
mdtp++;
} else {
ir.write(line + "\n");
}
}
if (inMacro) {
System.out.println("Error: Macro definition " + currentMacroName + " is not ended with MEND.");
}
mdt.close();
mnt.close();
ir.close();
pnt.close();
kpdt.close();
evnt.close();
// Write SSTAB and SSNTAB
for (String sstabEntry : sstabList) {
sstab.write(sstabEntry + "\n");
}
for (String ssnEntry : ssntabList) {
ssntab.write(ssnEntry + "\n");
}
sstab.close();
ssntab.close();
// Print all tables in sequence after processing
System.out.println("Macro Pass 1 Processing done.\n");
// Print MNT (Macro Name Table)
System.out.println("MNT (Macro Name Table):");
printFile("mnt.txt");
// Print MDT (Macro Definition Table)
System.out.println("\nMDT (Macro Definition Table):");
printFile("mdt.txt");
// Print KPDT (Keyword Parameter Default Table)
System.out.println("\nKPDT (Keyword Parameter Default Table):");
printFile("kpdt.txt");
// Print PNTAB (Parameter Name Table)
System.out.println("\nPNTAB (Parameter Name Table):");
printFile("pntab.txt");
// Print EVNTAB (Expansion Variable Name Table)
System.out.println("\nEVNTAB (Expansion Variable Name Table):");
printFile("evntab.txt");
// Print SSTAB (Sequence Symbol Table)
System.out.println("\nSSTAB (Sequence Symbol Table):");
printFile("sstab.txt");
// Print SSNTAB (Sequence Symbol Name Table)
System.out.println("\nSSNTAB (Sequence Symbol Name Table):");
printFile("ssntab.txt");
// Print Intermediate Code
System.out.println("\nIntermediate Code:");
printFile("intermediate.txt");
}
// Utility method to print the contents of a file
public static void printFile(String fileName) throws IOException {
BufferedReader fileReader = new BufferedReader(new FileReader(fileName));
String fileLine;
while ((fileLine = fileReader.readLine()) != null) {
System.out.println(fileLine);
}
fileReader.close();
}
}