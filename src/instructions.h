void compare(const unsigned char input1, const unsigned char input2, struct CPU *cpu);
void stackPush(const unsigned char input, struct CPU *cpu, unsigned char *ram);
unsigned char stackPop(struct CPU *cpu, unsigned char *ram);
void branch(struct CPU *cpu, const unsigned char *ram, const char *rom);
