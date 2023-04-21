#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb/stb_image_write.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <dirent.h>
#include <ctype.h>
#include <sys/stat.h>

#include "main.h"

int process_bw_image() {
    const char *filename = "test.jpg";
    int width, height, channels;

    unsigned char* image_data = stbi_load(filename, &width, &height, &channels, 0);

    unsigned char* BW = (unsigned char*)malloc(width * height * sizeof(unsigned char));
    
    unsigned char BW_min = 255;
    unsigned char BW_max = 0;

    long BW_sum = 0;
    long BW_square_sum = 0;


    /** First loop converts image read as rgb to grayscale, finds min and max
     *  values, and keeps a running sum and sum of squares. **/
    for (int i = 0; i < width * height; ++i) {
        // Single channel
        BW[i] = image_data[i * channels]/3 + image_data[i * channels + 1]/3 + image_data[i * channels + 2]/3; // Combine RGB channels into single grayscale channel
        if (BW[i] < BW_min) { // Check for smallest value
            BW_min = BW[i];
        }
        else if (BW[i] > BW_max) { // Check for largest value
            BW_max = BW[i];
        }
        // Compute the total sum and squared sum of all grayscale values
        // (used for mean and stdev calculation)
        BW_sum += BW[i];
        BW_square_sum += BW[i] * BW[i];

    }
    stbi_image_free(image_data);

    const int n = width * height;

    // Calculate the mean values
    // Mean = (sum of values) / (number of values)
    const unsigned char BW_mean = (unsigned char)(BW_sum / n);

    // Calculate standard deviation
    // stdev = sqrt( sum( each value^2) / (number of values) - mean^2)
    const unsigned char BW_sd = (unsigned char)sqrt(BW_square_sum / n - ((long)BW_mean * BW_mean));

    // Calculate 1st and 99th percentiles
    // 5th percentile value roughly 2 standard deviations below mean
    // 95th percentile value roughly 2 standard deviations above mean
    const unsigned char BW_5 = BW_mean - 2*BW_sd;
    const unsigned char BW_95 = BW_mean + 2*BW_sd;

    // Precompute the range between 1st and 99th percentile
    const unsigned char BW_inter_range = BW_95 - BW_5;

    for (int i = 0; i < width * height; ++i) {
        /* Process gray channel */
        if (BW[i] < BW_5) { // Clip low values
            BW[i] = BW_5;
        }
        else if (BW[i] > BW_95) { // Clip high values
            BW[i] = BW_95;
        }
        // Stretch values to a range of [0, 255] and then invert
        BW[i] = 255 - (255)*((BW[i]-BW_5)) / (BW_inter_range);
    }

    int result = stbi_write_jpg("output.jpg", width, height, 1, BW, 50); // 3 channels (R, G, B), quality = 100

    if (result == 0) {
        printf("Error writing JPEG file.\n");
    }

    // Free the memory allocated for arrays
    free(BW);

    return 0;
}
int process_rgb_image(char* in_path, char* out_path) {
    const char *filename = in_path;
    int width, height, channels;


    unsigned char *image_data = stbi_load(filename, &width, &height, &channels, 0);
    // Separate the channels
    unsigned char* R = (unsigned char*)malloc(width * height * sizeof(unsigned char));
    unsigned char* G = (unsigned char*)malloc(width * height * sizeof(unsigned char));
    unsigned char* B = (unsigned char*)malloc(width * height * sizeof(unsigned char));

    unsigned char R_min = 255;
    unsigned char G_min = 255;
    unsigned char B_min = 255;

    unsigned char R_max = 0;
    unsigned char G_max = 0;
    unsigned char B_max = 0;

    long long r_sum = 0;
    long long g_sum = 0;
    long long b_sum = 0;

    long long r_square_sum = 0;
    long long g_square_sum = 0;
    long long b_square_sum = 0;

    /** First loop splits the image into RGB channels, finds min and max
     *  values, and keeps a running sum and sum of squares. **/
    for (int i = 0; i < width * height; ++i) {
    /* RED channel */
        R[i] = (image_data[i * channels]); // Read red channel of image
        if (R[i] < R_min) { // Check for smallest value
            R_min = R[i];
        }
        else if (R[i] > R_max) { // Check for largest value
            R_max = R[i];
        }
        // Compute the total sum and squared sum of all RED values
        // (used for mean and stdev calculation)
        r_sum += R[i];
        r_square_sum += R[i] * R[i];

    /* GREEN channel */
        G[i] = (image_data[i * channels + 1]);
        if (G[i] < G_min) {
            G_min = G[i];
        }
        else if (G[i] > G_max) {
            G_max = G[i];
        }
        // Compute the total sum and squared sum of all RED values
        // (used for mean and stdev calculation)
        g_sum += G[i];
        g_square_sum += G[i] * G[i];

    /* BLUE channel */
        B[i] = (image_data[i * channels + 2]);
        if (B[i] < B_min) {
            B_min = B[i];
        }
        else if (B[i] > B_max) {
            B_max = B[i];
        }
        // Compute the total sum and squared sum of all RED values
        // (used for mean and stdev calculation)
        b_sum += B[i];
        b_square_sum += B[i] * B[i];
    }
    stbi_image_free(image_data);

    const int n = width * height;

    // Calculate the mean values of each channel
    // Mean = (sum of values) / (number of values)
    const unsigned char R_mean = (unsigned char)(r_sum / n);
    const unsigned char G_mean = (unsigned char)(g_sum / n);
    const unsigned char B_mean = (unsigned char)(b_sum / n);

    // Calculate standard deviation of each channel
    // stdev = sqrt( sum( each value^2) / (number of values) - mean^2)
    const unsigned char R_sd = (unsigned char)sqrt(r_square_sum / n - ((long)R_mean * R_mean));
    const unsigned char G_sd = (unsigned char)sqrt(g_square_sum / n - ((long)G_mean * G_mean));
    const unsigned char B_sd = (unsigned char)sqrt(b_square_sum / n - ((long)B_mean * B_mean));

    // Calculate 1st and 99th percentiles
    // 5th percentile value roughly 2 standard deviations below mean
    // 95th percentile value roughly 2 standard deviations above mean
    unsigned char R_5 = 0;
    unsigned char R_95 = 255;
    if (2*R_sd < R_mean){
        R_5 = R_mean - 2*R_sd;
    }
    if (2*R_sd < 255 - R_mean) {
        R_95 = R_mean + 2*R_sd;
    }

    unsigned char G_5 = 0;
    unsigned char G_95 = 255;
    if (2*G_sd < G_mean){
        G_5 = G_mean - 2*G_sd;
    }
    if (2*G_sd < 255 - G_mean) {
        G_95 = G_mean + 2*G_sd;
    }

    unsigned char B_5 = 0;
    unsigned char B_95 = 255;
    if (2*B_sd < B_mean){
        B_5 = B_mean - 2*B_sd;
    }
    if (2*B_sd < 255 - B_mean) {
        B_95 = B_mean + 2*B_sd;
    }
    


    // Precompute the range between 1st and 99th percentile
    const unsigned char R_inter_range = R_95 - R_5;
    const unsigned char G_inter_range = G_95 - G_5;
    const unsigned char B_inter_range = B_95 - B_5;
    //printf("B_inter_range: %d\n", B_inter_range);

    // Allocate space for the output image
    unsigned char* new_image = (unsigned char*)malloc(width * height * 3);


    for (int i = 0; i < width * height; ++i) {
    /* Process RED channel */
        if (R[i] < R_5) { // Clamp low red values
            R[i] = R_5;
        }
        else if (R[i] > R_95) { // Clamp high red values
            R[i] = R_95;
        }
        // Stretch values to a range of [0, 255] and then invert
        R[i] = 255 - (255)*((R[i]-R_5)) / (R_inter_range);
        // Store R channel value in final image array
        new_image[i * channels] = R[i];

    /* Process GREEN channel */
        if (G[i] < G_5) { // Clamp low green values
            G[i] = G_5;
        }
        else if (G[i] > G_95) { // Clamp high green values
            G[i] = G_95;
        }
        // Stretch values to a range of [0, 255] and then invert
        G[i] = 255 - (255)*((G[i]-G_5)) / (G_inter_range);
        // Store R channel value in final image array
        new_image[i * channels + 1] = G[i];

    /* Process BLUE channel */
        if (B[i] < B_5) { // Clamp low blue values
            B[i] = B_5;
        }
        else if (B[i] > B_95) { // Clamp high blue values
            B[i] = B_95;
        }
        // Stretch values to a range of [0, 255] and then invert
        B[i] = 255 - (255)*((B[i]-B_5)) / (B_inter_range);
        // Store R channel value in final image array
        new_image[i * channels + 2] = B[i];
    }
    int result = stbi_write_jpg(out_path, width, height, 3, new_image, 50); // 3 channels (R, G, B), quality = 100

    if (result == 0) {
        printf("Error writing JPEG file.\n");
    }

    // Free the memory allocated for arrays
    free(R);
    free(G);
    free(B);
    free(new_image);
    

    return 0;
}

