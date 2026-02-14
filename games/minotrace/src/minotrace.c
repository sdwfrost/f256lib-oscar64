#pragma stacksize(1024)
#pragma heapsize(0)

#include "f256lib.h"
#include "gamemusic.h"
#include "raycast.h"
#include "maze.h"
#include "display.h"
#include "input.h"
#include <string.h>
#include <stdlib.h>

// C64 VCOL color constants (same indices as C64 VIC-II)
#define VCOL_BLACK       0
#define VCOL_WHITE       1
#define VCOL_RED         2
#define VCOL_CYAN        3
#define VCOL_PURPLE      4
#define VCOL_GREEN       5
#define VCOL_BLUE        6
#define VCOL_YELLOW      7
#define VCOL_ORANGE      8
#define VCOL_BROWN       9
#define VCOL_LT_RED      10
#define VCOL_DARK_GREY   11
#define VCOL_MED_GREY    12
#define VCOL_LT_GREEN    13
#define VCOL_LT_BLUE     14
#define VCOL_LT_GREY     15


enum GameStates
{
	GS_INIT,
	GS_INTRO,
	GS_BUILD,
	GS_REVEAL,
	GS_READY,
	GS_RACE,
	GS_TIMEOUT,
	GS_EXPLOSION,
	GS_FINISHED,
	GS_RETRY,
	GS_GAMEOVER,
	GS_PAUSE,
	GS_COMPLETED,

	NUM_GAME_STATES
}	GameState;

int		GameTime;
char	GameLevel, GameSelect;
bool	GameDown;

struct PlayerStruct
{
	int			ipx, ipy;
	char		w;
	int			vx, vy;
	signed char dw;
	int			acc;

}	Player;


// Draw the maze at the current player position and direction
void maze_draw(void)
{
	char	w = Player.w;

	int		co = costab[w], si = sintab[w];
	int	idx = co + si, idy = si - co;
	int	iddx = dsintab[w], iddy = dcostab[w];

	rcast_cast_rays(Player.ipx, Player.ipy, idx, idy, iddx, iddy);

	rcast_draw_screen();
}

// Flip the display
void maze_flip(void)
{
	display_flip();

	time_draw();
	compass_draw(Player.w);
}

void player_init(void)
{
	Player.ipx = 3 * 128;
	Player.ipy = 25 * 128;
	Player.w = 0;
	Player.vx = 0;
	Player.vy = 0;
	Player.dw = 0;
	Player.acc = 0;
}

// Check player joystick control
void player_control(void)
{
	joy_poll_input();

	if (joyx)
	{
		if (Player.dw == 0)
			Player.dw = 4 * joyx;
		else
		{
			Player.dw += joyx;

			if (Player.dw > 8)
				Player.dw = 8;
			else if (Player.dw < -8)
				Player.dw = -8;
		}

		Player.w = (Player.w + ((Player.dw + 2) >> 2)) & 63;
	}
	else
		Player.dw = 0;

	if (joyb)
		Player.acc = 128;
	else if (joyy > 0)
		Player.acc = -32;
	else if (joyy < 0)
		Player.acc = 32;
	else
		Player.acc = 0;
}

// Fixed-point 8.8 signed multiply
static int lmul8f8s(int a, int b)
{
	return (int)(((long)a * (long)b) >> 8);
}

// Advance player speed and position
void player_move(void)
{
	int		co = costab[Player.w], si = sintab[Player.w];

	int	wx = lmul8f8s(Player.vx, co) + lmul8f8s(Player.vy, si);
	int	wy = lmul8f8s(Player.vy, co) - lmul8f8s(Player.vx, si);

	wx = (wx * 15 + 8) >> 4;
	wy = (wy + 1) >> 1;
	wx += Player.acc;

	if (wx >= 2048)
		wx = 2048;
	else if (wx < -2048)
		wx = -2048;

	int	vx = lmul8f8s(wx, co) - lmul8f8s(wy, si);
	int	vy = lmul8f8s(wy, co) + lmul8f8s(wx, si);

	int	ipx = Player.ipx + ((vx + 8) >> 4);
	int	ipy = Player.ipy + ((vy + 8) >> 4);

	static const int wdist = 0x40;
	static const int bspeed = 0x100;

	bool	bounce = false;

	if (vx > 0 && maze_inside(ipx + wdist, ipy))
	{
		ipx = ((ipx + wdist) & 0xff00) - wdist;
		if (vx > bspeed)
			bounce = true;
		vx = -(vx >> 1);
	}
	else if (vx < 0 && maze_inside(ipx - wdist, ipy))
	{
		ipx = ((ipx - wdist) & 0xff00) + (0x100 + wdist);
		if (vx < -bspeed)
			bounce = true;
		vx = -vx >> 1;
	}

	if (vy > 0 && maze_inside(ipx, ipy + wdist))
	{
		ipy = ((ipy + wdist) & 0xff00) - wdist;
		if (vy > bspeed)
			bounce = true;
		vy = -(vy >> 1);
	}
	else if (vy < 0 && maze_inside(ipx, ipy - wdist))
	{
		ipy = ((ipy - wdist) & 0xff00) + (0x100 + wdist);
		if (vy < -bspeed)
			bounce = true;
		vy = -vy >> 1;
	}

	Player.ipx = ipx;
	Player.ipy = ipy;
	Player.vx = vx;
	Player.vy = vy;

	if (bounce)
	{
		sidfx_play_effect(SFX_BOUNCE);
	}
}


