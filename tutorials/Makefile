OSCAR64 ?= ../../oscar64/build/oscar64
F256LIB = ../f256lib
FLAGS = -tm=f256k -n -i=$(F256LIB) -i=.

all:
	@for src in */*.c; do \
		case "$$src" in */adventure.c) continue;; esac; \
		pgz="$${src%.c}.pgz"; \
		dir="$$(dirname $$src)"; \
		if [ "$$src" -nt "$$pgz" ] || [ ! -f "$$pgz" ] || \
		   (ls "$$dir"/*.h >/dev/null 2>&1 && [ "$$(find "$$dir" -name '*.h' -newer "$$pgz" 2>/dev/null)" ]); then \
			echo "$(OSCAR64) $(FLAGS) -o=$$pgz $$src"; \
			$(OSCAR64) $(FLAGS) -o="$$pgz" "$$src" || true; \
		fi; \
	done

clean:
	rm -f */*.pgz */*.asm */*.int */*.lbl */*.map

.PHONY: all clean
