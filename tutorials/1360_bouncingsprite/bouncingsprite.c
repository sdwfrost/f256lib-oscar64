// 1360 BouncingSprite - Elastic collision physics
// Ported from OscarTutorials to F256K using f256lib

#include "f256lib.h"
#include "sprite_util.h"
#include <stdlib.h>

// Integer square root (replaces fixmath usqrt)
static unsigned usqrt(unsigned n)
{
	unsigned root = 0;
	unsigned bit = 1u << 14;  // highest power of 4 <= 16-bit max

	while (bit > n)
		bit >>= 2;

	while (bit != 0)
	{
		if (n >= root + bit)
		{
			n -= root + bit;
			root = (root >> 1) + bit;
		}
		else
		{
			root >>= 1;
		}
		bit >>= 2;
	}
	return root;
}

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

#define FBITS   3
#define FRADIUS (10 << FBITS)
#define FBITS2  4
#define FBITS3  5
#define FROUND3 (1 << (FBITS3 - 1))

int main(int argc, char *argv[])
{
	sprite_init();

	for (byte i = 0; i < 8; i++)
	{
		sprites[i].sx = (rand() % 296) << FBITS;
		sprites[i].sy = (rand() % 216) << FBITS;
		sprites[i].vx = rand() % 101 - 50;
		sprites[i].vy = rand() % 101 - 50;

		sprite_expand_c64(SpriteImage, i, i + 7);
		sprite_set(i, true, sprites[i].sx >> FBITS, sprites[i].sy >> FBITS, i, i + 7);
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

			if (sy < 0)
				sprites[i].vy = -sprites[i].vy;
			else if (sy > (216 << FBITS))
				sprites[i].vy = 1 - sprites[i].vy;
			else
				sprites[i].sy = sy;

			sprite_move(i, sprites[i].sx >> FBITS, sprites[i].sy >> FBITS);

			if (collflags & (1 << i))
			{
				for (byte j = i + 1; j < 8; j++)
				{
					if (collflags & (1 << j))
					{
						int dx = sprites[i].sx - sprites[j].sx;
						int dy = sprites[i].sy - sprites[j].sy;

						if (dx >= -2 * FRADIUS && dx <= 2 * FRADIUS &&
						    dy >= -2 * FRADIUS && dy < 2 * FRADIUS)
						{
							unsigned dd = dx * dx + dy * dy;
							if (dd <= 4 * FRADIUS * FRADIUS)
							{
								int dds = usqrt(dd);
								int nx = ((dx << FBITS2) + (dds >> 1)) / dds;
								int ny = ((dy << FBITS2) + (dds >> 1)) / dds;
								int vi = sprites[i].vx * nx + sprites[i].vy * ny;
								int vj = sprites[j].vx * nx + sprites[j].vy * ny;
								if (vi - vj < 0)
								{
									int vij = (vi - vj + (1 << (FBITS - 1))) >> FBITS;
									sprites[i].vx -= (nx * vij + FROUND3) >> FBITS3;
									sprites[i].vy -= (ny * vij + FROUND3) >> FBITS3;
									sprites[j].vx += (nx * vij + FROUND3) >> FBITS3;
									sprites[j].vy += (ny * vij + FROUND3) >> FBITS3;
								}
							}
						}
					}
				}
			}
			sprites[i].vy++;
		}
		graphicsWaitVerticalBlank();
	}

	return 0;
}
