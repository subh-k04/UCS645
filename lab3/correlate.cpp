#include <iostream>
#include <vector>
#include <cmath>
#include <omp.h>

// Task 1: Simple Sequential Baseline
void correlate_sequential(int ny, int nx, const float* data, float* result) {
    for (int i = 0; i < ny; i++) {
        for (int j = 0; j <= i; j++) {
            double s_x = 0.0, s_y = 0.0, s_xx = 0.0, s_yy = 0.0, s_xy = 0.0;
            for (int k = 0; k < nx; k++) {
                double x = (double)data[k + i * nx];
                double y = (double)data[k + j * nx];
                s_x += x; s_y += y;
                s_xx += x * x; s_yy += y * y;
                s_xy += x * y;
            }
            double num = (nx * s_xy - s_x * s_y);
            double den = std::sqrt((nx * s_xx - s_x * s_x) * (nx * s_yy - s_y * s_y));
            result[i + j * ny] = (den != 0) ? (float)(num / den) : 0.0f;
        }
    }
}

// Task 2: Parallelized with OpenMP
void correlate_parallel(int ny, int nx, const float* data, float* result) {
    #pragma omp parallel for collapse(2)
    for (int i = 0; i < ny; i++) {
        for (int j = 0; j <= i; j++) {
            double s_x = 0.0, s_y = 0.0, s_xx = 0.0, s_yy = 0.0, s_xy = 0.0;
            for (int k = 0; k < nx; k++) {
                double x = (double)data[k + i * nx];
                double y = (double)data[k + j * nx];
                s_x += x; s_y += y;
                s_xx += x * x; s_yy += y * y;
                s_xy += x * y;
            }
            double den = std::sqrt((nx * s_xx - s_x * s_x) * (nx * s_yy - s_y * s_y));
            result[i + j * ny] = (den != 0) ? (float)((nx * s_xy - s_x * s_y) / den) : 0.0f;
        }
    }
}

// Task 3: Highly Optimized (SIMD + Memory Access Optimization)
void correlate_optimized(int ny, int nx, const float* data, float* result) {
    std::vector<double> mu(ny), inv_sigma(ny);

    #pragma omp parallel for
    for (int i = 0; i < ny; i++) {
        double sum = 0.0, sq_sum = 0.0;
        for (int k = 0; k < nx; k++) {
            double val = (double)data[k + i * nx];
            sum += val;
            sq_sum += val * val;
        }
        mu[i] = sum / nx;
        double var = (sq_sum / nx) - (mu[i] * mu[i]);
        inv_sigma[i] = (var > 0) ? 1.0 / std::sqrt(var * nx * nx) : 0.0;
    }

    #pragma omp parallel for schedule(dynamic)
    for (int i = 0; i < ny; i++) {
        for (int j = 0; j <= i; j++) {
            double dot = 0.0;
            #pragma omp simd reduction(+:dot)
            for (int k = 0; k < nx; k++) {
                dot += ((double)data[k + i * nx] - mu[i]) * ((double)data[k + j * nx] - mu[j]);
            }
            result[i + j * ny] = (float)(dot * inv_sigma[i] * inv_sigma[j] * nx);
        }
    }
}


void correlate(int ny, int nx, const float* data, float* result) {
    correlate_optimized(ny, nx, data, result);
}