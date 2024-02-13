    @2
    M=0

    @0
    D=M

    @END
    D;JLT

(LOOP)
    @1
    M=M-1
    D=M
    @END
    D;JLT

    @0
    D=M
    @2
    M=M+D

    @LOOP
    0;JMP

(END)
    @END
    0;JMP
