; Macro definition

; Program start
SECTION .text
    global _start

_start:
    ; Initialize registers
    MOV R1, 10      ; R1 = 10
    MOV R2, 20      ; R2 = 20
    
    ; Use the macro to add R1 and R2, store result in R3
ADD R1, R2
MOV R3, R1
    
    ; Use macro with different registers
    MOV R4, 30      ; R4 = 30
ADD R3, R4
MOV R5, R3
    
    ; Use macro with immediate values
ADD R5, #5
MOV R6, R5
    
    ; Exit program
    MOV R7, #1
    SWI 0
