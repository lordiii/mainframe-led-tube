#ifndef SD_H
#define SD_H

#include <Arduino.h>

#define FS_LIST_LENGTH 32
#define FS_LENGTH_LIMIT 64

struct DirectoryEntry {
    size_t size;
    bool is_dir;
    char fileName[FS_LENGTH_LIMIT];
};

struct DirectoryListing {
    int cnt;
    DirectoryEntry entries[FS_LIST_LENGTH];
};

void FS_init();

int FS_loadFile(const char *directory, const char *filename, char *buf, size_t size);

int FS_loadFile(const char *filepath, char *buf, size_t size);

void FS_assertDirectory(const char *directory);

DirectoryListing *FS_listFiles(const char *directory);

#endif
