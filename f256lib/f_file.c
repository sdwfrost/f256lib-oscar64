/*
 *	Copyright (c) 2024 Scott Duensing, scott@kangaroopunch.com
 *	Adapted for oscar64.
 */


#ifndef WITHOUT_FILE


#include <string.h>
#include "f256lib.h"

// Undefine macros from f_file.h that conflict with struct member names
#undef close
#undef mkdir
#undef rename
#undef rmdir
#undef fclose
#undef fopen
#undef fread
#undef fwrite
#undef fseek

#define MAX_DRIVES 8


static char _dirStream[MAX_DRIVES];


static bool        findName(const char *name, int16_t *offset);
static int16_t     kernelRead(uint8_t fd, void *buf, uint16_t nbytes);
static int16_t     kernelWrite(uint8_t fd, void *buf, uint16_t nbytes);
static const char *pathWithoutDrive(const char *path, byte *drive);


int8_t fileClose(uint8_t *fd) {
	kernelArgs->u.file.close.stream = *fd;
	kernelCall(File.Close);

	free(fd);

	for (;;) {
		kernelNextEvent();
		if (kernelEventData.type == kernelEvent(file.CLOSED)
		 || kernelEventData.type == kernelEvent(file.ERROR)) {
			return -1;
		}
	}

	return 0;
}


int8_t fileCloseDir(char *dir) {
	if (!dir) return -1;

	for (;;) {
		if (*dir) {
			kernelArgs->u.directory.close.stream = *dir;
			kernelCall(Directory.Close);
			if (!kernelError) {
				*dir = 0;
			}
		}
		kernelNextEvent();
		if (kernelEventData.type == kernelEvent(directory.CLOSED)) {
			dir = NULL;
			return 0;
		}
	}
}


int8_t fileMakeDir(const char *dir) {
	byte  drive;

	dir = pathWithoutDrive(dir, &drive);

	kernelArgs->u.directory.mkdir.drive = drive;
	kernelArgs->u.common.buf = dir;
	kernelArgs->u.common.buflen = strlen(dir);
	kernelCall(Directory.MkDir);
	if (kernelError) return -1;

	for (;;) {
		kernelNextEvent();

		if (kernelEventData.type == kernelEvent(directory.CREATED)) break;
		if (kernelEventData.type == kernelEvent(directory.ERROR))   return -1;
	}

	return 0;
}


uint8_t *fileOpen(const char *fname, const char *mode) {
	uint8_t     ret = 0;
	uint8_t     m   = 0;
	uint8_t    *fd;
	byte        drive;
	const char *c;

	fname = pathWithoutDrive(fname, &drive);

	c = mode;
	while (*c != 0) {
		if (*c == 'w') m = 1;
		if (*c == 'a') m = 2;
		c++;
	}

	kernelArgs->u.common.buf = (const uint8_t *)fname;
	kernelArgs->u.common.buflen = strlen(fname);
	kernelArgs->u.file.open.drive = drive;
	kernelArgs->u.file.open.mode = m;
	ret = kernelCall(File.Open);
	if (kernelError) return NULL;

	for (;;) {
		kernelNextEvent();
		if (kernelEventData.type == kernelEvent(file.OPENED)) {
			fd = (uint8_t *)malloc(sizeof(uint8_t));
			*fd = ret;
			return fd;
		}
		if (kernelEventData.type == kernelEvent(file.NOT_FOUND)
		 || kernelEventData.type == kernelEvent(file.ERROR)) {
			return NULL;
		}
	}
}


char *fileOpenDir(const char *name) {
	byte  drive;
	char  stream;
	char *dir;

	name = pathWithoutDrive(name, &drive);

	if (_dirStream[drive]) return NULL;

	kernelArgs->u.directory.open.drive = drive;
	kernelArgs->u.common.buf = name;
	kernelArgs->u.common.buflen = strlen(name);
	stream = kernelCall(Directory.Open);
	if (kernelError) return NULL;

	for (;;) {
		kernelNextEvent();

		if (kernelEventData.type == kernelEvent(directory.OPENED)) break;
		if (kernelEventData.type == kernelEvent(directory.ERROR))  return NULL;
	}

	_dirStream[drive] = stream;
	dir = &_dirStream[drive];

	return dir;
}


