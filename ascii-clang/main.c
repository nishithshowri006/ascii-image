#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
// #include "lib.c"
// #define FILENAME_ARG 0
// #define ASCII " .:-=+*#%%@"

unsigned char *get_grayscale_data(char *filename, int *width, int *height, int *channels, int args, char *kwargs[]);
void write_ascii_txt_file(char *filename, unsigned char *data, int width, int height);
void apply_gaussian_filter(char *filename, int width, int height, int channels, int kernel_size, int sigma);

int main(int argc, char *argv[])
{
    int width, height, channels, args;
    int kernel_size;
    int sigma;

    char *filename = argv[1];
    char *output_filename = argv[2];
    kernel_size = 11;
    sigma = 5;
    args = 1;
    char *kwargs[args];
    kwargs[0] = output_filename;

    // gets grayscale image and writes it out to the file named from kwargs
    unsigned char *data = get_grayscale_data(filename, &width, &height, &channels, 1, kwargs);

    // outputs an ascii text file
    write_ascii_txt_file("ascii.txt", data, width, height);

    // applies gaussian filter to an image
    apply_gaussian_filter(filename, width, height, channels, kernel_size, sigma);
    free(data);
    return 0;
}
