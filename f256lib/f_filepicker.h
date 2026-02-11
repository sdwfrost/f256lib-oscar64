#ifndef FILEPICKER_H
#define FILEPICKER_H
#ifndef WITHOUT_FILEPICKER


#include "f256lib.h"
#include <string.h>
#include <ctype.h>


#define MAX_FILES 200
#define MAX_FILENAME_LEN 120
#define MAX_PATH_LEN 60
#define MAX_FILE_EXTS 4
#define EXT_LEN 3
#define MAX_VISIBLE_FILES 16
#define RESERVED_ENTRY_INDEX 0
#define FPR_BASE 0x30000

#define FPR_tlX              0
#define FPR_tlY              1

#define FPR_currentPath      2
#define FPR_selectedFile     (FPR_currentPath + MAX_PATH_LEN)

#define FPR_fileList         (FPR_selectedFile + MAX_FILENAME_LEN)
#define FPR_fileListEntry(i) (FPR_fileList + ((i) * MAX_FILENAME_LEN))

#define FPR_isDirList        (FPR_fileList + (MAX_FILES * MAX_FILENAME_LEN))
#define FPR_isDir(i)         (FPR_isDirList + (i))

#define FPR_fileCount        (FPR_isDirList + MAX_FILES)
#define FPR_cursorIndex      (FPR_fileCount + 4)
#define FPR_visualIndex      (FPR_cursorIndex + 4)
#define FPR_scrollOffset     (FPR_visualIndex + 4)

#define FPR_fileExts         (FPR_scrollOffset + 4)
#define FPR_fileExt(i)       (FPR_fileExts + ((i) * 3))

#define FPR_TOTAL_SIZE       (FPR_fileExts + 12)

#define FPR_ADDR(off)   (FPR_BASE + (uint32_t)(off))


// Core API
void fpr_set_currentPath(const char *);
void fpr_get_currentPath(char *);
void fpr_get_selectedFile(char *);

uint8_t pickFile_far(void);
uint8_t filePickModal_far(uint8_t, uint8_t,
                          const char *, const char *, const char *, const char *,
                          bool);
void reprepFPR_far(bool);
void initFilePickRecord_far(uint8_t, uint8_t, bool);

void initFPR(const char *defaultPath);
uint8_t getTheFile_far(char *, uint8_t, uint8_t,
                       const char *, const char *, const char *, const char *);
void backUpDirectory_far(void);
void readDirectory_far(void);
bool isExtensionAllowed_far(const char *);

void sortFileList_far(void);
void displayFileList_far(int);
void wipeArea_far(void);

extern char name[];


#pragma compile("f_filepicker.c")


#endif
#endif // FILEPICKER_H
