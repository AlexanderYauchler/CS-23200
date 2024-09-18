#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <xmmintrin.h>
#include <windows.h>

#define FILE_NAME "sieveFault.dat"

int main(int argc, const char** argv) {
    uint_fast64_t lowerBound, upperBound;
    HANDLE hFile, hFileMapping;
    char *sieveFault;

    if (argc != 3) {
        fprintf(stderr, "USAGE: %s lowerBound upperBound\n", argv[0]);
        return -1;
    }

    lowerBound = atoi(argv[1]);
    upperBound = atoi(argv[2]);

    if (lowerBound < 1 || upperBound < 1) {
        fprintf(stderr, "ERROR: both the lowerBound (%llu) and the upperBound (%llu) must be positive.\n", lowerBound, upperBound);
        return -2;
    }

    if (lowerBound > upperBound) {
        fprintf(stderr, "ERROR: lowerBound (%llu) must not be higher than upperBound (%llu)\n", lowerBound, upperBound);
        return -3;
    }

    // Create and open the file with size upperBound * 8
    hFile = CreateFile(
        FILE_NAME,
        GENERIC_READ | GENERIC_WRITE,
        0,
        NULL,
        CREATE_ALWAYS,
        FILE_ATTRIBUTE_NORMAL,
        NULL
    );

    if (hFile == INVALID_HANDLE_VALUE) {
        fprintf(stderr, "Failed to create file. Error: %lu\n", GetLastError());
        return -4;
    }

    if (!SetFilePointerEx(hFile, (LARGE_INTEGER){.QuadPart = upperBound * 8}, NULL, FILE_BEGIN)) {
        fprintf(stderr, "Failed to set file size. Error: %lu\n", GetLastError());
        CloseHandle(hFile);
        return -5;
    }

    if (!SetEndOfFile(hFile)) {
        fprintf(stderr, "Failed to set end of file. Error: %lu\n", GetLastError());
        CloseHandle(hFile);
        return -6;
    }

    // Create a file mapping object
    hFileMapping = CreateFileMapping(
        hFile,
        NULL,
        PAGE_READWRITE,
        0,
        upperBound * 8,
        NULL
    );

    if (hFileMapping == NULL) {
        fprintf(stderr, "Failed to create file mapping. Error: %lu\n", GetLastError());
        CloseHandle(hFile);
        return -7;
    }

    // Map the file into memory
    sieveFault = (char*)MapViewOfFile(
        hFileMapping,
        FILE_MAP_ALL_ACCESS,
        0,
        0,
        upperBound * 8
    );

    if (sieveFault == NULL) {
        fprintf(stderr, "Failed to map file. Error: %lu\n", GetLastError());
        CloseHandle(hFileMapping);
        CloseHandle(hFile);
        return -8;
    }

    // Clean up file handles
    CloseHandle(hFileMapping);
    CloseHandle(hFile);

    // Initialize sieveFault array
    memset(sieveFault, 0, upperBound * 8);
    if (upperBound >= 2) {
        sieveFault[0] = 1;
        sieveFault[1] = 1;
    }

    // INTEL FAST SQUIRT
    __m128 value = _mm_set_ss(upperBound);
    __m128 result = _mm_sqrt_ss(value);
    uint_fast32_t fsquirt = _mm_cvtss_si32(result);

    for (uint_fast64_t i = 2; i <= fsquirt; i++) {
        if (sieveFault[i] == 0) {
            uint_fast64_t j = i * 2;
            while (j <= upperBound) {
                sieveFault[j] = 1;
                j += i;
            }
        }
    }

    // Clean up
    UnmapViewOfFile(sieveFault);
    DeleteFile(FILE_NAME);

    return 0;
}
