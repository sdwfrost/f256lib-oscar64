OSCAR64 ?= ../../../oscar64/build/oscar64
FLAGS = -tm=f256k -n -O3 -i=../../f256lib -i=src

all: photoncrawler.pgz

photoncrawler.pgz: src/photoncrawler.c src/rgbimage.h src/rgbimage.c $(wildcard ../../f256lib/*.h) $(wildcard ../../f256lib/*.c)
	$(OSCAR64) $(FLAGS) -o=$@ $<

clean:
	rm -f photoncrawler.pgz *.asm *.int *.lbl *.map *.bin
