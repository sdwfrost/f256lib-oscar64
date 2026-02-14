/*
 *	Copyright (c) 2024 Scott Duensing, scott@kangaroopunch.com
 *	Adapted for oscar64.
 */


#ifndef GENERAL_H
#define GENERAL_H
#ifndef WITHOUT_GENERAL


#include "f256lib.h"


// Convert ASCII uppercase character to lowercase (non-alpha chars returned as-is)
char generalToLower(char c);

// Safe string length with maximum limit
int16_t generalStrnlen(const char *s, int16_t maxlen);

// Safe string copy (always null-terminates, returns length of src)
int16_t generalStrlcpy(char *dst, const char *src, int16_t maxlen);

// Safe string concatenation (always null-terminates, returns total length attempted)
int16_t generalStrlcat(char *dst, const char *src, int16_t maxlen);

// Case-sensitive string comparison up to maxlen characters
int16_t generalStrncmp(const char *a, const char *b, int16_t maxlen);

// Case-insensitive string comparison up to maxlen characters
int16_t generalStrncasecmp(const char *a, const char *b, int16_t maxlen);

// Extract file extension from filename (returns false if no extension found)
// ext buffer should be at least 5 bytes (4 chars + null)
bool generalExtractFileExtension(const char *filename, char *ext);


#pragma compile("f_general.c")


#endif
#endif // GENERAL_H
