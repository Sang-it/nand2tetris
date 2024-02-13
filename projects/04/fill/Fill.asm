    @24575
    D=A

    @0
    M=D

    @SCREEN
    D=A
    @1
    M=D

(LOOP)
    @KBD
    D=M
    @FILL
    D;JGT

    @CLEAR
    0;JMP

(FILL)
    @0
    D=M
    @1
    D=D-M
    @LOOP
    D;JLT

    @1
    D=M
    A=M
    M=-1

    @1
    M=D+1

    @LOOP
    0;JMP

(CLEAR)
    @SCREEN
    D=A
    @1
    D=D-M
    @LOOP
    D;JGT

    @1
    D=M
    A=M
    M=0

    @1
    M=D-1

    @LOOP
    0;JMP