// Table of levels
struct MazeInfo Levels[27] =
{
	{
		MGEN_CURVES_1, 0xa321,
		34, (VCOL_GREEN << 4) | VCOL_PURPLE,
		TUNE_GAME_2, 20
	},

	{
		MGEN_CURVES_2, 0xa321,
		33, (VCOL_RED << 4) | VCOL_BLUE,
		TUNE_GAME_2, 20
	},

	{
		MGEN_GATES, 0xa321,
		33, (VCOL_YELLOW << 4) | VCOL_ORANGE,
		TUNE_GAME_3, 20
	},

	{
		MGEN_CURVES_1, 0xa321,
		66, (VCOL_LT_BLUE << 4) | VCOL_GREEN,
		TUNE_GAME_3, 35
	},

	{
		MGEN_DOORS, 0xa321,
		35, (VCOL_LT_GREEN << 4) | VCOL_MED_GREY,
		TUNE_GAME_4, 20
	},

	{
		MGEN_MINEFIELD, 0xa321,
		34, (VCOL_CYAN << 4) | VCOL_BLUE,
		TUNE_GAME_2, 20
	},

	{
		MGEN_CURVES_2, 0xa321,
		65, (VCOL_ORANGE << 4) | VCOL_LT_BLUE,
		TUNE_GAME_3, 35
	},

	{
		MGEN_GATES, 0x1781,
		66, (VCOL_PURPLE << 4) | VCOL_YELLOW,
		TUNE_GAME_4, 30
	},

	{
		MGEN_LABYRINTH_3, 0xa321,
		34, (VCOL_DARK_GREY << 4) | VCOL_RED,
		TUNE_GAME_2, 20
	},

	{
		MGEN_LABYRINTH_1, 0xf921,
		20, (VCOL_YELLOW << 4) | VCOL_PURPLE,
		TUNE_GAME_3, 30
	},

	{
		MGEN_DOORS, 0x4521,
		55, (VCOL_CYAN << 4) | VCOL_ORANGE,
		TUNE_GAME_4, 30
	},

	{
		MGEN_CURVES_1, 0xa321,
		130, (VCOL_BLUE << 4) | VCOL_MED_GREY,
		TUNE_GAME_4, 45
	},

	{
		MGEN_CURVES_2, 0xa321,
		129, (VCOL_YELLOW << 4) | VCOL_RED,
		TUNE_GAME_1, 65
	},

	{
		MGEN_CURVES_1, 0xa321,
		226, (VCOL_GREEN << 4) | VCOL_BLUE,
		TUNE_GAME_1, 80
	},

	{
		MGEN_GATES, 0x9fb2,
		132, (VCOL_LT_BLUE << 4) | VCOL_MED_GREY,
		TUNE_GAME_3, 55
	},

	{
		MGEN_LABYRINTH_3, 0x2482,
		66, (VCOL_RED << 4) | VCOL_BLUE,
		TUNE_GAME_3, 30
	},

	{
		MGEN_LABYRINTH_1, 0xa321,
		34, (VCOL_GREEN << 4) | VCOL_PURPLE,
		TUNE_GAME_4, 60
	},

	{
		MGEN_MINEFIELD, 0x7951,
		34, (VCOL_GREEN << 4) | VCOL_RED,
		TUNE_GAME_3, 30
	},

	{
		MGEN_LABYRINTH_1, 0x2197,
		52, (VCOL_LT_GREEN << 4) | VCOL_DARK_GREY,
		TUNE_GAME_4, 70
	},

