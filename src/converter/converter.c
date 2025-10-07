#include "bmp.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int invert_bmp(BMPImage* image);

int main(int argc, char** argv) {
    if (argc != 4) {
        fprintf(stderr, "Error: Not enough arguments.\n");
        return 1;
    }

    char* command = argv[1];

    if (strcmp(command, "converter") != 0) {
        fprintf(stderr, "Error: Incorrect utility argument.\n");
        return 1;
    }

    BMPImage image;

    if (read_bmp(argv[2], &image) != 0) {
        fprintf(stderr, "Error: Unable to read file.\n");
        free_bmp(&image);
        return 1;
    }

    if (invert_bmp(&image) != 0) {
        fprintf(stderr, "Error: Failed to invert the image.\n");
        free_bmp(&image);
        return 1;
    }

    if (write_bmp(argv[3], &image) != 0 ) {
        fprintf(stderr, "Error: Failed to write new data.\n");
        free_bmp(&image);
        return 1;
    }
}

int invert_bmp(BMPImage* image) {
    int8_t rev = 0xFF; // 11111111
    if (image -> infoHeader.bitCount == 8) {
        for (size_t i = 0; i < 256; i++) {
            image -> colorTable[i].blue = (image -> colorTable[i].blue) ^ rev;
            image -> colorTable[i].green = (image -> colorTable[i].green) ^ rev;
            image -> colorTable[i].red = (image -> colorTable[i].red) ^ rev;
        }
    } else if (image -> infoHeader.bitCount == 24) {
        int width = image -> infoHeader.width;
        int height = abs(image -> infoHeader.height);

        int row_size = width * 3;
        int padding = (4 - (row_size % 4)) % 4;
        int stride = row_size + padding; // выравнивание

        uint8_t* data_ptr = image->data;
        for (size_t y = 0; y < height; y++) {
            CurrBMPPixel* row = (CurrBMPPixel*)(data_ptr + y * stride);
            for (size_t x = 0; x < width; x++) {
                row[x].blue = row[x].blue ^ rev;
                row[x].green = row[x].green ^ rev;
                row[x].red = row[x].red ^ rev;
            }
        }
    } else {
        fprintf(stderr, "Error: Unsupported BMP bit depth format.\n");
        return 1;
    }

    return 0;
}