int16_t fileRead(void *buf, uint16_t nbytes, uint16_t nmemb, uint8_t *fd) {
	char    *data     = (char *)buf;
	int16_t  read     = 0;
	uint16_t bytes    = nbytes * nmemb;
	int16_t  returned;

	while (read < bytes) {
		returned = kernelRead(*fd, data + read, bytes - read);
		if (returned < 0) return -1;
		if (returned == 0) break;
		read += returned;
	}

	return read / nbytes;
}


// Undefine EOF and FILE to allow struct member access in this function
#undef EOF
#undef FILE
fileDirEntT *fileReadDir(char *dir) {
	static fileDirEntT dirent;
	uint16_t len;

	if (!dir) return NULL;

	kernelArgs->u.directory.read.stream = *dir;
	kernelCall(Directory.Read);
	if (kernelError) return NULL;

	for (;;) {
		kernelNextEvent();

		if (kernelEventData.type == kernelEvent(directory.VOLUME)) {
			dirent.d_blocks = 0;
			dirent.d_type = 2;
		} else if (kernelEventData.type == kernelEvent(directory.FILE)) {
			kernelArgs->u.common.buf = &dirent.d_blocks;
			kernelArgs->u.common.buflen = sizeof(dirent.d_blocks);
			kernelCall(ReadExt);
			dirent.d_type = (dirent.d_blocks == 0);
		} else if (kernelEventData.type == kernelEvent(directory.FREE)) {
			kernelArgs->u.directory.read.stream = *dir;
			kernelCall(Directory.Read);
			if (!kernelError) continue;
			return NULL;  // fall through to EOF/ERROR behavior
		} else if (kernelEventData.type == kernelEvent(directory.EOF)
		        || kernelEventData.type == kernelEvent(directory.ERROR)) {
			return NULL;
		} else {
			continue;
		}

		// Copy the name.
		len = kernelEventData.u.directory.u.file.len;
		if (len >= sizeof(dirent.d_name)) {
			len = sizeof(dirent.d_name) - 1;
		}

		if (len > 0) {
			kernelArgs->u.common.buf = &dirent.d_name;
			kernelArgs->u.common.buflen = len;
			kernelCall(ReadData);
		}
		dirent.d_name[len] = 0;

		return &dirent;
	}
}
// Restore EOF
#define EOF (-1)


int8_t fileRemoveDir(const char *dir) {
	byte  drive;

	dir = pathWithoutDrive(dir, &drive);

	kernelArgs->u.directory.mkdir.drive = drive;
	kernelArgs->u.common.buf = dir;
	kernelArgs->u.common.buflen = strlen(dir);
	kernelCall(Directory.RmDir);
	if (kernelError) return -1;

	for (;;) {
		kernelNextEvent();

		if (kernelEventData.type == kernelEvent(directory.DELETED)) break;
		if (kernelEventData.type == kernelEvent(directory.ERROR))   return -1;
	}

	return 0;
}


int8_t fileRename(const char *name, const char *to) {
	byte    drive;
	byte    drive2;
	int16_t path1;
	int16_t path2;

	name = pathWithoutDrive(name, &drive);
	to = pathWithoutDrive(to, &drive2);

	if (false
		|| (drive != drive2)
		|| !findName(name, &path1)
		|| !findName(to, &path2)
		|| (path1 != path2)
		|| (strncmp(name, to, path1) != 0)
		) {
		return -1;
	}

	to += path2;

	kernelArgs->u.file.del.drive = drive;
	kernelArgs->u.common.buf = name;
	kernelArgs->u.common.buflen = strlen(name);
	kernelArgs->u.common.ext = to;
	kernelArgs->u.common.extlen = strlen(to);
	kernelCall(File.Rename);
	if (kernelError) return -1;

	for (;;) {
		kernelNextEvent();
		if (kernelEventData.type == kernelEvent(file.RENAMED)) break;
		if (kernelEventData.type == kernelEvent(file.ERROR))   return -1;
	}

	return 0;
}