	{
		MGEN_LABYRINTH_3, 0x9812,
		98, (VCOL_MED_GREY << 4) | VCOL_LT_BLUE,
		TUNE_GAME_2, 60
	},

	{
		MGEN_DOORS, 0x7491,
		105, (VCOL_GREEN << 4) | VCOL_PURPLE,
		TUNE_GAME_1, 60
	},

	{
		MGEN_GATES, 0xe8b1,
		252, (VCOL_YELLOW << 4) | VCOL_CYAN,
		TUNE_GAME_1, 105
	},

	{
		MGEN_MINEFIELD, 0xa952,
		100, (VCOL_DARK_GREY << 4) | VCOL_ORANGE,
		TUNE_GAME_1, 120
	},

	{
		MGEN_CURVES_2, 0xa321,
		225, (VCOL_GREEN << 4) | VCOL_PURPLE,
		TUNE_GAME_1, 90
	},

	{
		MGEN_LABYRINTH_3, 0xfe12,
		126, (VCOL_YELLOW << 4) | VCOL_CYAN,
		TUNE_GAME_1, 90
	},

	{
		MGEN_LABYRINTH_3, 0xfe12,
		254, (VCOL_ORANGE << 4) | VCOL_LT_GREEN,
		TUNE_GAME_1, 180
	},

	{
		MGEN_LABYRINTH_1, 0xcf1d,
		100, (VCOL_RED << 4) | VCOL_BLUE,
		TUNE_GAME_1, 240
	},
};


// Advance game state machine
void game_advance(GameStates state)
{
	while (state != GameState)
	{
		GameState = state;

		switch (GameState)
		{
			case GS_INIT:
				break;
			case GS_INTRO:
				music_patch_voice3(false);
				music_init(TUNE_MAIN);

				display_title();

				display_game();
				rcast_init_tables();

				GameLevel = 0;
				state = GS_BUILD;
				break;
			case GS_BUILD:
				maze_build(Levels + GameLevel);
				music_patch_voice3(false);
				music_init(Levels[GameLevel].tune);

				time_running = false;
				time_init(Levels[GameLevel].time);
				time_draw();
				state = GS_REVEAL;
				break;
			case GS_REVEAL:
				break;
			case GS_READY:
				player_init();
				GameTime = 0;
				break;
			case GS_RACE:
				time_running = true;
				break;
			case GS_EXPLOSION:
				sidfx_play_effect(SFX_EXPLOSION);
				maze_set(Player.ipx, Player.ipy, MF_EMPTY);
				display_explosion();
				GameTime = 0;
				break;
			case GS_TIMEOUT:
				GameTime = 0;
				Player.acc = 0;
				break;
			case GS_FINISHED:
				GameTime = 0;
				Player.acc = 0;
				break;
			case GS_PAUSE:
				GameSelect = 0;
				time_running = false;
				break;
			case GS_RETRY:
				music_patch_voice3(true);
				music_init(TUNE_RESTART);
				GameTime = 0;
				GameSelect = 0;
				Player.acc = 0;
				break;
			case GS_GAMEOVER:
				break;

			case GS_COMPLETED:
				music_patch_voice3(false);
				music_init(TUNE_MAIN);

				display_completed();

				display_game();
				rcast_init_tables();

				GameLevel = 0;
				state = GS_BUILD;
				break;
		}
	}
}

bool 	game_beep;

