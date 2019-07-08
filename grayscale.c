#include "grayscale.h"

Program_files * open_files(char *input, char *output) {
    Program_files *files = (Program_files *) malloc(sizeof(Program_files));
    if (files == NULL)
        return files;

    files->input_fp = fopen(input, "r"); 
    files->output_fp = fopen(output, "wb"); 

    return files;
}

void close_files(Program_files *files) {
    fclose(files->input_fp);
    fclose(files->output_fp);
    free(files);
}

uint8_t skip_and_copy_header(uint8_t *buffer) {
    uint8_t newline_counter = 0;
    for (int i = 0; i < HEADER_LOOKAHEAD; i++) {
        if (buffer[i] == NEWLINE) {
            newline_counter = i;
        }
    }
    /*
    Search is finished on last newline but we want 
    the start of actual data
    */
    return newline_counter + 1;
}

void grayscale_without_fscanf(char *input_str, char *output_str) {

    Program_files *files = open_files(input_str, output_str);
    FILE *input = files->input_fp;
    FILE *output = files->output_fp;
   
    /*
    Read data len
    */
    fseek(input, 0, SEEK_END);
    uint32_t data_size = ftell(input);
    fseek(input, 0, SEEK_SET);
    /*
    Store input data to buffer
    */
    uint8_t *buffer_in = (uint8_t *) malloc(sizeof(uint8_t) * data_size);
    fread(buffer_in, sizeof(uint8_t), data_size, input);
    /*
    Allocate output image, copy header and change to P5
    */
    uint8_t offset = skip_and_copy_header(buffer_in);
    uint8_t *buffer_out = (uint8_t *) malloc(sizeof(uint8_t) * (((data_size - offset) / COLOR_CHANNELS) + offset));
    memcpy(buffer_out, buffer_in, offset);
    buffer_out[P_TYPE_INDEX] = P_TYPE;
    /*
    Skip header part of ppm
    */
    uint8_t *buffer_ptr = &buffer_in[offset];
    /*
    Triplet buffer
    */
    uint8_t *tp[COLOR_CHANNELS] = {NULL,};
    uint32_t i = 0;
    uint32_t output_counter = offset;

    for (; i < (data_size - offset); i+=COLOR_CHANNELS) {
        tp[0] = buffer_ptr;
        tp[1] = tp[0] + sizeof(uint8_t);
        tp[2] = tp[0] + sizeof(uint8_t) * 2;
        /* 
        Typecast to float is implicit as oposed to AVX
        */
        buffer_out[output_counter] = (uint8_t)((*tp[0] * R_COEF) + (*tp[1] * G_COEF) + (*tp[2] * B_COEF));
        output_counter++;
        buffer_ptr += COLOR_CHANNELS;
    }

    fwrite(buffer_out, sizeof(uint8_t), output_counter, output);
    /*
    
    */
    free(buffer_in);
    free(buffer_out);
    /*

    */
    close_files(files);
}

void grayscale_mmapped(char *input_str, char *output_str) {

    Program_files *files = open_files(input_str, output_str);
    FILE *output = files->output_fp;
   
    /*
    Read data len
    */
    int f = open(input_str, O_RDONLY, S_IRUSR);
    struct stat sb;
    fstat(f, &sb);
    uint32_t data_size = sb.st_size;
    /*
    Map memory
    */
    uint8_t *buffer_in = mmap(NULL, sb.st_size, PROT_READ, MAP_PRIVATE, f, 0);
    /*
    Store input data to buffer
    Allocate output image, copy header and change to P5
    */
    uint8_t offset = skip_and_copy_header(buffer_in);
    uint8_t *buffer_out = (uint8_t *) malloc(sizeof(uint8_t) * (((data_size - offset) / COLOR_CHANNELS) + offset));
    memcpy(buffer_out, buffer_in, offset);
    buffer_out[P_TYPE_INDEX] = P_TYPE;
    /*
    Skip header part of ppm
    */
    uint8_t *buffer_ptr = &buffer_in[offset];
    /*
    Triplet buffer
    */
    uint8_t *tp[COLOR_CHANNELS] = {NULL,};
    uint32_t i = 0;
    uint32_t output_counter = offset;



    for (; i < (data_size - offset); i+=COLOR_CHANNELS) {
        tp[0] = buffer_ptr;
        tp[1] = tp[0] + sizeof(uint8_t);
        tp[2] = tp[0] + sizeof(uint8_t) * 2;
        /* 
        Typecast to float is implicit as oposed to AVX
        */
        buffer_out[output_counter] = (uint8_t)((*tp[0] * R_COEF) + (*tp[1] * G_COEF) + (*tp[2] * B_COEF));
        output_counter++;
        buffer_ptr += COLOR_CHANNELS;
    }

    fwrite(buffer_out, sizeof(uint8_t), output_counter, output);
    /*
    
    */
    munmap(buffer_in, sb.st_size);
    free(buffer_out);
    /*

    */
    close_files(files);
    close(f);
}

void grayscale_mmapped_parallel(char *input_str, char *output_str) {

    Program_files *files = open_files(input_str, output_str);
    FILE *output = files->output_fp;
   
    /*
    Read data len
    */
    int f = open(input_str, O_RDONLY, S_IRUSR);
    struct stat sb;
    fstat(f, &sb);
    uint32_t data_size = sb.st_size;
    /*
    Map memory
    */
    uint8_t *buffer_in = mmap(NULL, sb.st_size, PROT_READ, MAP_PRIVATE, f, 0);
    /*
    Store input data to buffer
    Allocate output image, copy header and change to P5
    */
    uint8_t offset = skip_and_copy_header(buffer_in);
    uint8_t *buffer_out = (uint8_t *) malloc(sizeof(uint8_t) * (((data_size - offset) / COLOR_CHANNELS) + offset));
    memcpy(buffer_out, buffer_in, offset);
    buffer_out[P_TYPE_INDEX] = P_TYPE;
    /*
    Skip header part of ppm
    */
    uint8_t *buffer_ptr = &buffer_in[offset];
    /*
    Triplet buffer
    */
    uint8_t *tp[COLOR_CHANNELS] = {NULL,};
    uint32_t output_counter = offset;


    #pragma omp for
    for (uint32_t i = 0; i < (data_size - offset); i+=COLOR_CHANNELS) {
        tp[0] = buffer_ptr;
        tp[1] = tp[0] + sizeof(uint8_t);
        tp[2] = tp[0] + sizeof(uint8_t) * 2;
        /* 
        Typecast to float is implicit as oposed to AVX
        */
        buffer_out[output_counter] = (uint8_t)((*tp[0] * R_COEF) + (*tp[1] * G_COEF) + (*tp[2] * B_COEF));
        output_counter++;
        buffer_ptr += COLOR_CHANNELS;
    }

    fwrite(buffer_out, sizeof(uint8_t), output_counter, output);
    /*
    
    */
    munmap(buffer_in, sb.st_size);
    free(buffer_out);
    /*

    */
    close_files(files);
    close(f);
}