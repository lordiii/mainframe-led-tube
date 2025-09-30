#include "fs.h"

#include <Arduino.h>
#include <SdFat.h>

SdFat32 sd;
File32 file;
File32 dir;

#define SD_CONFIG SdioConfig(FIFO_SDIO)
#define SPI_CONFIG SdSpiConfig(TeensySdioCard, DEDICATED_SPI, SPI_FULL_SPEED, &SPI2)

DirectoryListing directoryListing;

void SD_init() {
    if (!sd.begin(SD_CONFIG)) {
        sd.initErrorHalt();
    }
}

void SD_format() {
    sd.format(&Serial);
}

void SD_assertDirectory(const char *directory) {
    if (!dir.exists(directory)) {
        sd.mkdir(directory);
    }
}

int SD_loadFile(const char *filepath, char* buf, size_t size) {
    if (!file.open(filepath)) {
        // TODO: LOGGING
        return false;
    }

    return file.read(buf, size);
}

int SD_loadFile(const char *directory, const char *filename, char* buf, size_t size) {
    size_t len = strlen(directory) + strlen(filename) + 1;
    char str[len];
    str[0] = '\0';

    strcat(str, directory);
    strcat(str, "/");
    strcat(str, filename);

    return SD_loadFile(str, buf, size);
}

DirectoryListing *SD_listFiles(const char *directory, const char *suffix) {
    if (!dir.open(directory)) {
        // TODO: LOGGING
        return nullptr;
    }

    directoryListing.fileCount = 0;
    for (int i = 0; i < FILE_LIST_BUFFER_SIZE; i++) {
        directoryListing.fileNames[i][0] = '\0';
    }

    while (file.openNext(&dir)) {
        char *filename = file.name();
        if (file.isHidden() || file.isDir() || strlen(filename) > FILE_LIST_NAME_LIMIT || (suffix != nullptr && strstr(filename, suffix) == nullptr)) continue;

        strcpy(directoryListing.fileNames[directoryListing.fileCount], filename);
        file.close();
    }

    dir.close();

    return &directoryListing;
}