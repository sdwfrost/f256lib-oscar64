OSCAR64 ?= ../../../oscar64/build/oscar64

FLAGS = -tm=f256k -n -i=../../f256lib -i=src

# Library source files (for dependency tracking)
LIB_HEADERS = $(wildcard ../../f256lib/*.h)
LIB_SOURCES = $(wildcard ../../f256lib/*.c)
LIB_FILES   = $(LIB_HEADERS) $(LIB_SOURCES)

# App source files
APP_SOURCES = $(wildcard src/*.c)
APP_HEADERS = $(wildcard src/*.h)

TARGET = filemanager.pgz

.PHONY: all clean

all: $(TARGET)

$(TARGET): src/filemanager.c $(APP_SOURCES) $(APP_HEADERS) $(LIB_FILES)
	$(OSCAR64) $(FLAGS) -o=$@ $<

clean:
	rm -f $(TARGET)
	rm -f *.asm *.int *.lbl *.map *.bcs
