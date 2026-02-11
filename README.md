# f256lib-oscar64

A port of [f256lib](https://github.com/FoenixRetro/f256-dev) to the [oscar64](https://github.com/drmortalwombat/oscar64) C compiler for the [Foenix F256K](https://wiki.c256foenix.com/index.php?title=F256K) retro computer.

## Attribution

- **f256lib** — the original F256 hardware abstraction library by Scott Duensing, licensed under the MIT License. The library code in `f256lib/` is adapted from this project for compatibility with oscar64.

- **Doodles** — the example programs in `doodles/` are ported from mu0n's [F256KsimpleCdoodles](https://github.com/Mu0n/F256KsimpleCdoodles), a collection of small programs and experiments for the F256K. These were originally written for the llvm-mos/clang toolchain.

- **Examples** — the programs in `examples/` are adapted from the f256-dev examples by Scott Duensing.

## Building

You need the oscar64 compiler with F256K target support. F256K support is currently available on the [f256k-support branch](https://github.com/sdwfrost/oscar64/tree/f256k-support) fork of oscar64. Build from source:

```bash
git clone -b f256k-support https://github.com/sdwfrost/oscar64.git
cd oscar64 && make
cd ..
```

Then build the examples and doodles:

```bash
# Build all examples
cd examples && make

# Build all doodles
cd doodles && make -k
```

The Makefiles expect oscar64 at `../../oscar64/build/oscar64` relative to the `examples/` or `doodles/` directory, i.e., the expected directory layout is:

```
parent/
├── oscar64/           # Clone of oscar64, built from source
│   └── build/
│       └── oscar64    # Compiled compiler binary
└── f256lib-oscar64/   # This repository
    ├── examples/
    └── doodles/
```

Override with `OSCAR64=/path/to/oscar64 make`.

## Directory structure

```
f256lib-oscar64/
├── f256lib/          # Oscar64 port of the f256 hardware library
│   ├── f256lib.h     # Main include (includes all sub-headers)
│   ├── f256_regs.h   # Hardware register definitions
│   ├── f_bitmap.c/h  # Bitmap graphics
│   ├── f_graphics.c/h# Graphics layer and palette control
│   ├── f_kernel.c/h  # Kernel API wrappers
│   ├── f_math.c/h    # Hardware math coprocessor
│   ├── f_sprite.c/h  # Sprite control
│   ├── f_text.c/h    # Text display
│   ├── f_tile.c/h    # Tilemap support
│   └── ...
├── examples/         # Ported f256-dev examples
│   ├── cube/         # 3D wireframe cube
│   ├── lines/        # Random line drawing
│   ├── overlay/      # Code overlay / bank switching demo
│   ├── sprites/      # Sprite animation
│   └── tilemap/      # Tilemap scrolling
└── doodles/          # Ported F256KsimpleCdoodles experiments
    ├── BachHero/     # Guitar Hero-style MIDI game
    ├── mandel/       # Mandelbrot fractal
    ├── circles/      # Circle drawing
    └── ...           # ~80 programs total
```

## Key differences from llvm-mos

The oscar64 port required several adaptations from the original llvm-mos/clang codebase:

- **No `printf()`** — text output uses `textPrint()`, `textPrintInt()`, etc.
- **Anonymous unions** — oscar64 requires named union members (e.g., `kernelEventData.u.key.raw` instead of `kernelEventData.key.raw`).
- **Inline assembly** — uses `__asm volatile { sei }` instead of `asm("sei")`.
- **`#embed` directive** — oscar64 uses `#pragma section/region/data` with `#embed` for binary asset inclusion.
- **Multi-file compilation** — uses `#pragma compile("file.c")` in headers instead of separate compilation units.
- **Code overlays** — uses `#pragma region()` with a runtime address parameter for bank-switched code (see `examples/overlay/`).

## License

MIT License. See [LICENSE](LICENSE) for details.
