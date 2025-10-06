#pragma once

#pragma pack(push, 1) // отключение оптимизации выравнивания типов

#include <stdint.h>

typedef struct {
    uint16_t signature;
    uint32_t fileSize;
    uint32_t reserved;
    uint32_t dataOffset;
} BMPHeader;

typedef struct {
    uint32_t size;
    int32_t width;
    int32_t height;
    uint16_t planes;
    uint16_t bitCount;
    uint32_t compression;
    uint32_t imageSize;
    int32_t xPixelsPerM;
    int32_t yPixelsPerM;
    uint32_t colorsUsed;
    uint32_t colorsImportant;
} BMPInfoHeader;

typedef struct {
    uint8_t blue;
    uint8_t green;
    uint8_t red;
    uint8_t reserved;
} BMPColor;

typedef struct {
    uint8_t blue;
    uint8_t green;
    uint8_t red;
} CurrBMPPixel; // для 24 bit

typedef struct {
    BMPHeader header;
    BMPInfoHeader infoHeader;
    BMPColor* colorTable; // для 8 bit
    uint8_t* data; // для 8/24 bit запись пикселей
} BMPImage;

#pragma pack(pop) // возврат настроек 

int read_bmp(const char* filename, BMPImage* image);

int write_bmp(const char* filename, const BMPImage* image);

void free_bmp(BMPImage* image);