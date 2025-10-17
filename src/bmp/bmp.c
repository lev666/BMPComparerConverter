#include "bmp/bmp.h"
#include <stdio.h>
#include <stdlib.h>

int read_bmp(const char *filename, BMPImage *image) {
  FILE *file = fopen(filename, "rb");
  size_t sSignature = sizeof(image->header.signature);

  if (file == NULL) {
    fprintf(stderr, "Error: The %s file could not be opened.\n", filename);
    return 1;
  }

  if (fread(&image->header.signature, sSignature, 1, file) != 1) {
    fprintf(stderr, "Error: The file signature could not be read.\n");
    fclose(file);
    return 1;
  }

  if (image->header.signature != 0x4D42) {
    fprintf(stderr, "Error: The signature %d does not match 0x4D42.\n",
            image->header.signature);
    fclose(file);
    return 1;
  }

  if (fread(&image->header.fileSize, sizeof(BMPHeader) - sSignature, 1, file) !=
      1) {
    fprintf(stderr, "Error: The BMP header could not be read.\n");
    fclose(file);
    return 1;
  }

  if (fread(&image->infoHeader.size, sizeof(BMPInfoHeader), 1, file) != 1) {
    fprintf(stderr, "Error: The BMP information header could not be read.\n");
    fclose(file);
    return 1;
  }

  if (image->infoHeader.size != 40) {
    fprintf(stderr, "Error: Unsupported version of the DIB header (requires a "
                    "40-byte BITMAPINFOHEADER).\n");
    fclose(file);
    return 1;
  }

  if (image->infoHeader.bitCount != 8 && image->infoHeader.bitCount != 24) {
    fprintf(stderr, "Error: Only 8-bit and 24-bit images are supported.\n");
    fclose(file);
    return 1;
  }

  if (image->infoHeader.compression != 0) {
    fprintf(stderr, "Error: Compressed BMP files are not supported.\n");
    fclose(file);
    return 1;
  }

  /* Выравнивание для обоих случаев битности */
  int row_size = (image->infoHeader.width) *
                 ((image->infoHeader.bitCount) / 8); // чтение строки
  int row_padding = (4 - (row_size % 4)) % 4;
  int stride = row_padding + row_size;
  if (image->infoHeader.imageSize == 0) {
    image->infoHeader.imageSize =
        stride * abs(image->infoHeader.height); // выравнивание байтов
  }

  if (image->infoHeader.imageSize > image->header.fileSize) {
    fprintf(stderr, "Error: Image size bigger than file size.\n");
    fclose(file);
    return 1;
  }

  /* Для 8 бит чтение */
  if (image->infoHeader.bitCount == 8) {
    image->colorTable = malloc(sizeof(BMPColor) * 256);
    if (image->colorTable == NULL) {
      fprintf(stderr, "Error: Cannot read palette.\n");
      fclose(file);
      return 1;
    }

    if (fread(image->colorTable, sizeof(BMPColor), 256, file) != 256) {
      fprintf(stderr, "Error: Base palette size mismatch 256.\n");
      fclose(file);
      return 1;
    }
  } else if (image->infoHeader.bitCount != 24) {
    fprintf(stderr, "Error: Unsupported BMP bit depth format.\n");
    fclose(file);
  }

  image->data = malloc(image->infoHeader.imageSize);
  if ((image->data) == NULL) {
    fprintf(stderr, "Error: memory allocation for pixel array failed.\n");
    fclose(file);
    return 1;
  }

  if (fseek(file, image->header.dataOffset, SEEK_SET) !=
      0) { // установка указателя с учётом начала чтения offset
    fprintf(stderr,
            "Error: Failed to change pointer in file to start reading.\n");
    fclose(file);
    return 1;
  }

  if (fread(image->data, image->infoHeader.imageSize, 1, file) != 1) {
    fprintf(stderr, "Error: Failed to write pixel data.\n");
    fclose(file);
    return 1;
  }

  fclose(file);
  return 0;
}

int write_bmp(const char *filename, const BMPImage *image) {
  FILE *file = fopen(filename, "wb");
  if (file == NULL) {
    fprintf(stderr, "Error: Could not open file.\n");
    fclose(file);
    return 1;
  }

  if (fwrite(&image->header, sizeof(BMPHeader), 1, file) != 1) {
    fprintf(stderr, "Error: Failed to write header.\n");
    fclose(file);
    return 1;
  }

  if (fwrite(&image->infoHeader, sizeof(BMPInfoHeader), 1, file) != 1) {
    fprintf(stderr, "Error: Failed to write information header.\n");
    fclose(file);
    return 1;
  }

  if (image->infoHeader.bitCount == 8 && image->colorTable != NULL) {
    if (fwrite(image->colorTable, sizeof(BMPColor), 256, file) != 256) {
      fprintf(stderr, "Error: Failed to save palette to file.\n");
      fclose(file);
      return 1;
    }
  }

  if (fseek(file, image->header.dataOffset, SEEK_SET) != 0) {
    fprintf(stderr,
            "Error: Failed to change pointer in file to start reading.\n");
    fclose(file);
    return 1;
  }

  if (fwrite(image->data, image->infoHeader.imageSize, 1, file) != 1) {
    fprintf(stderr, "Error: Failed to write pixel data.\n");
    fclose(file);
    return 1;
  }

  fclose(file);
  return 0;
}

void free_bmp(BMPImage *image) {
  if (image != NULL) {
    if (image->data != NULL) {
      free(image->data);
      image->data = NULL;
    }
    if (image->colorTable != NULL) {
      free(image->colorTable);
      image->colorTable = NULL;
    }
  }
}
