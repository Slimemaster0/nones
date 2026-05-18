// vim:fileencoding=utf-8:foldmethod=marker

#include <stdbool.h>
#include <stdio.h>
#include "files.h"
#include "cpu.h"
#include "instructions.h"

struct CPU newCPU(short startingPoint) {
    struct CPU cpu = {
	.programCounter = startingPoint,
	.stackPointer = 0xff,
	.regA = 0,
	.regX = 0,
	.regY = 0,
	.statusCarry = false,
	.statusZero = false,
	.statusInterruptDisable = false,
	.statusDecimal = false,
	.statusB = false,
	.statusONE = false,
	.statusOverflow = false,
	.statusNegative = false,
	.currentInstruction = NOP,
	.instructionCycle = 0,
    };

    return cpu;
}


char cpuRead(short address, const unsigned char *ram, const char *rom) {
    if (address < 0x2000) {
	return ram[address % 0x800];
    } else if (address < 0x4000) {
	return 0; // TO DO!
    } else if (address < 0x4018) {
	return 0; // To DO!
    } else if (address < 0x401f) {
	return 0; // TO DO!
    } else {
	return rom[address - 0x4020];
    }
}

void cpuWrite(const short address, unsigned char *ram, const unsigned char value) {
    if (address < 0x2000) {
	ram[address % 0x800] = value;
    } else if (address < 0x4000) {
	return; // TO DO!
    } else if (address < 0x4018) {
	return; // To DO!
    } else if (address < 0x401f) {
	return; // TO DO!
    } else {
	// TO DO!
	// rom[address - 0x4020];
    }
}

char parseArgs(enum AddressingMode addressingMode, struct CPU *cpu, const unsigned char *ram, const char *rom) { // {{{
    short endAddr;
    if (addressingMode == NONE) {
	endAddr = 0;
    } else if (addressingMode == IMMEDIATE) {
	endAddr = cpu->programCounter +1;


	// Zero page
    } else if (addressingMode == ZEROPAGE) {
	cpu->instructionCycle += 2;
	endAddr = cpuRead(cpu->programCounter +1, ram, rom);

    } else if (addressingMode == ZEROPAGEREGX) {
	cpu->instructionCycle += 3;
	endAddr = cpuRead((char)(cpu->programCounter +1) +cpu->regX, ram, rom);
    
    } else if (addressingMode == ZEROPAGEREGY) {
	cpu->instructionCycle += 3;
	endAddr = cpuRead((char)(cpu->programCounter +1) +cpu->regY, ram, rom);
    
    } else if (addressingMode == ABSOLUTE) {
	cpu->instructionCycle += 3;
	short addr = ((short)cpuRead(cpu->programCounter +1, ram, rom)) << 8;
	addr = addr | ((short)cpuRead(cpu->programCounter +2, ram, rom));
	endAddr = cpuRead(addr, ram, rom);


	// Absolute addressing modes
    } else if (addressingMode == ABSOLUTEREGX) {
	cpu->instructionCycle += 3;
	short addr = ((short)cpuRead(cpu->programCounter +1, ram, rom)) << 8;
	addr = addr | ((short)cpuRead(cpu->programCounter +2, ram, rom));
	
	if (((addr + cpu->regX) & 0xff00) != (addr & 0xff00)) {
	    cpu->instructionCycle++;
	}

	endAddr = cpuRead(addr+cpu->regX, ram, rom);
    
    } else if (addressingMode == ABSOLUTEREGY) {
	cpu->instructionCycle += 3;
	short addr = ((short)cpuRead(cpu->programCounter +1, ram, rom)) << 8;
	addr = addr | ((short)cpuRead(cpu->programCounter +2, ram, rom));

	if (((addr + cpu->regY) & 0xff00) != (addr & 0xff00)) {
	    cpu->instructionCycle++;
	}

	endAddr = cpuRead(addr+cpu->regY, ram, rom);


	// Indirect addressing modes
    } else if (addressingMode == INDIRECT) {
	cpu->instructionCycle += 5;
	short addr = ((short)cpuRead(cpu->programCounter +1, ram, rom)) << 8;
	addr = addr | ((short)cpuRead(cpu->programCounter +2, ram, rom));
	
	short finalAddr = ((short)cpuRead(addr, ram, rom)) << 8;
	finalAddr = finalAddr | ((short)cpuRead(addr +1, ram, rom));

	endAddr = cpuRead(finalAddr, ram, rom);
    } else if (addressingMode == INDIRECTREGX) {
	cpu->instructionCycle += 5;
	char addr = cpuRead(cpu->programCounter +1, ram, rom);
	
	short finalAddr = cpuRead((addr + cpu->regX), ram, rom) << 8;
	finalAddr = finalAddr | cpuRead((addr + cpu->regX +1), ram, rom);

	endAddr = cpuRead(finalAddr, ram, rom);
	
    } else if (addressingMode == INDIRECTREGY) {
	cpu->instructionCycle += 4;

	char addr = cpuRead(cpu->programCounter +1, ram, rom);
	
	short finalAddr = cpuRead((addr), ram, rom) << 8;
	finalAddr = finalAddr | cpuRead((addr), ram, rom);
	
	if (((addr + cpu->regY) & 0xff00) != (addr & 0xff00)) {
	    cpu->instructionCycle++;
	}

	endAddr = cpuRead(finalAddr, ram, rom) + cpu->regY;
	
    } else if (addressingMode == ACCUMULATOR) {
	return cpu->regA;
	// Relative addressing
    } else {
	cpu->instructionCycle += 2;
	short addr = cpuRead(cpu->programCounter +1, ram, rom);

	if (((addr + cpu->programCounter) & 0xff00) != (cpu->programCounter & 0xff00)) {
	    cpu->instructionCycle++;
	}

	endAddr = addr + cpu->programCounter;
    }

    return cpuRead(endAddr, ram, rom);
} // }}}

