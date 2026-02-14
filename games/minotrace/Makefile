OSCAR64 ?= ../../../oscar64/build/oscar64
FLAGS = -tm=f256k -n -i=../../f256lib -i=src -O3

all: minotrace.pgz

minotrace.pgz: src/minotrace.c $(wildcard src/*.h) $(wildcard src/*.c)
	$(OSCAR64) $(FLAGS) -o=$@ $<

clean:
	rm -f minotrace.pgz *.asm *.int *.lbl *.map *.bin
