OSCAR64 ?= ../../oscar64/build/oscar64

FLAGS = -tm=f256k -n -i=../f256lib

# Library source files (for dependency tracking)
LIB_HEADERS = $(wildcard ../f256lib/*.h)
LIB_SOURCES = $(wildcard ../f256lib/*.c)
LIB_FILES   = $(LIB_HEADERS) $(LIB_SOURCES)

# Targets
ALL_TARGETS = cube/cube.pgz \
              lines/lines.pgz \
              overlay/overlay.pgz \
              sprites/sprites.pgz \
              tilemap/tilemap.pgz

.PHONY: all clean

all: $(ALL_TARGETS)

cube/cube.pgz: cube/src/cube.c $(LIB_FILES)
	$(OSCAR64) $(FLAGS) -o=$@ $<

lines/lines.pgz: lines/src/lines.c $(LIB_FILES)
	$(OSCAR64) $(FLAGS) -o=$@ $<

overlay/overlay.pgz: overlay/src/overlay.c $(LIB_FILES)
	$(OSCAR64) $(FLAGS) -o=$@ $<

sprites/sprites.pgz: sprites/src/sprites.c sprites/generated/sprites.bin $(LIB_FILES)
	$(OSCAR64) $(FLAGS) -o=$@ $<

tilemap/tilemap.pgz: tilemap/src/tilemap.c tilemap/tiles/tiles.bin $(LIB_FILES)
	$(OSCAR64) $(FLAGS) -o=$@ $<

clean:
	rm -f $(ALL_TARGETS)
	rm -f */*.asm */*.int */*.lbl */*.map
