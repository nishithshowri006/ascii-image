#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <math.h>
#define STB_IMAGE_IMPLEMENTATION
#include "lib/stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "lib/stb_image_write.h"
#define M_PI 3.14159265358979323846
// #define ASCII "$@B%%8&WM#*oahkbdpqwmZO0QLCJUYXzcvunxrjft/\\|()1{}[]?-_+~<>i!lI;:,\" ^`'. "
#define ASCII_REV " .\'`^ \",:;Il!i><~+_-?][}{1)(|\\/tfjrxnuvczXYUJCLQ0OZmwqpdbkhao*#MW&8%%B@$"

unsigned char *get_grayscale_data(char *filename, int *width, int *height, int *channels, int args, char *kwargs[])
{

    // getting image data using stb_image library
    unsigned char *image = stbi_load(filename, width, height, channels, 0);

    // allocating the recieved data pointer memory to hold the pixels

    unsigned char *temp = malloc((*width) * (*height) * sizeof(temp));
    int some = 0;
    // converting the iamge to grayscale
    for (int i = 0, j = 0; i < (*width) * (*height) * (*channels); j++, i += *channels)
    {

        for (int k = 0; k < *channels; k++)
        {
            some += image[i + k];
        }
        temp[j] = some / (*channels);
        some = 0;
    }
    // see if we need to output the grayscale image
    if (args == 1)
    {
        stbi_write_bmp(kwargs[0], *width, *height, 1, temp);
    }
    free(image);

    return temp;
}

// note kernel size is a single value 3,5,7,9 and we build a 3x3,5x5..etc.
// Sigma stands for the standard deviation or normal distrubution value.
void create_normal_kernel(int kernel_size, int sigma, double **kernel_matrix)
{

    // subtracting this with i,j poisition co-ordinate will center the overall positions
    int half_size = kernel_size / 2;

    // track the sum values calculated by gausian formula so that this sum equal to one else normalize to one.
    double sum = 0.0;
    double x = 0, y = 0;
    double sigma_squared = pow(sigma, 2);
    // assign gaussian values to kernel matrix
    for (int i = 0; i < kernel_size; i++)
    {
        for (int j = 0; j < kernel_size; j++)
        {
            // centering the coordinates
            x = i - half_size;
            y = j - half_size;
            kernel_matrix[i][j] = exp(-(x * x + y * y) / (2 * sigma_squared)) / (M_PI * 2 * sigma_squared);
            sum += kernel_matrix[i][j];
        }
    }

    // normalize the dirstribution
    for (int i = 0; i < kernel_size; i++)
    {
        for (int j = 0; j < kernel_size; j++)
        {
            kernel_matrix[i][j] /= sum;
        }
    }
}

void apply_gaussian_filter(char *filename, int width, int height, int channels, int kernel_size, int sigma)
{

    if (kernel_size % 2 == 0 || &kernel_size == NULL)
    {
        printf("Kernel Size should be an odd number less than 11, falling to size 5x5 and sigma 1");
        kernel_size = 5;
        sigma = 1;
    }
    if (&sigma == NULL)
    {
        sigma = 1;
    }
    unsigned char *image_data = stbi_load(filename, &width, &height, &channels, 0);
    unsigned char *gaussan_data = calloc(width * height * channels, sizeof(gaussan_data));
    double **kernel_matrix;

    // allocating memory for our multi dim kernel amtrix
    kernel_matrix = malloc(kernel_size * sizeof(double));
    for (int i = 0; i < kernel_size; i++)
    {
        kernel_matrix[i] = calloc(kernel_size, sizeof(double));
    }

    // get the normal matrix
    create_normal_kernel(kernel_size, sigma, kernel_matrix);

    int index_x;
    int index_y;
    int char_index;
    double temp_sum;

    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            for (int c = 0; c < channels; c++)
            {
                temp_sum = 0.0;
                for (int k1 = 0; k1 < kernel_size; k1++)
                {
                    for (int k2 = 0; k2 < kernel_size; k2++)
                    {
                        double weight = kernel_matrix[k1][k2];
                        // cooridantes of x and y from center calculated as
                        index_y = i + k1 - kernel_size / 2;
                        index_x = j + k2 - kernel_size / 2;

                        // condition to check if they are out of this range we don't need to multiply them aka Zero padding
                        if (index_y >= 0 && index_y < height && index_x >= 0 && index_x < width)
                        {
                            // this is the index of the coordinate we want inside the image data
                            char_index = (width * index_y + index_x) * channels + c;
                            temp_sum += image_data[char_index] * weight;
                        }
                    }
                }
                // assign the sum of the weights to the pixel we are targetting
                char_index = (width * i + j) * channels + c;
                gaussan_data[char_index] = (unsigned char)temp_sum;
            }
        }
    }

    printf("here\n");
    char *output_file = "gaussian.jpg";
    // write our data out
    stbi_write_jpg(output_file, width, height, channels, gaussan_data, 0);
    for (int i = 0; i < kernel_size; i++)
    {
        free(kernel_matrix[i]);
    }
    free(kernel_matrix);
    stbi_image_free(image_data);
    free(gaussan_data);
}

unsigned char *get_ascii_array(unsigned char *data, int width, int height)
{
    char *ascii = ASCII_REV;
    // float factor = (strlen(ascii) - 1) / 255;
    unsigned char *ascii_data = malloc(width * height * sizeof(ascii_data));
    int char_index = 0;
    for (long i = 0; i < width * height; i++)
    {
        char_index = (data[i] * (strlen(ascii) - 3)) / 255;
        ascii_data[i] = ascii[char_index];
    }
    ascii_data[width * height] = '\0';
    return ascii_data;
}

void write_ascii_txt_file(char *filename, unsigned char *data, int width, int height)
{
    FILE *fp = fopen(filename, "w");
    unsigned char *ascii_data = get_ascii_array(data, width, height);
    int index = 0;
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            index = width * i + j;
            fputc(ascii_data[width * i + j], fp);
            fputc(' ', fp);

            // printf("%c", ascii_data[width * i + j]);
        }
        fputc('\n', fp);
        // printf("\n");
    }
    free(ascii_data);
    fclose(fp);
}