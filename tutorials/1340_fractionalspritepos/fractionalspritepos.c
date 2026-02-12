// 1340 FractionalSpritePos - Sub-pixel sprite movement
// Ported from OscarTutorials to F256K using f256lib

#include "f256lib.h"
#include "sprite_util.h"
#include <stdlib.h>

const char SpriteImage[64] = {
	0b00000000, 0b11111000, 0b00000000,
	0b00000011, 0b11111110, 0b00000000,
	0b00001111, 0b11111111, 0b10000000,
	0b00011111, 0b11111111, 0b11000000,
	0b00111111, 0b11111111, 0b11100000,
	0b00111111, 0b11111111, 0b11100000,
	0b01111111, 0b11111111, 0b11110000,
	0b01111111, 0b11111111, 0b11110000,
	0b11111111, 0b11111111, 0b11111000,
	0b11111111, 0b11111111, 0b11111000,
	0b11111111, 0b11111111, 0b11111000,
	0b11111111, 0b11111111, 0b11111000,
	0b11111111, 0b11111111, 0b11111000,
	0b01111111, 0b11111111, 0b11110000,
	0b01111111, 0b11111111, 0b11110000,
	0b00111111, 0b11111111, 0b11100000,
	0b00111111, 0b11111111, 0b11100000,
	0b00011111, 0b11111111, 0b11000000,
	0b00001111, 0b11111111, 0b10000000,
	0b00000011, 0b11111110, 0b00000000,
	0b00000000, 0b11111000, 0b00000000
};

struct RefSprite {
	int sx, sy, vx, vy;
} sprites[8];

#define FBITS 4

int main(int argc, char *argv[])
{
	sprite_init();

	for (byte i = 0; i < 8; i++)
	{
		sprites[i].sx = (rand() % 296) << FBITS;
		sprites[i].sy = (rand() % 216) << FBITS;
		sprites[i].vx = rand() % 81 - 40;
		sprites[i].vy = rand() % 81 - 40;

		sprite_expand_c64(SpriteImage, i, 0);
		sprite_set(i, true, sprites[i].sx >> FBITS, sprites[i].sy >> FBITS, i, 0);
	}

	while (true)
	{
		byte collflags = sprite_check_collisions();

		for (byte i = 0; i < 8; i++)
		{
			int sx = sprites[i].sx + sprites[i].vx;
			int sy = sprites[i].sy + sprites[i].vy;

			if (sx < 0 || sx > (296 << FBITS))
				sprites[i].vx = -sprites[i].vx;
			else
				sprites[i].sx = sx;

			if (sy < 0 || sy > (216 << FBITS))
				sprites[i].vy = -sprites[i].vy;
			else
				sprites[i].sy = sy;

			sprite_move(i, sprites[i].sx >> FBITS, sprites[i].sy >> FBITS);

			byte new_color = (collflags & (1 << i)) ? 7 : 1;
			sprite_recolor(i, SpriteImage, new_color);
		}
		graphicsWaitVerticalBlank();
	}

	return 0;
}