short getAddr(enum AddressingMode addressingMode, struct CPU *cpu, const unsigned char *ram, const char *rom) { // {{{
    short endAddr;
    if (addressingMode == NONE) {
	endAddr = 0;
    } else if (addressingMode == IMMEDIATE) {
	endAddr = cpu->programCounter +1;


	// Zero page
    } else if (addressingMode == ZEROPAGE) {
	cpu->instructionCycle += 2;
	endAddr = cpuRead(cpu->programCounter +1, ram, rom);

    } else if (addressingMode == ZEROPAGEREGX) {
	cpu->instructionCycle += 3;
	endAddr = cpuRead((char)(cpu->programCounter +1) +cpu->regX, ram, rom);
    
    } else if (addressingMode == ZEROPAGEREGY) {
	cpu->instructionCycle += 3;
	endAddr = cpuRead((char)(cpu->programCounter +1) +cpu->regY, ram, rom);
    
    } else if (addressingMode == ABSOLUTE) {
	cpu->instructionCycle += 3;
	short addr = ((short)cpuRead(cpu->programCounter +1, ram, rom)) << 8;
	addr = addr | ((short)cpuRead(cpu->programCounter +2, ram, rom));
	endAddr = cpuRead(addr, ram, rom);


	// Absolute addressing modes
    } else if (addressingMode == ABSOLUTEREGX) {
	cpu->instructionCycle += 3;
	short addr = ((short)cpuRead(cpu->programCounter +1, ram, rom)) << 8;
	addr = addr | ((short)cpuRead(cpu->programCounter +2, ram, rom));
	
	if (((addr + cpu->regX) & 0xff00) != (addr & 0xff00)) {
	    cpu->instructionCycle++;
	}

	endAddr = cpuRead(addr+cpu->regX, ram, rom);
    
    } else if (addressingMode == ABSOLUTEREGY) {
	cpu->instructionCycle += 3;
	short addr = ((short)cpuRead(cpu->programCounter +1, ram, rom)) << 8;
	addr = addr | ((short)cpuRead(cpu->programCounter +2, ram, rom));

	if (((addr + cpu->regY) & 0xff00) != (addr & 0xff00)) {
	    cpu->instructionCycle++;
	}

	endAddr = cpuRead(addr+cpu->regY, ram, rom);


	// Indirect addressing modes
    } else if (addressingMode == INDIRECT) {
	cpu->instructionCycle += 5;
	short addr = ((short)cpuRead(cpu->programCounter +1, ram, rom)) << 8;
	addr = addr | ((short)cpuRead(cpu->programCounter +2, ram, rom));
	
	short finalAddr = ((short)cpuRead(addr, ram, rom)) << 8;
	finalAddr = finalAddr | ((short)cpuRead(addr +1, ram, rom));

	endAddr = cpuRead(finalAddr, ram, rom);
    } else if (addressingMode == INDIRECTREGX) {
	cpu->instructionCycle += 5;
	char addr = cpuRead(cpu->programCounter +1, ram, rom);
	
	short finalAddr = cpuRead((addr + cpu->regX), ram, rom) << 8;
	finalAddr = finalAddr | cpuRead((addr + cpu->regX +1), ram, rom);

	endAddr = cpuRead(finalAddr, ram, rom);
	
    } else if (addressingMode == INDIRECTREGY) {
	cpu->instructionCycle += 4;

	char addr = cpuRead(cpu->programCounter +1, ram, rom);
	
	short finalAddr = cpuRead((addr), ram, rom) << 8;
	finalAddr = finalAddr | cpuRead((addr), ram, rom);
	
	if (((addr + cpu->regY) & 0xff00) != (addr & 0xff00)) {
	    cpu->instructionCycle++;
	}

	endAddr = cpuRead(finalAddr, ram, rom) + cpu->regY;
	
    } else if (addressingMode == ACCUMULATOR) {
	return cpu->regA;
	// Relative addressing
    } else {
	cpu->instructionCycle += 2;
	short addr = cpuRead(cpu->programCounter +1, ram, rom);

	if (((addr + cpu->programCounter) & 0xff00) != (cpu->programCounter & 0xff00)) {
	    cpu->instructionCycle++;
	}

	endAddr = addr + cpu->programCounter;
    }

    return endAddr;
    
} //}}}

