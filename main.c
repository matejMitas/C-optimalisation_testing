#include "main.h"

int main(int argc, char **argv) {
    srand(time(NULL));
    
    struct timeval st, et;

    double testing = 0;
    int tries = 1;

    for (int i = 0; i < tries; i++) {
        //printf("%d\n", i);
        gettimeofday(&st, NULL);
        //grayscale_without_fscanf(argv[1], "output/out_program.pgm");
        //grayscale_mmapped(argv[1], "output/out_program.pgm");
        grayscale_mmapped_parallel(argv[1], "output/out_program.pgm");
        
        gettimeofday(&et, NULL);
        double elapsed = ((et.tv_sec - st.tv_sec) * 1000000) + (et.tv_usec - st.tv_usec);

        testing += elapsed;
    }

    printf("\n%d us - %f s\n", (int)testing / tries, testing / (1000000 * tries));
        
    // #pragma omp for schedule(static, 1)
    // for (int i = 0; i < 10; i++) {
    //     printf("%d\n", i);
    // }

    //return run();
    return 0;
}