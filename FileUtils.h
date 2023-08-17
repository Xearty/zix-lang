#pragma once

#include <stdio.h>

namespace FileUtils
{
    static char* ReadFile(const char* filename) {
        FILE* file = fopen(filename, "r");
        fseek(file, 0, SEEK_END);
        size_t filesize = ftell(file);
        rewind(file);

        char* buffer = new char[filesize + 1];
        fread(buffer, sizeof(char), filesize, file);
        buffer[filesize] = '\0';
        return buffer;
    }
}

