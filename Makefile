Flags=-O2 -flto

srcDir=src/

VPATH = src/

output: main.o files.o cpu.o cartridge.o
	gcc $(Flags) $(srcDir)main.o $(srcDir)files.o $(srcDir)cpu.o $(srcDir)cartridge.o -o nones

main.o: main.c
	gcc $(Flags) -c $(srcDir)main.c -o $(srcDir)main.o

files.o: files.c
	gcc $(Flags) -c $(srcDir)files.c -o $(srcDir)files.o

cpu.o: cpu.c
	gcc $(Flags) -c $(srcDir)cpu.c -o $(srcDir)cpu.o

cartridge.o:
	gcc $(Flags) -c $(srcDir)cartridge.c -o $(srcDir)cartridge.o

run: output
	./nones
