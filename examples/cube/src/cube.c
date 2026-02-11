/*
 *	Copyright (c) 2024 Scott Duensing, scott@kangaroopunch.com
 *
 *	Permission is hereby granted, free of charge, to any person obtaining a copy
 *	of this software and associated documentation files (the "Software"), to deal
 *	in the Software without restriction, including without limitation the rights
 *	to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 *	copies of the Software, and to permit persons to whom the Software is
 *	furnished to do so, subject to the following conditions:
 *
 *	The above copyright notice and this permission notice shall be included in
 *	all copies or substantial portions of the Software.
 *
 *	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 *	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 *	SOFTWARE.
 */


// Ported from https://github.com/root42/doscube


#include "f256lib.h"


#define FIX_PREC 9
#define TO_FIX(x) ((int32_t)((x)*(1<<FIX_PREC)))  // This is only used for initialization and needs to be constant.
#define TO_LONG(x) (x>>FIX_PREC)  // ((x)/(1<<FIX_PREC))

#define fix_mul(am,bm) (mathSignedMultiply(am, bm) >> FIX_PREC)
#define fix_sqr(am)   (mathSignedMultiply(am, am) >> FIX_PREC)
#define fix_div(num,den) (mathSignedMultiply(mathUnsignedDivision(0x8000, (den >> 1)), num) >> 7)  // ((a << FIX_PREC) / b)

#define SIN_SIZE 512
#define COS_OFF  128


int16_t SIN[] = { 0, 6, 12, 18, 25, 31, 37, 43, 50, 56, 62, 68, 75, 81, 87, 93,
			  99, 106, 112, 118, 124, 130, 136, 142, 148, 154, 160, 166, 172,
			  178, 184, 190, 195, 201, 207, 213, 218, 224, 230, 235, 241, 246,
			  252, 257, 263, 268, 273, 279, 284, 289, 294, 299, 304, 310, 314,
			  319, 324, 329, 334, 339, 343, 348, 353, 357, 362, 366, 370, 375,
			  379, 383, 387, 391, 395, 399, 403, 407, 411, 414, 418, 422, 425,
			  429, 432, 435, 439, 442, 445, 448, 451, 454, 457, 460, 462, 465,
			  468, 470, 473, 475, 477, 479, 482, 484, 486, 488, 489, 491, 493,
			  495, 496, 498, 499, 500, 502, 503, 504, 505, 506, 507, 508, 508,
			  509, 510, 510, 511, 511, 511, 511, 511, 512, 511, 511, 511, 511,
			  511, 510, 510, 509, 508, 508, 507, 506, 505, 504, 503, 502, 500,
			  499, 498, 496, 495, 493, 491, 489, 488, 486, 484, 482, 479, 477,
			  475, 473, 470, 468, 465, 462, 460, 457, 454, 451, 448, 445, 442,
			  439, 435, 432, 429, 425, 422, 418, 414, 411, 407, 403, 399, 395,
			  391, 387, 383, 379, 375, 370, 366, 362, 357, 353, 348, 343, 339,
			  334, 329, 324, 319, 314, 310, 304, 299, 294, 289, 284, 279, 273,
			  268, 263, 257, 252, 246, 241, 235, 230, 224, 218, 213, 207, 201,
			  195, 190, 184, 178, 172, 166, 160, 154, 148, 142, 136, 130, 124,
			  118, 112, 106, 99, 93, 87, 81, 75, 68, 62, 56, 50, 43, 37, 31,
			  25, 18, 12, 6, 0, -6, -12, -18, -25, -31, -37, -43, -50, -56,
			  -62, -68, -75, -81, -87, -93, -99, -106, -112, -118, -124, -130,
			  -136, -142, -148, -154, -160, -166, -172, -178, -184, -190, -195,
			  -201, -207, -213, -218, -224, -230, -235, -241, -246, -252, -257,
			  -263, -268, -273, -279, -284, -289, -294, -299, -304, -310, -314,
			  -319, -324, -329, -334, -339, -343, -348, -353, -357, -362, -366,
			  -370, -375, -379, -383, -387, -391, -395, -399, -403, -407, -411,
			  -414, -418, -422, -425, -429, -432, -435, -439, -442, -445, -448,
			  -451, -454, -457, -460, -462, -465, -468, -470, -473, -475, -477,
			  -479, -482, -484, -486, -488, -489, -491, -493, -495, -496, -498,
			  -499, -500, -502, -503, -504, -505, -506, -507, -508, -508, -509,
			  -510, -510, -511, -511, -511, -511, -511, -512, -511, -511, -511,
			  -511, -511, -510, -510, -509, -508, -508, -507, -506, -505, -504,
			  -503, -502, -500, -499, -498, -496, -495, -493, -491, -489, -488,
			  -486, -484, -482, -479, -477, -475, -473, -470, -468, -465, -462,
			  -460, -457, -454, -451, -448, -445, -442, -439, -435, -432, -429,
			  -425, -422, -418, -414, -411, -407, -403, -399, -395, -391, -387,
			  -383, -379, -375, -370, -366, -362, -357, -353, -348, -343, -339,
			  -334, -329, -324, -319, -314, -310, -304, -299, -294, -289, -284,
			  -279, -273, -268, -263, -257, -252, -246, -241, -235, -230, -224,
			  -218, -213, -207, -201, -195, -190, -184, -178, -172, -166, -160,
			  -154, -148, -142, -136, -130, -124, -118, -112, -106, -99, -93,
			  -87, -81, -75, -68, -62, -56, -50, -43, -37, -31, -25, -18, -12,
			  -6, 0, 6, 12, 18, 25, 31, 37, 43, 50, 56, 62, 68, 75, 81, 87, 93,
			  99, 106, 112, 118, 124, 130, 136, 142, 148, 154, 160, 166, 172,
			  178, 184, 190, 195, 201, 207, 213, 218, 224, 230, 235, 241, 246,
			  252, 257, 263, 268, 273, 279, 284, 289, 294, 299, 304, 310, 314,
			  319, 324, 329, 334, 339, 343, 348, 353, 357, 362, 366, 370, 375,
			  379, 383, 387, 391, 395, 399, 403, 407, 411, 414, 418, 422, 425,
			  429, 432, 435, 439, 442, 445, 448, 451, 454, 457, 460, 462, 465,
			  468, 470, 473, 475, 477, 479, 482, 484, 486, 488, 489, 491, 493,
			  495, 496, 498, 499, 500, 502, 503, 504, 505, 506, 507, 508, 508,
			  509, 510, 510, 511, 511, 511, 511, 511
};

