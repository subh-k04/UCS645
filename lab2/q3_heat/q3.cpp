#include <iostream>
#include <vector>
#include <omp.h>
#include <iomanip>
#include <algorithm>

int main() {

    const int N = 2000;
    const int STEPS = 200;
    const int TILE = 64;

    const double total_updates =
        (double)(N-2) * (N-2) * STEPS;

    std::vector<std::vector<double>> grid(N, std::vector<double>(N, 50.0));
    std::vector<std::vector<double>> next(N, std::vector<double>(N, 50.0));

    // boundary heat source
    for (int i = 0; i < N; i++)
        grid[0][i] = 100.0;

    std::cout << "\nThreads | Time(s) | Speedup | Efficiency | Throughput(Mcells/s) | TotalHeat\n";
    std::cout << "-------------------------------------------------------------------------------\n";

    double T1 = 0.0;

    for (int threads = 1; threads <= 12; threads++) {

        omp_set_num_threads(threads);

        // reset grid for fair comparison
        for (int i = 0; i < N; i++)
            for (int j = 0; j < N; j++)
                grid[i][j] = (i == 0 ? 100.0 : 50.0);

        double start = omp_get_wtime();

        double total_heat = 0.0;

        for (int step = 0; step < STEPS; step++) {

            #pragma omp parallel for collapse(2) schedule(runtime) reduction(+:total_heat)
            for (int ii = 1; ii < N-1; ii += TILE) {
                for (int jj = 1; jj < N-1; jj += TILE) {

                    for (int i = ii; i < std::min(ii + TILE, N-1); i++) {
                        for (int j = jj; j < std::min(jj + TILE, N-1); j++) {

                            next[i][j] =
                                0.25 * (grid[i+1][j] +
                                        grid[i-1][j] +
                                        grid[i][j+1] +
                                        grid[i][j-1]);

                            total_heat += next[i][j];
                        }
                    }
                }
            }

            grid.swap(next);
        }

        double end = omp_get_wtime();

        double Tp = end - start;

        if (threads == 1)
            T1 = Tp;

        double Sp = T1 / Tp;
        double Ep = Sp / threads;
        double throughput = (total_updates / Tp) / 1e6;

        std::cout << std::setw(7) << threads << " | "
                  << std::fixed << std::setprecision(4)
                  << std::setw(7) << Tp << " | "
                  << std::setw(7) << Sp << " | "
                  << std::setw(10) << Ep << " | "
                  << std::setw(12) << throughput << " | "
                  << total_heat
                  << "\n";
    }

    std::cout << "\n";

    return 0;
}
