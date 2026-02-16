# Parallel Correlation Matrix Computation using OpenMP
UCS645 – Parallel & Distributed Computing

============================================================

1. Problem Description

Given a matrix of size:

- ny rows
- nx columns

Each row represents a vector of length nx.

For all pairs (i, j) such that:

0 ≤ j ≤ i < ny

we compute the Pearson correlation coefficient between row i and row j.

The result is stored in:

result[i + j*ny]

The computational complexity is:

O(ny^2 × nx)

This makes the problem computationally intensive and suitable for parallel optimization.

============================================================

2. Mathematical Background

The Pearson correlation coefficient between two vectors x and y is:

corr(x,y) =
(n Σxy − Σx Σy) /
sqrt[(n Σx^2 − (Σx)^2)(n Σy^2 − (Σy)^2)]

Where:
- Numerator → covariance
- Denominator → product of standard deviations
- Result ∈ [-1, 1]

All arithmetic was performed using double precision to ensure numerical stability.

============================================================

3. Implemented Versions

Three implementations were developed and compared.

------------------------------------------------------------

3.1 Sequential Baseline

- Triple nested loop
- All statistical values computed inside inner loop
- No parallelism
- Serves as performance baseline

Time Complexity:
O(ny^2 × nx)

------------------------------------------------------------

3.2 OpenMP Parallel Version

Used:

#pragma omp parallel for collapse(2)

- Parallelizes outer pair loops (i, j)
- Each thread computes independent correlation pairs
- No race conditions (each thread writes to unique result index)

This version exploits thread-level parallelism.

------------------------------------------------------------

3.3 Optimized Version (SIMD + OpenMP)

Three major optimizations were introduced:

1) Precomputation of Mean and Variance
   - Mean computed once per row
   - Inverse standard deviation computed once per row
   - Eliminates redundant statistical recomputation

2) SIMD Vectorization
   Used:
   #pragma omp simd reduction(+:dot)

   - Enables AVX2 vector instructions
   - Improves floating-point throughput
   - Exploits instruction-level parallelism

3) Dynamic Scheduling
   Used:
   schedule(dynamic)

   - Improves load balancing
   - Important because workload decreases as j ≤ i

============================================================

4. Experimental Setup

Processor: AMD Ryzen 5 5500U
- 6 physical cores
- 12 hardware threads
- AVX2 supported
- L3 Cache: 4 MB

Compiler: GCC 11.4.0

Compilation Flags:
-O3 -march=native -fopenmp

Explanation:
- -O3 → aggressive optimization
- -march=native → enable CPU-specific instructions (AVX2)
- -fopenmp → enable multithreading

Experiments conducted in WSL2.

============================================================

5. Performance Results (2000 x 2000)

5.1 Version Comparison

Sequential (1 thread)   : 10.56 sec
Parallel (4 threads)    :  2.69 sec
Optimized (4 threads)   :  1.80 sec

Speedup vs Sequential:

Parallel (4 threads):
10.56 / 2.69 ≈ 3.92x

Optimized (4 threads):
10.56 / 1.80 ≈ 5.86x

Observation:
The optimized implementation provides nearly 6× improvement over baseline.

============================================================

6. Thread Scaling – Optimized Version (2000 x 2000)

Threads   Time (s)   Speedup
--------------------------------
1         6.45       1.00x
2         3.34       1.93x
4         1.89       3.41x
6         1.39       4.64x
8         1.21       5.33x
12        1.03       6.26x

Observations:

- Near-linear scaling up to 6 threads (physical cores).
- Diminishing returns beyond 6 threads.
- Final speedup ≈ 6.26x.

This matches expected behavior on a 6-core CPU with SMT.

============================================================

7. Larger Matrix Scaling (3000 x 3000)

Optimized Version:

Threads   Time (s)   Speedup
--------------------------------
1         21.95      1.00x
6         4.90       4.48x
12        3.49       6.29x

Observations:

- Larger matrix improves parallel efficiency.
- Scaling remains strong at 12 threads.
- Confirms algorithm benefits more as problem size increases.

============================================================

8. Performance Counter Analysis (perf)

Sequential (1 thread):

- Cycles: 40.68B
- Instructions: 80.44B
- IPC: 1.98
- CPU utilization: 1.0

Optimized (12 threads):

- Cycles: 45.59B
- Instructions: 72.49B
- IPC: 1.59
- CPU utilization: 10.22

Interpretation:

1) Instruction Count Reduction
Optimized version executes fewer instructions, confirming reduced redundant computation.

2) IPC Reduction in Parallel Mode
Lower IPC in parallel mode is expected due to:
- Increased memory traffic
- Cache contention
- Thread competition for resources

3) High CPU Utilization
10.22 CPUs utilized confirms effective multi-core execution.

============================================================

9. Bottleneck Analysis

The algorithm is memory-intensive:

- Each correlation requires full row traversal.
- High memory bandwidth demand.
- Large working set size.

Scaling is primarily limited by:

- Memory bandwidth saturation
- Cache hierarchy limitations
- SMT resource sharing

Performance gains reduce beyond physical core count.

============================================================

10. Key Insights

1. OpenMP significantly reduces runtime.
2. SIMD improves floating-point throughput.
3. Near-linear scaling up to physical core count.
4. Larger matrices improve parallel efficiency.
5. Memory subsystem becomes primary bottleneck.

============================================================

11. Conclusion

The optimized OpenMP + SIMD implementation achieved:

- ~6× speedup over sequential baseline
- Efficient scaling on 6-core architecture
- Effective utilization of AVX2 instructions
- Significant reduction in redundant computations