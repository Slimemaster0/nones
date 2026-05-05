#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include "files.h"
#include "cartridge.h"

const size_t PROGRAMROMCHUNK = 16384;
const size_t CHARACTERROMCHUNK = 8192;

struct cartridge readCartridge(struct binArray input) {
    if (
	    input.contents[0] != 0x4E &&
	    input.contents[1] != 0x45 &&
	    input.contents[2] != 0x53 &&
	    input.contents[3] != 0x1A
    ) {
	perror("Please provide an NES rom file!");
	exit(3);
    }

    bool hasTrainer = (input.contents[6] & 0b00000100) >> 2;
    
    // Check rom size
    if (input.size < ((16 + 512 * hasTrainer) + PROGRAMROMCHUNK * input.contents[4] + CHARACTERROMCHUNK * input.contents[5])) {
	perror("Malformed file header!");
	exit(4);
    }

    // Rom data
    char *programROM = malloc(PROGRAMROMCHUNK * input.contents[4]);
    memcpy(programROM, input.contents + 16 + 512 * hasTrainer, PROGRAMROMCHUNK * input.contents[4]);

    char *characterROM = malloc(PROGRAMROMCHUNK * input.contents[4]);
    memcpy(characterROM, (input.contents + 16 + 512 * hasTrainer) + PROGRAMROMCHUNK * input.contents[4], CHARACTERROMCHUNK * input.contents[5]);

    char lowerbit = 0;
    char upperbit = 0;

    // Name table layout
    lowerbit = input.contents[6] & 0b00000001;
    upperbit = input.contents[6] & 0b00001000 >> 2;
    enum NameTableLayout nameTableLayout = lowerbit | upperbit;

    // Mapper
    lowerbit = input.contents[6] & 0b11110000;
    upperbit = input.contents[7] & 0b00001111;
    short mapper = lowerbit | upperbit;
    
    struct cartridge ret = {
	.programROM = programROM,
	.programROMSize = input.contents[4],
	.characterROM = characterROM,
	.characterROMSize = input.contents[5],
	.hasTrainer = hasTrainer,
	.nameTableMirroring = nameTableLayout,
	.mapper = mapper
    };

    return ret;
}