void str_lower_case(char* str) {
    if (str == NULL) {
        return;
    }
    for (int i = 0; str[i] != '\0'; ++i) {
        str[i] = tolower(str[i]);
    }
}

void* thread_start(void* arg) {
    char* in_path = ((char**)arg)[0];
    char* out_path = ((char**)arg)[1];
    process_rgb_image(in_path, out_path);
    // Free the dynamically allocated memory
    free(in_path);
    free(out_path);
    free(arg);
    return NULL;
}

// Function to check if a file has .jpg extension
int is_jpg_file(const char* file_name) {
    char* ext = strrchr(file_name, '.');
    str_lower_case(ext);
    if (ext != NULL && strcmp(ext, ".jpg") == 0) {
        return 1;
    } else if (ext != NULL && strcmp(ext, ".jpeg") == 0) {
        return 1;
    }
    return 0;
}

// Function to get a list of all JPG files in a directory
char** get_jpg_files(const char* dir_path, int* num_files) {
    DIR* dir;
    struct dirent* ent;
    char** jpg_files = NULL;
    int count = 0;

    dir = opendir(dir_path);
    if (dir == NULL) {
        printf("Failed to open directory: %s\n", dir_path);
        return NULL;
    }

    while ((ent = readdir(dir)) != NULL) {
        if (ent->d_type == DT_REG && is_jpg_file(ent->d_name)) {
            count++;
            jpg_files = realloc(jpg_files, count * sizeof(char*));
            jpg_files[count - 1] = strdup(ent->d_name);
        }
    }

    closedir(dir);

    *num_files = count;
    return jpg_files;
}

