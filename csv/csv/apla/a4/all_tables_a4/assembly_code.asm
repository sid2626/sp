

MACRO SWAP X, Y
    LOAD X       ; Load value of X into the accumulator
    STORE TEMP   ; Temporarily store value of X
    LOAD Y       ; Load value of Y into the accumulator
    STORE X      ; Store value of Y into X
    LOAD TEMP    ; Load the temporarily stored value of X
    STORE Y      ; Store it back in Y
MEND



LOAD A           ; Load value of A
ADD B            ; Add value of B
STORE A          ; Store result back in A
    

SWAP X, Y       

END
