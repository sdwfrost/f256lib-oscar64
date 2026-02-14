/*
 *	Copyright (c) 2024 Scott Duensing, scott@kangaroopunch.com
 *	Adapted for oscar64.
 */


#ifndef WITHOUT_GENERAL


#include "f256lib.h"
#include <string.h>


char generalToLower(char c) {
	if (c >= 'A' && c <= 'Z') {
		return c + ('a' - 'A');
	}
	return c;
}


int16_t generalStrnlen(const char *s, int16_t maxlen) {
	int16_t len = 0;
	while (len < maxlen && s[len] != 0) {
		len++;
	}
	return len;
}


int16_t generalStrlcpy(char *dst, const char *src, int16_t maxlen) {
	int16_t src_len = 0;
	int16_t i;

	// Count source length
	while (src[src_len] != 0) src_len++;

	if (maxlen > 0) {
		int16_t copy_len = (src_len < maxlen - 1) ? src_len : maxlen - 1;
		for (i = 0; i < copy_len; i++) {
			dst[i] = src[i];
		}
		dst[copy_len] = 0;
	}

	return src_len;
}


int16_t generalStrlcat(char *dst, const char *src, int16_t maxlen) {
	int16_t dst_len = generalStrnlen(dst, maxlen);
	int16_t src_len = 0;
	int16_t i;

	while (src[src_len] != 0) src_len++;

	if (dst_len < maxlen - 1) {
		int16_t space = maxlen - dst_len - 1;
		int16_t copy_len = (src_len < space) ? src_len : space;
		for (i = 0; i < copy_len; i++) {
			dst[dst_len + i] = src[i];
		}
		dst[dst_len + copy_len] = 0;
	}

	return dst_len + src_len;
}


int16_t generalStrncmp(const char *a, const char *b, int16_t maxlen) {
	int16_t i;
	for (i = 0; i < maxlen; i++) {
		if (a[i] != b[i]) {
			return (int16_t)(unsigned char)a[i] - (int16_t)(unsigned char)b[i];
		}
		if (a[i] == 0) break;
	}
	return 0;
}


int16_t generalStrncasecmp(const char *a, const char *b, int16_t maxlen) {
	int16_t i;
	for (i = 0; i < maxlen; i++) {
		char ca = generalToLower(a[i]);
		char cb = generalToLower(b[i]);
		if (ca != cb) {
			return (int16_t)(unsigned char)ca - (int16_t)(unsigned char)cb;
		}
		if (a[i] == 0) break;
	}
	return 0;
}


bool generalExtractFileExtension(const char *filename, char *ext) {
	int16_t len = 0;
	int16_t dot_pos = -1;
	int16_t i;

	while (filename[len] != 0) {
		if (filename[len] == '.') {
			dot_pos = len;
		}
		len++;
	}

	if (dot_pos < 0 || dot_pos == len - 1) {
		ext[0] = 0;
		return false;
	}

	// Copy extension (after dot), max 4 chars
	{
		int16_t ext_start = dot_pos + 1;
		int16_t ext_len   = len - ext_start;
		if (ext_len > 4) ext_len = 4;

		for (i = 0; i < ext_len; i++) {
			ext[i] = generalToLower(filename[ext_start + i]);
		}
		ext[ext_len] = 0;
	}

	return true;
}


#endif