int main() {
    const char* dir_path = "."; // Change this to the desired directory path
    int num_files;
    char** jpg_files = get_jpg_files(dir_path, &num_files);
    

    const char* out_dir_name = "NegativeQuickConvertC/"; // Directory name to be checked/created
    
    

    // Check if directory exists
    struct stat st;
    if (stat(out_dir_name, &st) == -1) {
        // Directory does not exist, create it
        if (mkdir(out_dir_name, 0700) == -1) {
            printf("Failed to create directory '%s'\n", out_dir_name);
            return 1; // Return with error status
        } else {
            printf("Directory '%s' created successfully\n", out_dir_name);
        }
    } else {
        printf("Directory '%s' already exists\n", out_dir_name);
    }

    if (jpg_files == NULL) {
        printf("No JPG files found in directory '%s'\n", dir_path);
        return 1;
    }

    pthread_t threads[num_files];    
    for (int i = 0; i < num_files; i++) {
        int len = strlen(jpg_files[i]) + strlen("out/") + 1;

        char** args = malloc(2 * sizeof(char*));
        args[0] = jpg_files[i];
        char* out_path = malloc(len * sizeof(char*));
        strcpy(out_path, out_dir_name);
        strcat(out_path, jpg_files[i]);
        args[1] = out_path;
        
        pthread_create(&threads[i], NULL, thread_start, args);
    }
        
    for (int i = 0; i < num_files; i++) {
        pthread_join(threads[i], NULL);
    }
    

    free(jpg_files);
    return 0;
}