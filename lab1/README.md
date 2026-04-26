# Lab 1 — Introduction to OpenMP

**Course:** UCS645 — Parallel & Distributed Computing
**Platform:** AMD Ryzen 5 5500U (6 cores, 12 threads), Linux (WSL)
**Compiler:** GCC with -fopenmp


## 1. Overview

Introduces OpenMP fork-join parallelism using practical loop-based workloads.
Evaluates thread scaling across DAXPY, matrix multiply, and numerical integration.


## 2. Exercises Summary

| # | Exercise | Parallelization Strategy | Key OpenMP Directives |
|---|---|---|---|
| Q1 | DAXPY Loop | parallel for over vector elements | #pragma omp parallel for |
| Q2 | Matrix Multiply | 1D (outer loop) and 2D (collapse) threading | #pragma omp parallel for collapse(2) |
| Q3 | Pi Calculation | parallel for with reduction | #pragma omp parallel for reduction(+:sum) |


## 3. Exercise Analysis

### Q1 — DAXPY Loop

- Computes scalar-vector multiplication and vector addition operations.
- Vector arrays contain exactly 65536 double precision elements.
- Uses simple parallel for across independent vector indices.
- Maximum speedup achieved using only 2 threads.
- Thread management overhead completely destroyed computing performance gains.


### Q2 — Matrix Multiply

- Multiplies dense integer matrices sized 1000 by 1000.
- Sequential baseline execution required 6.151953 seconds.
- 1D strategy only threaded the outermost row loop.
- 2D strategy utilized collapse(2) for nested loops.
- 2D version achieved slightly better performance overall.
- Collapse improves thread distribution but risks cache locality.


### Q3 — Pi Calculation

- Approximates pi using 100000000 rectangle integration steps.
- Numerical integration handles completely independent data chunks.
- Reduction clause prevents concurrent race conditions on sum.
- Speedup scaled very linearly up to physical cores.
- Final pi accuracy achieved 2.2648549702e-14 absolute error.


## 4. Performance Analysis

### Q1 — DAXPY Speedup

| Threads | Time (s) | Speedup | Efficiency |
|---|---|---|---|
| 1 | 0.000264 | 1.000000 | 100.00% |
| 2 | 0.000529 | 0.498410 | 24.92% |
| 4 | 0.000633 | 0.416388 | 10.41% |
| 6 | 0.000611 | 0.431261 | 7.19% |
| 8 | 0.001218 | 0.216487 | 2.71% |
| 12 | 0.013692 | 0.019252 | 0.16% |


### Q2 — Matrix Multiply Speedup

| Threads | 1D Time (s) | 1D Speedup | 2D Time (s) | 2D Speedup |
|---|---|---|---|---|
| 1 (seq) | 6.151953 | 1.000000 | 6.151953 | 1.000000 |
| 2 | 3.819200 | 1.610796 | 3.571353 | 1.722583 |
| 4 | 1.897448 | 3.242225 | 1.963235 | 3.133580 |
| 6 | 1.385969 | 4.438740 | 1.385270 | 4.440977 |
| 8 | 1.106472 | 5.559971 | 1.090522 | 5.641294 |


### Q3 — Pi Calculation Speedup

| Threads | Time (s) | Speedup | Pi Value | Error |
|---|---|---|---|---|
| 1 | 0.661888 | 1.000000 | 3.1415926536 | 2.2648549702e-14 |
| 2 | 0.322376 | 2.053156 | 3.1415926536 | 2.2648549702e-14 |
| 4 | 0.169647 | 3.901564 | 3.1415926536 | 2.2648549702e-14 |
| 6 | 0.116502 | 5.681324 | 3.1415926536 | 2.2648549702e-14 |
| 8 | 0.089396 | 7.404032 | 3.1415926536 | 2.2648549702e-14 |


## 5. Scaling Observations

- DAXPY maximum speedup occurred at 2 threads despite extreme overhead.
- DAXPY performance collapsed beyond 2 threads due to microscopic workload.
- 2D collapse threading won slightly for dense matrix multiplication.
- Cache locality degradation offset some advantages of nested collapse threading.
- Pi calculation scaled extremely well across all thread configurations.
- Reduction variable localization makes pi scale far better than DAXPY.
- Serial fractions dictate maximum achievable parallel speedup per Amdahl's Law.


## 6. Key Takeaways

- OpenMP fork-join model only spawns threads inside parallel regions.
- Reduction clause remains mandatory for preventing concurrent write corruption.
- Threading extreme short loops severely damages overall execution performance.
- Collapse perfectly distributes nested iterations but modifies memory access patterns.
- Pi integration represents an embarrassingly parallel workload with zero dependencies.
- Maximum practical efficiency centers around the six physical processor cores.
- Hyper-threading above six cores yields heavily diminishing computational returns.


## 7. Conclusion

OpenMP provides accessible fork-join multithreading via simple compiler directives.
Thread overhead completely destroys speedup if computation volume remains insufficient.
Shared memory parallelism excels on massive independent arithmetic workloads.
