enum NameTableLayout {
    VERTICAL,
    HERISONTAL,
    ALT1,
    ALT2
};

struct cartridge {
    char *programROM;
    char *characterROM;
    short mapper;
    bool hasTrainer;
    enum NameTableLayout nameTableMirroring;
    char programROMSize;
    char characterROMSize;
};

struct cartridge readCartridge(struct binArray input);
