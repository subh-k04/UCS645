#include <iostream>
#include <vector>
#include <cmath>
#include <omp.h>
#include <iomanip>
#include <cstdlib>

struct Particle {
    double x, y, z;
    double fx, fy, fz;
};

double simulate(std::vector<Particle>& p, int threads) {

    int N = p.size();
    double energy = 0.0;

    omp_set_num_threads(threads);

    // reset forces
    for (int i = 0; i < N; i++) {
        p[i].fx = p[i].fy = p[i].fz = 0.0;
    }

    #pragma omp parallel for reduction(+:energy) schedule(guided)
    for (int i = 0; i < N; i++) {

        for (int j = i + 1; j < N; j++) {

            double dx = p[i].x - p[j].x;
            double dy = p[i].y - p[j].y;
            double dz = p[i].z - p[j].z;

            double r2 = dx*dx + dy*dy + dz*dz + 1e-12;

            double inv = 1.0 / r2;
            double r6 = inv * inv * inv;

            // Lennard-Jones potential
            energy += 4.0 * (r6*r6 - r6);

            double f = 48.0 * inv * r6 * (r6 - 0.5);

            // race-safe force accumulation
            #pragma omp atomic
            p[i].fx += f * dx;

            #pragma omp atomic
            p[i].fy += f * dy;

            #pragma omp atomic
            p[i].fz += f * dz;

            #pragma omp atomic
            p[j].fx -= f * dx;

            #pragma omp atomic
            p[j].fy -= f * dy;

            #pragma omp atomic
            p[j].fz -= f * dz;
        }
    }

    return energy;
}

int main() {

    const int N = 8000;   // workload large enough for scaling
    std::vector<Particle> particles(N);

    // random initialization
    for (int i = 0; i < N; i++) {
        particles[i].x = rand()/(double)RAND_MAX * 10;
        particles[i].y = rand()/(double)RAND_MAX * 10;
        particles[i].z = rand()/(double)RAND_MAX * 10;
    }

    std::cout << "Threads  Time(s)\n";
    std::cout << "------------------\n";

    for (int threads = 1; threads <= 12; threads++) {

        double start = omp_get_wtime();

        simulate(particles, threads);

        double end = omp_get_wtime();

        std::cout << std::setw(7) << threads
                << "  "
                << std::fixed << std::setprecision(4)
                << (end - start)
                << "\n";
    }

    return 0;
}
