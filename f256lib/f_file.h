/*
 *	Copyright (c) 2024 Scott Duensing, scott@kangaroopunch.com
 *	Adapted for oscar64.
 */


#ifndef FILE_H
#define FILE_H
#ifndef WITHOUT_FILE


#include "f256lib.h"


typedef struct fileDirEntS {
	unsigned char  d_blocks;
	unsigned char  d_type;
	char           d_name[256];
} fileDirEntT;


int8_t       fileClose(uint8_t *fd);
int8_t       fileCloseDir(char *dir);
int8_t       fileMakeDir(const char *dir);
uint8_t     *fileOpen(const char *fname, const char *mode);
char        *fileOpenDir(const char *name);
int16_t      fileRead(void *buf, uint16_t nbytes, uint16_t nmemb, uint8_t *fd);
fileDirEntT *fileReadDir(char *dir);
int8_t       fileRemoveDir(const char *dir);
int8_t       fileRename(const char *name, const char *to);
void         fileReset(void);
int8_t       fileSeek(uint8_t *fd, uint32_t offset, uint8_t whence);
int8_t       fileUnlink(const char *name);
int16_t      fileWrite(void *buf, uint16_t nbytes, uint16_t nmemb, uint8_t *fd);


#define _DE_ISREG(t)  (t == 0)
#define _DE_ISDIR(t)  (t == 1)
#define _DE_ISLBL(t)  (t == 2)
#define _DE_ISLNK(t)  (0)


// Aliases to the standard names if they don't exist.
#ifndef DIR
#define closedir   fileCloseDir
#define DIR        char
#define dirent     fileDirEntS
#ifndef FILE
#define FILE       uint8_t
#endif
#define fclose     fileClose
#define fopen      fileOpen
#define fread      fileRead
#define fseek      fileSeek
#define fwrite     fileWrite
#define mkdir(d,m) fileMakeDir(d)
#define opendir    fileOpenDir
#define readdir    fileReadDir
#define rename     fileRename
#define rewind(s)  (void)fileSeek(s, 0, SEEK_SET)
#define rmdir      fileRemoveDir
#ifndef SEEK_SET
#define SEEK_SET   0
#endif
#define STDIN      0
#define STDOUT     1
#define unlink     fileUnlink
#endif


#pragma compile("f_file.c")


#endif
#endif // FILE_H
