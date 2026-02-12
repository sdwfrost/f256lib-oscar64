# OscarTutorials for F256K (f256lib-oscar64)

Ports of the [OscarTutorials](https://github.com/drmortalwombat/OscarTutorials) (C64 programming tutorials for the oscar64 compiler) to the **Foenix F256K** using the **f256lib** library.

## Building

Requires [oscar64](https://github.com/drmortalwombat/oscar64) in your PATH.

```bash
cd f256lib-oscar64/tutorials
make
```

Each tutorial compiles to a `.pgz` file that can be loaded on the F256K or in the FoenixIDE emulator.

## Tutorial Index

### Basics (0010–0070)

| # | Name | Description | Status |
|---|------|-------------|--------|
| 0010 | HelloWorld | Print text to screen | Ported |
| 0020 | Diagonals | Diagonal text output with cursor positioning | Ported |
| 0030 | KeyWait | Wait for keypress | Ported |
| 0040 | Lowercase | Switch to lowercase character set | Ported |
| 0050 | Salutation | Read user input and respond | Ported |
| 0060 | ColorChars | Colored text output | Ported |
| 0070 | Guessing | Number guessing game | Ported |

### Screen Memory (0100–0300)

| # | Name | Description | Status |
|---|------|-------------|--------|
| 0100 | ScreenMem | Direct VRAM text writes via I/O page | Ported |
| 0110 | ColorMem | Direct color RAM writes via I/O page | Ported |
| 0200 | CursorMove | Arrow-key cursor movement | Ported |
| 0300 | Labyrinth | Random maze generator with cursor exploration | Ported |

### PEEK and POKE (0400–0410)

| # | Name | Description | Status |
|---|------|-------------|--------|
| 0400 | PeekAndPoke | Direct hardware register access | Ported |
| 0410 | NoPeekAndPoke | Structured alternatives to PEEK/POKE | Ported |

### Border and Raster (1000–1010)

| # | Name | Description | Status |
|---|------|-------------|--------|
| 1000 | BorderColor | Cycle border colors | Ported |
| 1010 | RasterLine | Raster-racing border color effect | Ported |

### Character ROM (1100–1120)

| # | Name | Description | Status |
|---|------|-------------|--------|
| 1100 | CharRom | Read and display font data from character ROM | Ported |
| 1110 | CustomChar | Define a custom character glyph | Ported |
| 1120 | CopyChars | Copy and modify character set | Ported |
| 1130 | CharPad | CharPad .ctm resource format | Skipped (C64-specific) |
| 1140 | CharPadMulticolor | Multicolor CharPad | Skipped (C64-specific) |
| 1150 | SpritePad | SpritePad .spd format | Skipped (C64-specific) |
| 1160 | SpritePadMulticolor | Multicolor SpritePad | Skipped (C64-specific) |

### Scrolling (1200–1280)

| # | Name | Description | Status |
|---|------|-------------|--------|
| 1200 | ScrollUp | Scroll text up line by line | Ported |
| 1210 | ScrollUpFast | Fast VRAM-copy scroll | Ported |
| 1220 | HardwareScrollUp | VIC-II CTRL1 scroll register | Skipped (no equivalent) |
| 1230 | HardwareScrollLR | VIC-II CTRL2 horizontal scroll | Skipped (no equivalent) |
| 1240 | HardwareScrollMap | Hardware scroll with tile map | Skipped (no equivalent) |
| 1250 | HardwareScrollBig | Large scrolling map | Skipped (no equivalent) |
| 1260 | HardwareScrollStars | Parallax star scroll | Skipped (no equivalent) |
| 1270 | HardwareScrollJoystick | Joystick-controlled scroll | Skipped (no equivalent) |
| 1280 | HardwareScrollSmooth | Smooth scroll with VIC-II | Skipped (no equivalent) |

### Sprites (1300–1380)

| # | Name | Description | Status |
|---|------|-------------|--------|
| 1300 | StaticSprite | Display a static sprite | Ported |
| 1310 | MovingSprite | Keyboard-controlled sprite movement | Ported |
| 1320 | ReflectingSprite | Sprite bouncing off screen edges | Ported |
| 1330 | CollidingSprite | Two sprites with collision detection | Ported |
| 1340 | FractionalSpritePos | Sub-pixel sprite positioning | Ported |
| 1350 | GravitySprite | Sprite with gravity physics | Ported |
| 1360 | BouncingSprite | Multiple bouncing sprites | Ported |
| 1370 | AnimatedSprite | Frame-based sprite animation | Ported |
| 1380 | AnimatedSpritePad | SpritePad animated sprite | Skipped (C64-specific) |

### Interrupts (1400–1480)

| # | Name | Description | Status |
|---|------|-------------|--------|
| 1400–1480 | Various | CIA/VIC-II raster interrupt system | Skipped (C64-specific) |

### Bitmap (1500–1540)

| # | Name | Description | Status |
|---|------|-------------|--------|
| 1500 | BitmapPixels | Lissajous curve in bitmap mode | Ported |
| 1520 | BitmapImage | C64 bitmap image display | Skipped (C64-specific) |
| 1530 | BitmapScroll | C64 bitmap scrolling | Skipped (C64-specific) |
| 1540 | BitmapMulticolor | C64 multicolor bitmap | Skipped (C64-specific) |

### Sprite-Background Interaction (1600–1630)

| # | Name | Description | Status |
|---|------|-------------|--------|
| 1600 | SpriteBackCollision | Sprite vs background char detection | **New** |
| 1610 | SpriteBackBlocking | Blocking sprite movement against walls | **New** |
| 1620 | BigSpriteBlocking | Multi-char sprite blocking | **New** |
| 1630 | BallSpriteBlocking | Ball physics with wall blocking | **New** |

### Sprite Multiplexing (1700–1750)

| # | Name | Description | Status |
|---|------|-------------|--------|
| 1700–1750 | Various | VIC-II sprite multiplexing | Skipped (F256K has 64 sprites) |

### Color Techniques (1800–1820)

| # | Name | Description | Status |
|---|------|-------------|--------|
| 1800–1820 | Various | VIC-II per-cell color tricks | Skipped (C64-specific) |

### SID Music (2000–2010)

| # | Name | Description | Status |
|---|------|-------------|--------|
| 2000 | SIDMusicPlayback | C major scale on F256K SID chip | **New** |
| 2010 | SIDMusicInterrupt | Frame-timed melody with vblank sync | **New** |

### Text Adventure (3000–3050)

| # | Name | Description | Status |
|---|------|-------------|--------|
| 3000 | AdventureTokens | Input tokenizer | Ported |
| 3010 | AdventureParse | Vocabulary parser | Ported |
| 3020 | AdventureMap | Room navigation | Ported |
| 3030 | AdventureItems | Inventory system | Ported |
| 3040 | AdventureDoors | Doors and passages | Ported |
| 3050 | AdventureKeys | Key-locked doors | Ported |

### Math (4000–4030)

| # | Name | Description | Status |
|---|------|-------------|--------|
| 4000 | FloatNumbers | Float arithmetic circle test | Ported |
| 4010 | FixPointNumbers | 4.12 fixed-point circle test | Ported |
| 4020 | FixPointMult | fixmath.h multiply with timing | Ported |
| 4030 | FixPointTable | Square-table multiplication | Ported |

### Trigonometry (4200–4260)

| # | Name | Description | Status |
|---|------|-------------|--------|
| 4200 | AtanFloat | Float atan2 with crosshair sprite | Ported |
| 4210 | AtanTable | Lookup table atan2 | Ported |
| 4220 | AtanCordic | CORDIC atan2 (16-bit) | Ported |
| 4230 | AtanCordicByte | CORDIC atan2 (8-bit) | Ported |
| 4240 | CosinFloat | Float sin/cos circle motion | Ported |
| 4250 | CosinTable | Lookup table sin/cos | Ported |
| 4260 | CosinCordic | CORDIC sin/cos | Ported |

### Distance (4270–4290)

| # | Name | Description | Status |
|---|------|-------------|--------|
| 4270 | DistanceSqrt | Integer square root distance | Ported |
| 4280 | DistanceTable | Table-based distance | Ported |
| 4290 | DistanceCordic | CORDIC distance | Ported |

### Memory Banking (4500–4530)

| # | Name | Description | Status |
|---|------|-------------|--------|
| 4500–4530 | Various | C64 PLA memory banking | Skipped (different architecture) |

### Vector Animation (5000–5030)

| # | Name | Description | Status |
|---|------|-------------|--------|
| 5000 | VectorAnimXor | Wireframe star, clear-and-redraw | **New** |
| 5010 | VectorAnimXorDelay | Wireframe with trail effect | **New** |
| 5020 | VectorAnimDoubleBuffer | True double-buffered animation | **New** |
| 5030 | VectorAnimDBuffClear | Double buffer with full clear | **New** |

## Summary

- **59 tutorials ported/created** (49 re-ported + 10 new)
- **26 tutorials skipped** (C64-specific hardware features with no F256K equivalent)

## Key Differences from C64 Originals

| Feature | C64 (OscarTutorials) | F256K (f256lib) |
|---------|----------------------|-----------------|
| Screen size | 40×25 (320×200) | 80×60 (320×240) |
| Text colors | PETSCII codes | `textSetColor(fg, bg)` with Apple IIgs palette |
| Sprites | 8 hardware, 24×21 1-bit | 64 hardware, 24×24 8bpp |
| Sprite coords | Screen-relative | +32 offset (via `sprite_util.h`) |
| Collision | VIC-II hardware register | Software bounding-box |
| Joystick | CIA port reads | Keyboard arrow keys |
| Bitmap | 1-bit hires/multicolor | 8bpp with CLUT |
| Bitmap XOR | VIC-II native | Not available; use clear-and-redraw |
| SID | Direct register pokes | f256lib SID API |
| Scrolling | VIC-II CTRL1/CTRL2 | Software VRAM copy |
| Interrupts | CIA/VIC-II raster IRQ | `graphicsWaitVerticalBlank()` |
| Memory | 64KB + PLA banking | MMU with 8KB banks, I/O pages |

## Shared Files

- **`sprite_util.h`** — C64→8bpp sprite expansion, sprite management wrappers, collision detection
- **`adventure.h` / `adventure.c`** — Shared adventure engine (per-tutorial copies with progressive features)
