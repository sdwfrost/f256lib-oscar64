OSCAR64 ?= ../../../oscar64/build/oscar64
FLAGS = -tm=f256k -n -i=../../f256lib -i=src -dWITHOUT_KEYBOARD

all: switcheroo.pgz

switcheroo.pgz: src/switcheroo.c $(wildcard src/*.h) $(wildcard src/*.c)
	$(OSCAR64) $(FLAGS) -o=$@ $<

clean:
	rm -f switcheroo.pgz *.asm *.int *.lbl *.map *.bin
