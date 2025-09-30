#ifndef SD_H
#define SD_H

#define FILE_LIST_BUFFER_SIZE 32
#define FILE_LIST_NAME_LIMIT 64

struct DirectoryListing {
  int fileCount;
  char fileNames[FILE_LIST_BUFFER_SIZE][FILE_LIST_NAME_LIMIT];
};

int SD_loadFile(const char *filepath, char* buf, size_t size);

int SD_loadFile(const char *directory, const char *filename, char* buf, size_t size);

void SD_format();

void SD_assertDirectory(const char *directory);

DirectoryListing *SD_listFiles(const char *directory, const char *suffix = nullptr);

#endif
