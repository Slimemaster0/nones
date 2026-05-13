#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include "cpu.h"
#include "files.h"
#include "cartridge.h"

int main(int argc, char** argv) {

    if (argc < 2) {
	perror("Please provide a ROM file!");
	exit(1);
    }

    struct binArray input = readFileBinary(argv[1]);

    if (input.contents == NULL) {
	perror("could not open file!");
	exit(2);
    }

    struct cartridge cartridge = readCartridge(input);

    // Deallocate input buffer
    free(input.contents);
    input.size = -1;
    input.contents = NULL;

    char lowerByte = cartridge.characterROM[0xfffc];
    char upperByte = cartridge.characterROM[0xfffd];
    short startingPoint = ((short)upperByte << 8) | (short)lowerByte;

    struct CPU cpu = newCPU(startingPoint);

    size_t cycle = 0;
    
    for (;; cycle++) {
	if (cycle % 4 == 0) {
	    
	}
	
    }
}