void prgCountInc(struct CPU *cpu, enum AddressingMode addressingMode) {
    if (addressingMode == NONE || addressingMode == ACCUMULATOR) {
	cpu->programCounter+=1;
    } else if (
	    addressingMode == INDIRECT ||
	    addressingMode == ZEROPAGE ||
	    addressingMode == ZEROPAGEREGX ||
	    addressingMode == ZEROPAGEREGY ||
	    addressingMode == RELATIVE ||
	    addressingMode == INDIRECTREGX ||
	    addressingMode == INDIRECTREGY
	    ) {
	cpu->programCounter+=2;
    } else {
	cpu->programCounter+=3;
    }
}

void updateZN(struct CPU *cpu, const unsigned char value) {
    if (value == 0) {
	cpu->statusZero = true;
    } else {
	cpu->statusZero = false;
    }
    if (value >= 0x80) {
	cpu->statusNegative = true;
    } else {
	cpu->statusNegative = false;
    }
}

void cpuTick(struct CPU *cpu, unsigned char *ram, const char *rom) {
    enum AddressingMode addressingMode = 0;
    if (cpu->instructionCycle == 0) {
	unsigned char opCode = cpuRead(cpu->programCounter, ram, rom);
	cpu->programCounter++;


	unsigned char lowerByte = opCode & 0x1f;
	if (lowerByte == 0x00) {
	    if (opCode == 0x20) {
		addressingMode = ABSOLUTE;
	    } else if (opCode < 0x80) {
		addressingMode = IMMEDIATE;
	    } else {
		addressingMode = NONE;
	    }
	} else if (lowerByte == 0x04) {
	    addressingMode = ZEROPAGE;
	} else if (lowerByte == 0x08) {
	    addressingMode = NONE;
	} else if (lowerByte == 0x0c) {
	
	}
    }

    if (cpu->currentInstruction != STP) {
	cpu->instructionCycle--;
    }
    return;

    // {{{ Instructions

brk:
    cpu->currentInstruction = BRK;
    // TO DO!
    perror("brk not implemented!");
    cpu->instructionCycle--;
    prgCountInc(cpu, addressingMode);
    return;

jsr:
    cpu->currentInstruction = JSR;
    cpu->instructionCycle = 3;

    unsigned short retAddr = cpu->programCounter+2;
    stackPush((const unsigned char)(retAddr >> 8), cpu, ram);
    stackPush((const unsigned char)retAddr, cpu, ram);
    
    cpu->programCounter = getAddr(ABSOLUTE, cpu, ram, rom);

    cpu->instructionCycle--;
    return;

rti:
    cpu->currentInstruction = RTI;
    cpu->instructionCycle = 6;

    unsigned char pws = stackPop(cpu, ram);

    cpu->statusCarry = pws & 1;
    cpu->statusZero = (pws & 2) >> 1;
    cpu->statusInterruptDisable = (pws & 4) >> 2;
    cpu->statusDecimal = (pws & 8) >> 3;
    cpu->statusB = (pws & 16) >> 4;
    cpu->statusONE = (pws & 32) >> 5;
    cpu->statusOverflow = (pws & 64) >> 6;
    cpu->statusNegative = (pws & 128) >> 7;
    
    cpu->programCounter = (short)stackPop(cpu, ram) | ((short)stackPop(cpu, ram) << 8);

    cpu->instructionCycle--;
    return;
    
rts:
    cpu->currentInstruction = RTS;
    cpu->instructionCycle = 6;
    cpu->programCounter = (short)stackPop(cpu, ram) | ((short)stackPop(cpu, ram) << 8);
    cpu->instructionCycle--;
    prgCountInc(cpu, addressingMode);
    return;

nop:
    cpu->currentInstruction = NOP;
    cpu->instructionCycle = 2;
    parseArgs(addressingMode, cpu, ram, rom);
    
    cpu->instructionCycle--;
    prgCountInc(cpu, addressingMode);
    return;

ldy:
    cpu->currentInstruction = LDY;
    cpu->instructionCycle = 1;
    cpu->regY = parseArgs(addressingMode, cpu, ram, rom);

    cpu->instructionCycle--;
    prgCountInc(cpu, addressingMode);
    return;

cpy:
    cpu->currentInstruction = CPY;
    cpu->instructionCycle = 1;
    compare(cpu->regY, parseArgs(addressingMode, cpu, ram, rom), cpu);

    cpu->instructionCycle--;
    prgCountInc(cpu, addressingMode);
    return;

cpx:
    cpu->currentInstruction = CPX;
    cpu->instructionCycle = 1;
    compare(cpu->regX, parseArgs(addressingMode, cpu, ram, rom), cpu);

    cpu->instructionCycle--;
    prgCountInc(cpu, addressingMode);
    return;

ora:
    cpu->currentInstruction = ORA;
    cpu->instructionCycle = 1;
    cpu->regA = cpu->regA | parseArgs(addressingMode, cpu, ram, rom);

    updateZN(cpu, cpu->regA);

    cpu->instructionCycle--;

eor:
    cpu->currentInstruction = EOR;
    cpu->instructionCycle = 1;
    cpu->regA = cpu->regA ^ parseArgs(addressingMode, cpu, ram, rom);

    updateZN(cpu, cpu->regA);

    cpu->instructionCycle--;
    prgCountInc(cpu, addressingMode);
    return;

adc:
    cpu->currentInstruction = ADC;
    cpu->instructionCycle = 1;
    unsigned char arg = parseArgs(addressingMode, cpu, ram, rom);
    unsigned char result = cpu->regA + arg + cpu->statusCarry;

    if ((unsigned short)cpu->regA + (unsigned short)arg + (unsigned short)cpu->statusCarry > 0x00ff) {
	cpu->statusCarry = true;
    } else {
	cpu->statusCarry = false;
    }

    if ((result ^ cpu->regA) & (result ^ arg) & 0x80 == 1) {
	cpu->statusOverflow = true;
    } else {
	cpu->statusOverflow = false;
    }


    updateZN(cpu, result);

    cpu->regA = result;
    cpu->instructionCycle--;
    prgCountInc(cpu, addressingMode);
    return;

sta:
    cpu->currentInstruction = STA;
    cpu->instructionCycle = 1;
    cpuWrite(getAddr(addressingMode, cpu, ram, rom), ram, cpu->regA);

    cpu->instructionCycle--;
    prgCountInc(cpu, addressingMode);
    return;


lda:
    cpu->currentInstruction = LDA;
    cpu->instructionCycle = 1;
    cpu->regA = parseArgs(addressingMode, cpu, ram, rom);

    cpu->instructionCycle--;
    prgCountInc(cpu, addressingMode);
    return;

cmp:
    cpu->currentInstruction = CMP;
    cpu->instructionCycle = 1;
    compare(cpu->regA, parseArgs(addressingMode, cpu, ram, rom), cpu);

    cpu->instructionCycle--;
    prgCountInc(cpu, addressingMode);
    return;

sbc:
    cpu->currentInstruction = SBC;
    cpu->instructionCycle = 1;
    arg = parseArgs(addressingMode, cpu, ram, rom);
    result = (cpu->regA - arg + cpu->statusCarry) -1;

    if ((unsigned short)cpu->regA - (unsigned short)arg + (unsigned short)cpu->statusCarry -1 > 0x00ff) {
	cpu->statusCarry = false;
    } else {
	cpu->statusCarry = true;
    }

    if ((result ^ cpu->regA) & (result ^ ~arg) & 0x80 == 1) {
	cpu->statusOverflow = true;
    } else {
	cpu->statusOverflow = false;
    }

    updateZN(cpu, result);

    cpu->regA = result;
    cpu->instructionCycle--;
    prgCountInc(cpu, addressingMode);
    return;

stp:
    cpu->currentInstruction = STP;
    cpu->instructionCycle = 0xff;

    prgCountInc(cpu, addressingMode);
    return;

ldx:
    cpu->currentInstruction = LDX;
    cpu->instructionCycle = 1;
    cpu->regX = parseArgs(addressingMode, cpu, ram, rom);

    cpu->instructionCycle--;
    prgCountInc(cpu, addressingMode);
    return;

asl:
    cpu->currentInstruction = ASL;
    cpu->instructionCycle = 1;
    unsigned char value;

    if (addressingMode == ACCUMULATOR) {
	value = cpu->regA;
    } else {
	value = parseArgs(addressingMode, cpu, ram, rom);
    }
    
    cpu->statusCarry = value >> 7;
    result = value << 1;
    
    updateZN(cpu, result);

    if (addressingMode == ACCUMULATOR) {
	cpu->regA = result;
	cpu->instructionCycle++;
    } else {
	cpuWrite(getAddr(addressingMode, cpu, ram, rom), ram, result);
    }
    
    cpu->instructionCycle--;
    prgCountInc(cpu, addressingMode);
    return;
    

slo: 
    cpu->currentInstruction = SLO;
    cpu->instructionCycle = 1;

    value = parseArgs(addressingMode, cpu, ram, rom);
    
    cpu->statusCarry = value >> 7;
    result = value << 1;
    
    updateZN(cpu, result);

    cpuWrite(getAddr(addressingMode, cpu, ram, rom), ram, result);

    cpu->regA = value;

    updateZN(cpu, cpu->regA);

    cpu->instructionCycle--;
    prgCountInc(cpu, addressingMode);
    return;

rol:
    cpu->currentInstruction = ROL;
    cpu->instructionCycle = 1;

    if (addressingMode == ACCUMULATOR) {
	value = cpu->regA;
    } else {
	value = parseArgs(addressingMode, cpu, ram, rom);
    }

    bool shouldCarry;
    if (value >= 0x80) {
	shouldCarry = true;
    } else {
	shouldCarry = false;
    }
    result = (value << 1) | cpu->statusCarry;

    cpu->statusCarry = shouldCarry;
    
    updateZN(cpu, result);

    if (addressingMode == ACCUMULATOR) {
	cpu->regA = result;
	cpu->instructionCycle++;
    } else {
	cpuWrite(getAddr(addressingMode, cpu, ram, rom), ram, result);
    }
    
    cpu->instructionCycle--;
    prgCountInc(cpu, addressingMode);
    return;

and:
    cpu->currentInstruction = AND;
    cpu->instructionCycle = 1;
    cpu->regA = cpu->regA & parseArgs(addressingMode, cpu, ram, rom);

    updateZN(cpu, cpu->regA);

    cpu->instructionCycle--;
    prgCountInc(cpu, addressingMode);
    return;

rla:
    cpu->currentInstruction = RLA;
    cpu->instructionCycle = 1;

    if (addressingMode == ACCUMULATOR) {
	value = cpu->regA;
    } else {
	value = parseArgs(addressingMode, cpu, ram, rom);
    }

    if (value >= 0x80) {
	shouldCarry = true;
    } else {
	shouldCarry = false;
    }
    result = (value << 1) | cpu->statusCarry;
    cpuWrite(getAddr(addressingMode, cpu, ram, rom), ram, result);

    cpu->statusCarry = shouldCarry;
    
    updateZN(cpu, result);
    cpuWrite(getAddr(addressingMode, cpu, ram, rom), ram, result);

    cpu->regA = cpu->regA & result;

    updateZN(cpu, cpu->regA);

    
    cpu->instructionCycle--;
    prgCountInc(cpu, addressingMode);
    return;

lsr:
    cpu->instructionCycle = 1;
    cpu->currentInstruction = LSR;

    if (addressingMode == ACCUMULATOR) {
	value = cpu->regA;
    } else {
	value = parseArgs(addressingMode, cpu, ram, rom);
    }
    
    cpu->statusCarry = value & 1;
    result = value >> 1;
    
    updateZN(cpu, result);

    if (addressingMode == ACCUMULATOR) {
	cpu->regA = result;
	cpu->instructionCycle++;
    } else {
	cpuWrite(getAddr(addressingMode, cpu, ram, rom), ram, result);
    }
    
    cpu->instructionCycle--;
    prgCountInc(cpu, addressingMode);
    return;

sre:
    cpu->currentInstruction = SRE;
    cpu->instructionCycle = 1;

    value = parseArgs(addressingMode, cpu, ram, rom);
    
    cpu->statusCarry = value & 1;
    result = value >> 1;
    
    updateZN(cpu, result);

    cpuWrite(getAddr(addressingMode, cpu, ram, rom), ram, result);

    cpu->regA = cpu->regA ^ result;

    updateZN(cpu, cpu->regA);
    
    cpu->instructionCycle--;
    prgCountInc(cpu, addressingMode);
    return;
    
ror:
    cpu->currentInstruction = ROR;
    cpu->instructionCycle = 1;

    if (addressingMode == ACCUMULATOR) {
	value = cpu->regA;
    } else {
	value = parseArgs(addressingMode, cpu, ram, rom);
    }

    
    
    shouldCarry = value & 1;
    result = (value << 1) | cpu->statusCarry;

    cpu->statusCarry = shouldCarry;
    
    updateZN(cpu, result);

    if (addressingMode == ACCUMULATOR) {
	cpu->regA = result;
	cpu->instructionCycle++;
    } else {
	cpuWrite(getAddr(addressingMode, cpu, ram, rom), ram, result);
    }
    
    cpu->instructionCycle--;
    prgCountInc(cpu, addressingMode);
    return;

rra:
    cpu->currentInstruction = RRA;
    cpu->instructionCycle = 1;

    if (addressingMode == ACCUMULATOR) {
	value = cpu->regA;
    } else {
	value = parseArgs(addressingMode, cpu, ram, rom);
    }

    
    
    shouldCarry = value & 1;
    result = (value << 1) | cpu->statusCarry;

    cpu->statusCarry = shouldCarry;
    
    updateZN(cpu, result);

    if (addressingMode == ACCUMULATOR) {
	cpu->regA = result;
	cpu->instructionCycle++;
    } else {
	cpuWrite(getAddr(addressingMode, cpu, ram, rom), ram, result);
    }

    unsigned char out = cpu->regA + result + cpu->statusCarry;;
    
    if ((unsigned short)cpu->regA + (unsigned short)result + (unsigned short)cpu->statusCarry > 0x00ff) {
	cpu->statusCarry = true;
    } else {
	cpu->statusCarry = false;
    }

    if ((out ^ cpu->regA) & (out ^ result) & 0x80 == 1) {
	cpu->statusOverflow = true;
    } else {
	cpu->statusOverflow = false;
    }


    if (out == 0) {
	cpu->statusZero = true;
    } else {
	cpu->statusZero = false;
    }
    if (out >= 0x80) {
	cpu->statusNegative = true;
    } else {
	cpu->statusNegative = false;
    }

    cpu->regA = out;
    cpu->instructionCycle--;
    prgCountInc(cpu, addressingMode);
    return;

sax:
    cpu->currentInstruction = SAX;
    cpu->instructionCycle = 1;
    cpuWrite(getAddr(addressingMode, cpu, ram, rom), ram, cpu->regA & cpu->regX);

    cpu->instructionCycle--;
    prgCountInc(cpu, addressingMode);
    return;

lax:
    cpu->currentInstruction = LAX;
    cpu->instructionCycle = 1;
    cpu->regA = parseArgs(addressingMode, cpu, ram, rom);
    cpu->regX = cpu->regA;

    cpu->instructionCycle--;
    prgCountInc(cpu, addressingMode);
    return;

dec:
    cpu->currentInstruction = DEC;
    cpu->instructionCycle = 1;
    value = parseArgs(addressingMode, cpu, ram, rom);
    
    result = value - 1;
    
    updateZN(cpu, result);

    cpuWrite(getAddr(addressingMode, cpu, ram, rom), ram, result);
    
    cpu->instructionCycle--;
    prgCountInc(cpu, addressingMode);
    return;
    

dcp:
    cpu->currentInstruction = DCP;
    cpu->instructionCycle = 1;

    result = value - 1;
    
    updateZN(cpu, result);

    cpuWrite(getAddr(addressingMode, cpu, ram, rom), ram, result);

    compare(cpu->regA, result, cpu);
    
    cpu->instructionCycle--;
    prgCountInc(cpu, addressingMode);
    return;

inc:
    value = parseArgs(addressingMode, cpu, ram, rom);
    
    result = value - 1;
    
    updateZN(cpu, result);

    if (addressingMode == ACCUMULATOR) {
	cpu->regA = result;
	cpu->instructionCycle++;
    } else {
	cpuWrite(getAddr(addressingMode, cpu, ram, rom), ram, result);
    }
    
    cpu->instructionCycle--;
    prgCountInc(cpu, addressingMode);
    return;
    

isc:
    cpu->currentInstruction = ISC;
    cpu->instructionCycle = 1;

    value = parseArgs(addressingMode, cpu, ram, rom);
    
    result = value - 1;
    
    updateZN(cpu, result);

    if (addressingMode == ACCUMULATOR) {
	cpu->regA = result;
	cpu->instructionCycle++;
    } else {
	cpuWrite(getAddr(addressingMode, cpu, ram, rom), ram, result);
    }

    if ((unsigned short)cpu->regA - (unsigned short)result + (unsigned short)cpu->statusCarry -1 > 0x00ff) {
	cpu->statusCarry = false;
    } else {
	cpu->statusCarry = true;
    }

    if ((out ^ cpu->regA) & (out ^ ~result) & 0x80 == 1) {
	cpu->statusOverflow = true;
    } else {
	cpu->statusOverflow = false;
    }

    if (out == 0) {
	cpu->statusZero = true;
    } else {
	cpu->statusZero = false;
    }
    if (out >= 0x80) {
	cpu->statusNegative = true;
    } else {
	cpu->statusNegative = false;
    }

    cpu->regA = out;
    cpu->instructionCycle--;
    prgCountInc(cpu, addressingMode);
    return;

bit:
    cpu->currentInstruction = BIT;
    cpu->instructionCycle = 1;
    arg = parseArgs(addressingMode, cpu, ram, rom);
    
    if ((arg & cpu->regA) == 0) {
	cpu->statusZero = true;
    } else {
	cpu->statusZero = false;
    }

    cpu->statusNegative = result >> 7;
    cpu->statusOverflow = (arg >> 6) & 1;

    cpu->instructionCycle--;
    prgCountInc(cpu, addressingMode);
    return;


sty:
    cpu->currentInstruction = STY;
    cpu->instructionCycle = 1;
    cpuWrite(getAddr(addressingMode, cpu, ram, rom), ram, cpu->regY);

    cpu->instructionCycle--;
    prgCountInc(cpu, addressingMode);
    return;

php:
    cpu->currentInstruction = PHP;
    cpu->instructionCycle = 3;
    
    pws = cpu->statusCarry |
	(cpu->statusZero << 1) |
	(cpu->statusInterruptDisable << 2) |
	(cpu->statusDecimal << 3) |
	(cpu->statusB << 4) |
	(cpu->statusONE << 5) |
	(cpu->statusOverflow << 6) |
	(cpu->statusNegative << 7);

    stackPush(pws, cpu, ram);

    cpu->instructionCycle--;
    prgCountInc(cpu, addressingMode);
    return;

plp:
    cpu->currentInstruction = PLP;
    cpu->instructionCycle = 4;
    
    pws = stackPop(cpu, ram);

    cpu->statusCarry = pws & 1;
    cpu->statusZero = (pws & 2) >> 1;
    cpu->statusInterruptDisable = (pws & 4) >> 2;
    cpu->statusDecimal = (pws & 8) >> 3;
    cpu->statusB = (pws & 16) >> 4;
    cpu->statusONE = (pws & 32) >> 5;
    cpu->statusOverflow = (pws & 64) >> 6;
    cpu->statusNegative = (pws & 128) >> 7;
    
    cpu->instructionCycle--;
    prgCountInc(cpu, addressingMode);
    return;

pla:
    cpu->currentInstruction = PLA;
    cpu->instructionCycle = 4;

    cpu->regA = stackPop(cpu, ram);
    
    cpu->instructionCycle--;
    prgCountInc(cpu, addressingMode);
    return;

pha:
    cpu->currentInstruction = PHA;
    cpu->instructionCycle = 3;

    stackPush(cpu->regA, cpu, ram);
    
    cpu->instructionCycle--;
    prgCountInc(cpu, addressingMode);
    return;

dey:
    cpu->currentInstruction = DEY;
    cpu->instructionCycle = 2;
    value = cpu->regY;
    
    result = value - 1;
    
    updateZN(cpu, result);

    cpu->regY = result;
    
    cpu->instructionCycle--;
    prgCountInc(cpu, addressingMode);
    return;

tay:
    cpu->currentInstruction = TAY;
    cpu->instructionCycle = 2;
    cpu->regY = cpu->regA;

    cpu->instructionCycle--;
    prgCountInc(cpu, addressingMode);
    return;

iny:
    cpu->currentInstruction = INY;
    cpu->instructionCycle = 2;
    value = cpu->regY;
    
    result = value + 1;
    
    updateZN(cpu, result);

    cpu->regY = result;
    
    cpu->instructionCycle--;
    prgCountInc(cpu, addressingMode);
    return;

inx:
    cpu->currentInstruction = INX;
    cpu->instructionCycle = 2;
    value = cpu->regX;
    
    result = value + 1;
    
    updateZN(cpu, result);

    cpu->regX = result;
    
    cpu->instructionCycle--;
    prgCountInc(cpu, addressingMode);
    return;

anc:
    cpu->currentInstruction = ANC;
    cpu->instructionCycle = 1;
    cpu->regA = cpu->regA & parseArgs(addressingMode, cpu, ram, rom);

    cpu->statusCarry = cpu->regA >> 7;

    updateZN(cpu, cpu->regA);

    cpu->instructionCycle--;
    prgCountInc(cpu, addressingMode);
    return;

alr:
    cpu->instructionCycle = 1;
    cpu->currentInstruction = ALR;

    value = cpu->regA & parseArgs(addressingMode, cpu, ram, rom) ;
    
    cpu->statusCarry = value & 1;
    result = value >> 1;
    
    updateZN(cpu, result);

    cpu->regA = result;
    
    cpu->instructionCycle--;
    prgCountInc(cpu, addressingMode);
    return;

// A pirates favorite Instruction
arr:
    cpu->currentInstruction = ARR;
    cpu->instructionCycle = 1;

    value = cpu->regA & parseArgs(addressingMode, cpu, ram, rom);
    
    shouldCarry = value & 1;
    result = (value << 1) | cpu->statusCarry;

    cpu->statusCarry = shouldCarry;
    
    updateZN(cpu, result);

    cpu->regA = result;
    
    cpu->instructionCycle--;
    prgCountInc(cpu, addressingMode);
    return;

xaa:
    cpu->currentInstruction = XAA;
    cpu->instructionCycle = 1;
    
    cpu->regA = cpu->regA & cpu->regX & parseArgs(IMMEDIATE, cpu, ram, rom);

    updateZN(cpu, result);

    cpu->instructionCycle--;
    prgCountInc(cpu, addressingMode);
    return;

axs:
    cpu->currentInstruction = AXS;
    cpu->instructionCycle = 2;

    result = cpu->regA & cpu->regX;

    cpu->statusCarry = cpu->regA >> 7;

    updateZN(cpu, cpu->regA);
    
    cpu->instructionCycle--;
    prgCountInc(cpu, addressingMode);
    return;

jmp:
    cpu->currentInstruction = JMP;

    if (addressingMode == INDIRECT) {
	cpu->programCounter = getAddr(ABSOLUTE, cpu, ram, rom) -1;
	cpu->programCounter = getAddr(ABSOLUTE, cpu, ram, rom);
	cpu->instructionCycle = 4;
    } else {
	cpu->programCounter = getAddr(ABSOLUTE, cpu, ram, rom);
	cpu->instructionCycle = 3;
    }

    cpu->instructionCycle--;
    return;




    // }}}
}
