#include "bmp.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int first_check(BMPImage *image1, BMPImage *image2);

int comparer(BMPImage *image1, BMPImage *image2);

int main(int argc, char** argv) {
    if (argc != 3) {
        fprintf(stderr, "Error: Not enough arguments.\n");
        return 1;
    }

    BMPImage image1;
    BMPImage image2;

    BMPImage* image_per1 = &image1;
    BMPImage* image_per2 = &image2;

    if (read_bmp(argv[1], image_per1) != 0) {
        fprintf(stderr, "Error: Unable to read file.\n");
        free_bmp(image_per1);
        return 1;
    }

    if (read_bmp(argv[2], image_per2) != 0) {
        fprintf(stderr, "Error: Unable to read file.\n");
        free_bmp(image_per2);
        return 1;
    }

    int8_t f_c_ret = first_check(image_per1, image_per2);
    if (f_c_ret != 0) {
        free_bmp(image_per1);
        free_bmp(image_per2);
        return f_c_ret;
    }

    int8_t cmp_ret = comparer(image_per1, image_per2);
    if (cmp_ret != 0) {
        free_bmp(image_per1);
        free_bmp(image_per2);
        return cmp_ret;
    }

    free_bmp(image_per1);
    free_bmp(image_per2);
    return 0;
}

int first_check(BMPImage *image1, BMPImage *image2) {
    if (image1 -> infoHeader.bitCount != image2 -> infoHeader.bitCount) {
        fprintf(stderr, "Error: Image bit depth does not match.\n");
        return 1;
    }

    if (image1 -> infoHeader.width != image2 -> infoHeader.width) {
        fprintf(stderr, "Error: resolution across the width of the image lines does not match.\n");
        return 1;
    }

    if (abs(image1 -> infoHeader.height) != abs(image2 -> infoHeader.height)) {
        fprintf(stderr, "Error: resolution of the image rows does not match.\n");
        return 1;
    }

    return 0;
}

int comparer(BMPImage *image1, BMPImage *image2) {
    if (image1 -> infoHeader.bitCount == 24 || image1 -> infoHeader.bitCount == 8) {
        int8_t diff_count = 0;
        size_t width = (size_t) image1 -> infoHeader.width;
        size_t height = (size_t) abs(image1 -> infoHeader.height);

        int row_size = 0;
        if (image1 -> infoHeader.bitCount == 8) {
            row_size = width;
        } else {
            row_size = width * 3;
        }
        int padding = (4 - (row_size % 4)) % 4;
        int stride = row_size + padding;

        uint8_t* data_ptr1 = image1->data;
        uint8_t* data_ptr2 = image2->data;

        int8_t flip_image2_rows = (image1 -> infoHeader.height > 0) == (image2 -> infoHeader.height < 0);
        int8_t flip_image1_rows = (image1 -> infoHeader.height < 0) == (image2 -> infoHeader.height > 0);

        if ((image1 -> infoHeader.height >= 0) == (image2 -> infoHeader.height >= 0) 
            || (image1 -> infoHeader.height <= 0) == (image2 -> infoHeader.height <= 0)) {
                for (size_t y = 0; y < height; y++) {
                    int y2 = flip_image2_rows ? (height - 1 - y) : y;
                    int y1 = flip_image1_rows ? (height - 1 - y) : y;

                    uint8_t* row1_ptr = data_ptr1 + y1 * stride;
                    uint8_t* row2_ptr = data_ptr2 + y2 * stride;

                    CurrBMPPixel* row1 = (CurrBMPPixel*)(row1_ptr);
                    CurrBMPPixel* row2 = (CurrBMPPixel*)(row2_ptr);
                    
                    for (size_t x = 0; x < width; x++) {
                        int8_t xor_blue = 0;
                        int8_t xor_green = 0;
                        int8_t xor_red  = 0;
                        if (image1 -> infoHeader.bitCount == 24) {
                            xor_blue = row1[x].blue ^ row2[x].blue;
                            xor_green = row1[x].green ^ row2[x].green;
                            xor_red = row1[x].red ^ row2[x].red;
                        } else {
                            BMPColor colorTable1 = image1 -> colorTable[row1_ptr[x]];
                            BMPColor colorTable2 = image2 -> colorTable[row2_ptr[x]];

                            xor_blue = colorTable1.blue ^ colorTable2.blue;
                            xor_green = colorTable1.green ^ colorTable2.green;
                            xor_red = colorTable1.red ^ colorTable2.red;
                        } 
                        if ((xor_blue | xor_green | xor_red) != 0) {
                            if (diff_count == 0) {
                                fprintf(stderr, "Next pixels are different:\n");
                            }
                            if ((diff_count++) < 100) {
                                fprintf(stderr, "x%-6d y%-6d\n", (int) x, (int) y);
                            } else if (diff_count >= 100) {
                                return 2;
                            }
                        }
                }
            }
        }
    }
    fprintf(stdout, "Images are same\n");
    return 0;
}