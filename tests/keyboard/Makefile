OSCAR64 ?= ../../../oscar64/build/oscar64
FLAGS = -tm=f256k -n -i=../../f256lib -i=src

all: keyboard_test.pgz

keyboard_test.pgz: src/keyboard_test.c
	$(OSCAR64) $(FLAGS) -o=$@ $<

clean:
	rm -f keyboard_test.pgz *.asm *.int *.lbl *.map *.bin
