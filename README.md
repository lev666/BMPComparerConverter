
# BMPComparerConverter
[![MIT License](https://img.shields.io/badge/License-MIT-green.svg)](https://github.com/lev666/BMPComparerConverter/blob/main/LICENSE)
[![CodeFactor](https://www.codefactor.io/repository/github/lev666/bmpcomparerconverter/badge)](https://www.codefactor.io/repository/github/lev666/bmpcomparerconverter)

A command-line utility to invert and compare raster images in the BMP format.

## Features

- Invert the colors of 24-bit and 8-bit BMP images.

- Perform a pixel-by-pixel comparison of two BMP files with the same resolution and color depth.


## Prerequisites
* **CMake** >= 3.7

* A **C99 compatible compiler:** (e.g., GCC, Clang, MSVC)

* **Python:** 3.13 (Python is required for the test system to function. The tests were written using version py 3.13.7)

* **AddressSanitizer:** The AddressSanitizer library must be pre-installed.
## Installation and Build

1.  **Clone the repository:**
    ```sh
    git clone https://github.com/lev666/BMPComparerConverter.git
    ```

2.  **Navigate into the project directory:**
    ```sh
    cd BMPComparerConverter
    ```

3.  **Run the build script:**
    ```sh
    bash scripts/build.sh
    ```
    ***
*The executables will be available in the `build/src/` directory.*
## Test coverage

#### Also, when building via CMake, the project automatically runs basic coverage tests.

You can **run the tests** separately using:

**Run the tests script:**
```sh
bash scripts/tests.sh
```
## Authors

- [@lev666](https://www.github.com/lev666)


## Usage/Examples

#### After building the project, you will have two utilities: **converter** and **comparer**.
***

- Inverting an Image (converter):

Takes two arguments: an input file and an output file name for the new, inverted image.

```sh
./build/src/converter image1.bmp image1_inv.bmp
```
***
- Comparing Images (comparer):

Takes two files to compare. If the files differ, the program will output the coordinates of the first 100 differing pixels.

```sh
./build/src/comparer image1.bmp image2.bmp
```

## License

This project is licensed under the [MIT](https://github.com/lev666/BMPComparerConverter/blob/main/LICENSE)
