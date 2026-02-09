#include <iostream>
#include <vector>
#include <string>
#include <omp.h>
#include <iomanip>
#include <cstdlib>
#include <algorithm>

int smith_waterman(const std::string& A,
                   const std::string& B,
                   int threads)
{
    int n = A.size();
    int m = B.size();

    const int match = 2;
    const int mismatch = -1;
    const int gap = -1;

    std::vector<std::vector<int>> H(n+1, std::vector<int>(m+1, 0));

    omp_set_num_threads(threads);

    int max_score = 0;

    // Wavefront parallelization
    for (int wave = 2; wave <= n + m; wave++) {

        #pragma omp parallel for schedule(runtime) reduction(max:max_score)
        for (int i = 1; i <= n; i++) {

            int j = wave - i;

            if (j < 1 || j > m) continue;

            int diag = H[i-1][j-1] +
                (A[i-1] == B[j-1] ? match : mismatch);

            int up = H[i-1][j] + gap;
            int left = H[i][j-1] + gap;

            int score = std::max({0, diag, up, left});

            H[i][j] = score;

            if (score > max_score)
                max_score = score;
        }
    }

    return max_score;
}

int main()
{
    const int N = 2000;  // workload size

    std::string A, B;

    const char bases[] = {'A','C','G','T'};

    for (int i = 0; i < N; i++) {
        A += bases[rand() % 4];
        B += bases[rand() % 4];
    }

    std::cout << "Threads  Time(s)\n";
    std::cout << "------------------\n";

    for (int threads = 1; threads <= 12; threads++) {

        double start = omp_get_wtime();

        smith_waterman(A, B, threads);

        double end = omp_get_wtime();

        std::cout << std::setw(7)
                  << threads << "  "
                  << std::fixed << std::setprecision(4)
                  << (end - start)
                  << "\n";
    }

    return 0;
}
