#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <immintrin.h>
#include <string.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>


#include "defs.h"

typedef struct Program_files {
    FILE *input_fp;
    FILE *output_fp;
} Program_files;

void grayscale_without_fscanf(char *input_str, char *output_str);
void grayscale_mmapped(char *input_str, char *output_str);
void grayscale_mmapped_parallel(char *input_str, char *output_str);