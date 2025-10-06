#include <stdio.h>
#include <stdlib.h>
#include "bmp.h"

void free_bmp(BMPImage* image);

int read_bmp(const char* filename, BMPImage* image) {
    FILE* file = fopen(filename, "rb");
    size_t sSignature = sizeof(image -> header.signature);

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

    image->colorTable = NULL;
    image->data = NULL;

    /* Выравнивание для обоих случаев битности */
    int stride = 0;
    if (image -> infoHeader.imageSize == 0) {
        int row_size = (image -> infoHeader.width) * ((image -> infoHeader.bitCount) / 8); // чтение строки
        int row_padding = (4 - (row_size % 4)) % 4;
        stride = row_padding + row_size;
        image -> infoHeader.imageSize = stride * abs(image -> infoHeader.height); // выравнивание байтов 
    } else {
        int row_size = image -> infoHeader.width * (image -> infoHeader.bitCount / 8);
        int padding = (4 - (row_size % 4)) % 4;
        stride = row_size + padding;
    }
    
    /* Для 8 бит чтение */
    if (image -> infoHeader.bitCount == 8) {
        image -> colorTable = malloc(sizeof(BMPColor) * 256);
        if (image -> colorTable == NULL) {
            fprintf(stderr, "Ошибка чтения палитры");
            fclose(file);
            return 1;
        }

        if (fread(image -> colorTable, sizeof(BMPColor), 256, file) != 256) {
            fprintf(stderr, "Несоответсвие базовой палитре размером 256");
            free(image -> colorTable);
            return 1;
        }
    } else if (image -> infoHeader.bitCount != 24) {
        fprintf(stderr, "Ошибка: Неподдерживаемый формат битности BMP");
        fclose(file);
    }

    image -> data = malloc(image -> infoHeader.imageSize);
    if ((image -> data) == NULL) {
        fprintf(stderr, "Ошибка выделения памяти для массива пикселей");
        free_bmp(image);
        fclose(file);
        return 1;
    }

    if (fseek(file, image -> header.dataOffset, SEEK_SET) != 0) { // установка указателя с учётом начала чтения offset
        fprintf(stderr, "Ошибка смены указателя в файле для начала чтения");
        free_bmp(image);
        fclose(file);
        return 1;
    }

    if (fread(image -> data, image -> infoHeader.imageSize, 1, file) != 1) {
        fprintf(stderr, "Ошибка записи данных пикселей");
        free_bmp(image);
        fclose(file);
        return 1;
    }

    fclose(file);
    return 0;
}

int write_bmp(const char* filename, const BMPImage* image) {
    FILE* file = fopen(filename, "wb");
    if (file == NULL) {
        fprintf(stderr, "Ошибка открытия файла");
        fclose(file);
        return 1;
    }

    if (fwrite(&image -> header, sizeof(BMPHeader), 1, file) != 1) {
        fprintf(stderr, "Ошибка записи заголовка");
        fclose(file);
        return 1;
    }

    if (fwrite(&image -> infoHeader, sizeof(BMPInfoHeader), 1, file) != 1) {
        fprintf(stderr, "Ошибка записи информационного заголовка");
        fclose(file);
        return 1;
    }

    if (image -> infoHeader.bitCount == 8 && image -> colorTable != NULL) {
        if (fwrite(image -> colorTable, sizeof(BMPColor), 256, file) != 256) {
            fprintf(stderr, "Ошибка записи палитры в файл");
            fclose(file);
            return 1;
        }
    }

    if (fwrite(image -> data, image -> infoHeader.imageSize, 1, file) != 1) {
        fprintf(stderr, "Оошибка записи данных пикселей");
        fclose(file);
        return 1;
    }

    fclose(file);
    return 0;
}

void free_bmp(BMPImage* image) {
    if (image != NULL) {
            free(image -> data);
            free(image -> colorTable);
            image->colorTable = NULL;
            image->data = NULL;
    }
}