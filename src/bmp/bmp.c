#include <stdio.h>
#include <stdlib.h>
#include "bmp.h"

int read_bmp(const char* filename, BMPImage* image) {
    FILE* file = fopen(filename, "rb");
    size_t sSignature = image -> header.signature;

    if (file == NULL) {
        fprintf(stderr, "Error: The %s file could not be opened.\n", filename);
        return 1;
    }

    if (fread(&image -> header.signature, sSignature, 1, file) != 1) {
        fprintf(stderr, "Error: The file signature could not be read.\n");
        fclose(file);
        return 1;
    }

    if (image -> header.signature != 0x4D42) {
        fprintf(stderr, "Error: The signature %PRIu16 does not match 0x4D42.\n", image ->header.signature);
        fclose(file);
        return 1;
    }

    if (fread(&image -> header.fileSize, sizeof(BMPHeader) - sSignature, 1, file) != 1) {
        fprintf(stderr, "Error: The BMP header could not be read.\n");
        fclose(file);
        return 1;
    }

    if (fread(&image -> infoHeader.size, sizeof(BMPInfoHeader), 1, file) != 1) {
        fprintf(stderr, "Error: The BMP information header could not be read.\n");
        fclose(file);
        return 1;
    }

    if (image -> infoHeader.size != 40) {
        fprintf(stderr, "Error: Unsupported version of the DIB header (requires a 40-byte BITMAPINFOHEADER).\n");
        fclose(file);
        return 1;
    }

    if (image -> infoHeader.bitCount != 8 && image -> infoHeader.bitCount != 24) {
        fprintf(stderr, "Error: Only 8-bit and 24-bit images are supported.");
        fclose(file);
        return 1;
    }

    if (image -> infoHeader.compression != 0) {
        fprintf(stderr, "Error: Compressed BMP files are not supported.\n");
        fclose(file);
        return 1;
    }
}