void fileReset(void) {
	byte x;

	for (x=0; x<MAX_DRIVES; x++) _dirStream[x] = 0;
}


int8_t fileSeek(uint8_t *fd, uint32_t offset, uint8_t whence) {
	if (whence != 0) return -1;  // SEEK_SET only

	kernelArgs->u.file.seek.stream = *fd;
	kernelArgs->u.file.seek.offset = offset;
	kernelCall(File.Seek);
	if (kernelError) return -1;

	return 0;
}


int8_t fileUnlink(const char *name) {
	byte drive;

	name = pathWithoutDrive(name, &drive);
	kernelArgs->u.file.del.drive = drive;
	kernelArgs->u.common.buf = name;
	kernelArgs->u.common.buflen = strlen(name);
	kernelCall(File.Delete);
	if (kernelError) return -1;

	for (;;) {
		kernelNextEvent();
		if (kernelEventData.type == kernelEvent(file.DELETED)) break;
		if (kernelEventData.type == kernelEvent(file.ERROR))   return -1;
	}

	return 0;
}


int16_t fileWrite(void *buf, uint16_t nbytes, uint16_t nmemb, uint8_t *fd) {
	uint8_t *data  = (uint8_t *)buf;
	int16_t  total = 0;
	int16_t  bytes = nbytes * nmemb;
	uint8_t  writing;
	int16_t  written;

	while (bytes) {

		if (bytes > 254) {
			writing = 254;
		} else {
			writing = bytes;
		}

		written = kernelWrite(*fd, data + total, writing);
		if (written <= 0) {
			return -1;
		}

		total += written;
		bytes -= written;
	}

	return total / nbytes;
}


static bool findName(const char *name, int16_t *offset) {
	int16_t i;
	int16_t pos;

	for (i = pos = 0; name[i]; i++) {
		if (name[i] == '/') {
			pos = i+1;
			if (!name[pos]) {
				return false;
			}
		}
	}

	*offset = pos;
	return true;
}


// Undefine EOF for struct member access
#undef EOF
static int16_t kernelRead(uint8_t fd, void *buf, uint16_t nbytes) {

	// STDIN
	if (fd == 0) {
		*(char *)buf = f256getchar();
		return 1;
	}

	if (nbytes > 256) nbytes = 256;

	kernelArgs->u.file.read.stream = fd;
	kernelArgs->u.file.read.buflen = nbytes;
	kernelCall(File.Read);
	if (kernelError) return -1;

	for(;;) {
		kernelNextEvent();
		if (kernelEventData.type == kernelEvent(file.DATA)) {
			kernelArgs->u.common.buf = buf;
			kernelArgs->u.common.buflen = kernelEventData.u.file.u.data.delivered;
			kernelCall(ReadData);
			if (!kernelEventData.u.file.u.data.delivered) return 256;
			return kernelEventData.u.file.u.data.delivered;
		}
		if (kernelEventData.type == kernelEvent(file.EOF)) return 0;
		if (kernelEventData.type == kernelEvent(file.ERROR)) return -1;
	}
}
#define EOF (-1)


static int16_t kernelWrite(uint8_t fd, void *buf, uint16_t nbytes) {
	int16_t  i;
	char    *text;

	// STDOUT
	if (fd == 1) {
		text = (char *)buf;
		for (i = 0; i < nbytes; i++) {
			f256putchar(text[i]);
		}
		return i;
	}

	kernelArgs->u.file.read.stream = fd;
	kernelArgs->u.common.buf = buf;
	kernelArgs->u.common.buflen = nbytes;
	kernelCall(File.Write);
	if (kernelError) return -1;

	for (;;) {
		kernelNextEvent();
		if (kernelEventData.type == kernelEvent(file.WROTE)) return kernelEventData.u.file.u.data.delivered;
		if (kernelEventData.type == kernelEvent(file.ERROR)) return -1;
	}
}


static const char *pathWithoutDrive(const char *path, byte *drive) {
	*drive = 0;

	if (strlen(path) < 2) return path;
	if (path[1] != ':')   return path;
	if ((*path >= '0') && (*path <= '7')) *drive = *path - '0';

	return (path + 2);
}


#endif
