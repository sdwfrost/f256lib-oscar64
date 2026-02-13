#ifndef RGBIMAGE_H
#define RGBIMAGE_H

void rgbimg_begin(void);

void rgbimg_end(void);

struct RGBA
{
	char	r, g, b, a;
};

extern RGBA	rgbblock[8][4];
extern RGBA	rgbpalette[4];

void rgbimg_putblock(char x, char y);

char rgbimg_mapcolor(RGBA c);

void rgbimg_buildpal(void);

void rgbimg_noisergb(RGBA * rgba);

void rgbimg_noiseblock(void);

void rgbimg_mapblock(void);


#pragma compile("rgbimage.c")

#endif