void game_loop(void)
{
	// Update sound effects each frame
	sidfx_update();

	// Decrement timer each frame if running
	if (time_running)
		time_dec();

	switch (GameState)
	{
		case GS_INIT:
			break;
		case GS_INTRO:
			break;
		case GS_REVEAL:
			{
				char	t[3];
				t[0] = '0' + (GameLevel + 1) / 10;
				t[1] = '0' + (GameLevel + 1) % 10;
				t[2] = 0;
				display_put_bigtext(8, 4, "lvl", BC_WHITE);
				display_put_bigtext(12, 14, t, BC_WHITE);
			}
			maze_preview();
			game_advance(GS_READY);
			break;
		case GS_READY:
			maze_draw();

			if (GameTime == 0 || GameTime == 25)
				sidfx_play_effect(SFX_BEEP_SHORT);
			else if (GameTime == 50)
				sidfx_play_effect(SFX_BEEP_LONG);

			if (GameTime < 25)
				display_put_bigtext(0, 10, "ready", BC_WHITE);
			else if (GameTime < 50)
				display_put_bigtext(8, 10, "set", BC_WHITE);
			else if (GameTime < 60)
				display_put_bigtext(12, 10, "go", BC_WHITE);
			else
				game_advance(GS_RACE);

			maze_flip();

			player_control();
			GameTime++;
			break;
		case GS_RACE:
			maze_draw();

			if (time_count <= 10)
			{
				if (time_digits[3] >= 4)
				{
					char	t[2];
					t[0] = time_digits[2] + '0';
					t[1] = 0;
					display_put_bigtext(16, 10, t, BC_WHITE);
					if (!game_beep)
					{
						sidfx_play_effect(SFX_BEEP_HURRY);
						game_beep = true;
					}
				}
				else
					game_beep = false;
			}

			maze_flip();

			player_control();
			player_move();

			if (joy_pause)
				game_advance(GS_PAUSE);
			else if (time_count == 0)
				game_advance(GS_TIMEOUT);
			else
			{
				MazeFields	field = maze_field(Player.ipx, Player.ipy);
				if (field == MF_EXIT)
					game_advance(GS_FINISHED);
				else if (field == MF_MINE)
					game_advance(GS_EXPLOSION);
			}
			break;

		case GS_PAUSE:
			if (joyb)
			{
				if (GameSelect == 1)
				{
					display_reset();
					game_advance(GS_BUILD);
				}
				else if (GameSelect == 2)
				{
					display_reset();
					game_advance(GS_INTRO);
				}
				else
				{
					game_advance(GS_RACE);
				}
			}
			else
			{
				maze_draw();

				display_put_bigtext(4,  1, "cont", GameSelect == 0 ? BC_BOX_RED : BC_BOX_BLACK);
				display_put_bigtext(0,  9, "retry", GameSelect == 1 ? BC_BOX_RED : BC_BOX_BLACK);
				display_put_bigtext(4, 17, "exit", GameSelect == 2 ? BC_BOX_RED : BC_BOX_BLACK);

				maze_flip();

				joy_poll_input();

				if (joyy == 0)
					GameDown = false;
				else if (!GameDown)
				{
					if (joyy > 0 && GameSelect < 2)
					{
						GameSelect++;
						GameDown = true;
					}
					else if (joyy < 0 && GameSelect > 0)
					{
						GameSelect--;
						GameDown = true;
					}
				}
			}

			break;
		case GS_EXPLOSION:
			if (GameTime == 30)
			{
				joy_poll_input();
				if (!joyb)
					game_advance(GS_RETRY);
			}
			else
			{
				maze_draw();

				display_put_bigtext(4, 4, "boom", BC_WHITE);
				display_put_bigtext(0, 13, "crash", BC_WHITE);

				maze_flip();

				Player.w = (Player.w + ((30 - GameTime) >> 2)) & 63;

				GameTime++;
			}
			break;
		case GS_TIMEOUT:
			if (GameTime == 30)
			{
				joy_poll_input();
				if (!joyb)
					game_advance(GS_RETRY);
			}
			else
			{
				maze_draw();

				display_put_bigtext(4, 4, "outa", BC_WHITE);
				display_put_bigtext(4, 13, "time", BC_WHITE);

				maze_flip();
				player_move();

				GameTime++;
			}
			break;
		case GS_FINISHED:
			if (GameTime == 30)
			{
				GameLevel++;
				display_reset();
				if (GameLevel == 27)
					game_advance(GS_COMPLETED);
				else
					game_advance(GS_BUILD);
			}
			else
			{
				display_five_star(GameTime * 8);
				maze_flip();
				GameTime++;
			}
			break;
		case GS_RETRY:
			if (GameTime == 120 || joyb)
			{
				display_reset();
				if (GameSelect)
					game_advance(GS_INTRO);
				else
					game_advance(GS_BUILD);
			}
			else
			{
				maze_draw();

				display_put_bigtext(0, 4, "retry", GameSelect == 0 ? BC_BOX_RED : BC_BOX_BLACK);
				display_put_bigtext(4, 13, "exit", GameSelect == 1 ? BC_BOX_RED : BC_BOX_BLACK);

				maze_flip();
				player_move();

				joy_poll_input();
				if (joyy > 0)
					GameSelect = 1;
				else if (joyy < 0)
					GameSelect = 0;

				GameTime++;
			}
			break;
		case GS_GAMEOVER:
			break;
	}
}

int main(int argc, char *argv[])
{
	display_init();

	game_advance(GS_INTRO);

	for(;;)
	{
		game_loop();
	}

	return 0;
}
