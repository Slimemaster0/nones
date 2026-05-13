enum Instruction {
    BRK,
    JSR,
    RTI,
    RTS,
    NOP,
    LDY,
    CPY,
    CPX,
    ORA,
    AND,
    EOR,
    ADC,
    STA,
    LDA,
    CMP,
    SBC,
    STP,
    LDX,
    SLO,
    RLA,
    SRE,
    RRA,
    SAX,
    LAX,
    DCP,
    ISC,
    BIT,
    STY,
    ASL,
    ROL,
    LSR,
    ROR,
    DEC,
    INC,
    PHP,
    PLP,
    PHA,
    PLA,
    DEY,
    TAY,
    INY,
    INX,
    ANC,
    ALR,
    ARR,
    XAA,
    AXS,
    JMP,
    STX,
    BPL,
    BMI,
    BVC,
    BVS,
    BCC,
    BCS,
    BNE,
    BEQ,
    AHX,
    CLC,
    SEC,
    CLI,
    SEI,
    TYA,
    CLV,
    CLD,
    SED,
    TXS,
    TSX,
    TAS,
    LAS,
    SHY,
    SHX,
};

enum AddressingMode {
    NONE,
    IMMEDIATE,
    ZEROPAGE,
    ZEROPAGEREGX,
    ZEROPAGEREGY,
    ABSOLUTE,
    ABSOLUTEREGX,
    ABSOLUTEREGY,
    INDIRECT,
    INDIRECTREGX,
    INDIRECTREGY,
    RELATIVE,
    ACCUMULATOR,
};

struct CPU {
    short programCounter;
    char stackPointer;
    unsigned char regA;
    char regX;
    char regY;
    bool statusCarry;
    bool statusZero;
    bool statusInterruptDisable;
    bool statusDecimal;
    bool statusB;
    bool statusONE;
    bool statusOverflow;
    bool statusNegative;
    enum Instruction currentInstruction;
    char instructionCycle;
};

struct CPU newCPU(short startingPoint);

void cpuTick(struct CPU *cpu, unsigned char *ram, const char *rom);