typedef struct lineS {
	int16_t x1;
	int16_t y1;
	int16_t x2;
	int16_t y2;
} lineT;

int16_t *COS = SIN + COS_OFF;
uint16_t widthOffset;
uint16_t heightOffset;
int32_t  a     = TO_FIX(4);
int32_t  scale = TO_FIX(40);
lineT    past[12][2];


void draw_cube(byte p, int16_t t) {

	int32_t  cubeRotX[8];
	int32_t  cubeRotY[8];
	int32_t  cubeRotZ[8];
	int32_t  tempY;
	int32_t  tempZ;
	int32_t  cubeProjX[8];
	int32_t  cubeProjY[8];
	int16_t  i;
	byte     l;

	static const int16_t edges[] = {
		0, 1,  1, 3,  3, 2,  2, 0,
		1, 5,  0, 4,  2, 6,  3, 7,
		4, 5,  5, 7,  7, 6,  6, 4
	};

	static const int32_t cubeX[] = {
		TO_FIX(-1), TO_FIX(-1), TO_FIX( 1), TO_FIX( 1),
		TO_FIX(-1), TO_FIX(-1), TO_FIX( 1), TO_FIX( 1)
	};

	static const int32_t cubeY[] = {
		TO_FIX(-1), TO_FIX( 1), TO_FIX(-1), TO_FIX( 1),
		TO_FIX(-1), TO_FIX( 1), TO_FIX(-1), TO_FIX( 1)
	};

	static const int32_t cubeZ[] = {
		TO_FIX(-1), TO_FIX(-1), TO_FIX(-1), TO_FIX(-1),
		TO_FIX( 1), TO_FIX( 1), TO_FIX( 1), TO_FIX( 1)
	};

    for (i=0; i<8; ++i) {
        // Rotation around Y
        cubeRotX[i] = fix_mul(cubeX[i], COS[t % SIN_SIZE]) + fix_mul(cubeZ[i], SIN[t % SIN_SIZE]);
        cubeRotY[i] = cubeY[i];
        cubeRotZ[i] = -fix_mul(cubeX[i], SIN[t % SIN_SIZE]) + fix_mul(cubeZ[i], COS[t % SIN_SIZE]);
        // Rotation around X
        tempY = fix_mul(cubeRotY[i], COS[t % SIN_SIZE]) - fix_mul(cubeRotZ[i], SIN[t % SIN_SIZE]);
        tempZ = fix_mul(cubeRotY[i], SIN[t % SIN_SIZE]) + fix_mul(cubeRotZ[i], COS[t % SIN_SIZE]);
        cubeRotY[i] = tempY;
        cubeRotZ[i] = tempZ;
        // Translate further away
        cubeRotZ[i] += TO_FIX(4);
        // Projection to screen space
        cubeProjX[i] = fix_div(fix_mul(a, cubeRotX[i]), cubeRotZ[i]);
        cubeProjY[i] = fix_div(fix_mul(a, cubeRotY[i]), cubeRotZ[i]);
        cubeProjX[i] = TO_LONG(fix_mul(cubeProjX[i], scale)) + widthOffset;
        cubeProjY[i] = TO_LONG(fix_mul(cubeProjY[i], scale)) + heightOffset;
    }

	l = 0;
	for (i=0; i<24; i += 2) {
		past[l][p].x1 = cubeProjX[edges[i]];
		past[l][p].y1 = cubeProjY[edges[i]];
		past[l][p].x2 = cubeProjX[edges[i + 1]];
		past[l][p].y2 = cubeProjY[edges[i + 1]];
		bitmapLine(past[l][p].x1, past[l][p].y1, past[l][p].x2, past[l][p].y2);
		l++;
	}
}


int main(int argc, char *argv[]) {
	byte    i;
	int16_t t  = 0;
	byte    p  = 0;

	(void)argc;
	(void)argv;

	textSetCursor(0);  // No cursor.

	// Clear two graphics pages.
	bitmapSetColor(0);
	bitmapSetActive(0);
	bitmapClear();
	bitmapSetActive(1);
	bitmapClear();

	bitmapGetResolution(&widthOffset, &heightOffset);
	widthOffset  = widthOffset >> 1;
	heightOffset = heightOffset >> 1;

	while(1) {
		if (p) {
			p = 0;
			bitmapSetActive(1);
			bitmapSetVisible(0, true);
			bitmapSetVisible(1, false);
		} else {
			p = 1;
			bitmapSetActive(0);
			bitmapSetVisible(0, false);
			bitmapSetVisible(1, true);
		}
		bitmapSetColor(0);
		for (i=0; i<12; i++) bitmapLine(past[i][p].x1, past[i][p].y1, past[i][p].x2, past[i][p].y2);
		bitmapSetColor(255);
		draw_cube(p, t);
		t += 2;
	}

	return 0;
}
