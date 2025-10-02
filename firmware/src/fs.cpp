#include "fs.h"

#include <Arduino.h>
#include <SD.h>

DirectoryListing directoryListing;

File file;
File dir;

void FS_init() {
    if (!SD.begin(BUILTIN_SDCARD)) {
        while (true) {
            Serial.println("initialization failed!");
            delay(1000);
        }
    }
}

void FS_assertDirectory(const char *directory) {
    FS_init();

    if (!SD.exists(directory)) {
        SD.mkdir(directory);
    }
}

int FS_loadFile(const char *filepath, char *buf, size_t size) {
    FS_init();

    if (!SD.exists(filepath)) {
        return -1;
    }

    file = SD.open(filepath, FILE_READ);
    if (!file) {
        Serial.println(F("file read failed"));
        return -2;
    }

    return file.read(buf, size);
}

int FS_loadFile(const char *directory, const char *filename, char *buf, size_t size) {
    const int max_length = 128;
    FS_init();

    size_t len = strlen(directory) + strlen(filename) + 1;

    if (len > max_length) {
        return -1;
    }

    char str[max_length];

    str[0] = '\0';

    strcat(str, directory);
    strcat(str, "/");
    strcat(str, filename);

    return FS_loadFile(str, buf, size);
}

DirectoryListing *FS_listFiles(const char *directory) {
    FS_init();

    directoryListing.cnt = 0;
    for (int i = 0; i < FS_LIST_LENGTH; i++) {
        directoryListing.entries[i].fileName[0] = '\0';
        directoryListing.entries[i].size = -1;
    }

    if (!SD.exists(directory)) {
        return &directoryListing;
    }

    dir = SD.open(directory);
    if (!dir) {
        Serial.println("DIR FAILED TO OPEN");
        return &directoryListing;
    }

    if (!dir.isDirectory()) {
        return &directoryListing;
    }

    while ((dir = dir.openNextFile())) {
        const char *name = dir.name();

        DirectoryEntry *entry = &directoryListing.entries[directoryListing.cnt++];

        entry->size = dir.size();
        entry->is_dir = dir.isDirectory();
        strncpy(entry->fileName, name, FS_LENGTH_LIMIT);
        entry->fileName[FS_LENGTH_LIMIT - 1] = '\0';

        dir.close();
    }

    dir.close();

    return &directoryListing;
}