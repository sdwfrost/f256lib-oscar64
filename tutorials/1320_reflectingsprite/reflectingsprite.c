// 1320 ReflectingSprite - Bouncing sprites off screen edges
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

int main(int argc, char *argv[])
{
	sprite_init();

	for (byte i = 0; i < 8; i++)
	{
		sprites[i].sx = rand() % 296;
		sprites[i].sy = rand() % 216;
		sprites[i].vx = rand() % 5 - 2;
		sprites[i].vy = rand() % 5 - 2;

		sprite_expand_c64(SpriteImage, i, i + 8);
		sprite_set(i, true, sprites[i].sx, sprites[i].sy, i, i + 8);
	}

	while (true)
	{
		for (byte i = 0; i < 8; i++)
		{
			int sx = sprites[i].sx + sprites[i].vx;
			int sy = sprites[i].sy + sprites[i].vy;

			if (sx < 0 || sx > 296)
				sprites[i].vx = -sprites[i].vx;
			else
				sprites[i].sx = sx;

			if (sy < 0 || sy > 216)
				sprites[i].vy = -sprites[i].vy;
			else
				sprites[i].sy = sy;

			sprite_move(i, sprites[i].sx, sprites[i].sy);
		}
		graphicsWaitVerticalBlank();
	}

	return 0;
}
