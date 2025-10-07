#include "bmp.h"
#include <stdio.h>
#include <string.h>

int main(int argc, char** argv) {
    if (argc != 4) {
        fprintf(stderr, "Error: Not enough arguments.\n");
        return 1;
    }

    char* command = argv[1];

    if (strcmp(command, "comparer") != 0) {
        fprintf(stderr, "Error: Incorrect utility argument.\n");
        return 1;
    }

    BMPImage image1;
    BMPImage image2;

    BMPImage* image_per1 = &image1;
    BMPImage* image_per2 = &image2;

    if (read_bmp(argv[2], image_per1) != 0) {
        fprintf(stderr, "Error: Unable to read file.\n");
        free_bmp(image_per1);
        return 1;
    }

    if (read_bmp(argv[3], image_per2) != 0) {
        fprintf(stderr, "Error: Unable to read file.\n");
        free_bmp(image_per2);
        return 1;
    }
}

