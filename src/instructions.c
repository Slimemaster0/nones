#include <stdbool.h>
#include "cpu.h"


char parseArgs(enum AddressingMode addressingMode, struct CPU *cpu, const unsigned char *ram, const char *rom);

void compare(const unsigned char input1, const unsigned char input2, struct CPU *cpu) {
    if (input1 >= input2) {
	cpu->statusCarry = true;
    } else {
	cpu->statusCarry = false;
    }

    if (input1 == input2) {
	cpu->statusZero = true;
    } else {
	cpu->statusZero = false;
    }

    if (input1 >= 0x80) {
	cpu->statusNegative = true;
    } else {
	cpu->statusNegative = false;
    }
}

void stackPush(const unsigned char input, struct CPU *cpu, unsigned char *ram) {
    ram[0x100 + (short)cpu->stackPointer] = input;
    cpu->stackPointer--;
}

unsigned char stackPop(struct CPU *cpu, unsigned char *ram) {
    cpu->stackPointer++;
    return ram[0x100 + (short)cpu->stackPointer];
}

void branch(struct CPU *cpu, const unsigned char *ram, const char *rom) {
    cpu->programCounter++;

    short delta = parseArgs(IMMEDIATE, cpu, ram, rom);
    delta = (delta & 0x7f) | ((delta & 0x80) << 8);

    short pcOld = cpu->programCounter;

    cpu->programCounter = cpu->programCounter + delta;

    if ((pcOld & 0xff00) != (cpu->programCounter & 0xff00)) {
	cpu->programCounter++;
    }
    
}
