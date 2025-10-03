#pragma once

#pragma pack(push, 1) // отключение оптимизации выравнивания типов

#include <stdint.h>

typedef struct {
    uint16_t Signature;
    uint32_t FileSize;
    uint32_t Reserved;
    uint32_t DataOffset;
} BMPHeader;

typedef struct {
    uint32_t Size;
    int32_t Width;
    int32_t Height;
    uint16_t Planes;
    uint16_t BitCount;
    uint32_t Compression;
    uint32_t ImageSize;
    int32_t XpixelsPerM;
    int32_t YpixelsPerM;
    uint32_t ColorsUsed;
    uint32_t ColorsImportant;
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
} СurrBMPPixel; // для 24 bit

typedef strcut {
    BMPHeader header;
    BMPInfoHeader infoHeader;
    BMPColor* colorTable; // обработка случая с 24 bit изображением
    uint8_t* data; // для 8 bit
} BMPImage;

#pragma pack(pop); // возврат настроек 
