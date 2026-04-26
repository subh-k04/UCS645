#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <math.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

int main() {
    long num_steps = 100000000;
    double step = 1.0 / (double)num_steps;
    double sum = 0.0;
    
    double start_time, end_time, seq_time;

    start_time = omp_get_wtime();
    for (long i = 0; i < num_steps; i++) {
        double x = (i + 0.5) * step;
        sum += 4.0 / (1.0 + x * x);
    }
    double pi_seq = step * sum;
    end_time = omp_get_wtime();
    seq_time = end_time - start_time;

    printf("Threads: 1, Pi: %.10f, Time: %f, Speedup: 1.00\n", pi_seq, seq_time);

    int thread_counts[] = {2, 4, 6, 8};
    int num_counts = 4;
    double pi_final = 0.0;

    for (int t = 0; t < num_counts; t++) {
        int threads = thread_counts[t];
        omp_set_num_threads(threads);
        
        sum = 0.0;
        start_time = omp_get_wtime();
        
        #pragma omp parallel for reduction(+:sum)
        for (long i = 0; i < num_steps; i++) {
            double x = (i + 0.5) * step;
            sum += 4.0 / (1.0 + x * x);
        }
        
        double pi_par = step * sum;
        end_time = omp_get_wtime();
        double par_time = end_time - start_time;
        double speedup = seq_time / par_time;

        printf("Threads: %d, Pi: %.10f, Time: %f, Speedup: %f\n", threads, pi_par, par_time, speedup);
        pi_final = pi_par;
    }

    printf("Final Pi: %.10f, Error vs M_PI: %.10e\n", pi_final, fabs(pi_final - M_PI));

    return 0;
}
