# Parallel Correlation Matrix Computation

---

## 1. Overview

This project computes the **Pearson correlation coefficient** between all pairs of row vectors in a matrix of size **ny × nx**.

Each row represents an input vector.  
For every pair *(i, j)* such that:

```
0 ≤ j ≤ i < ny
```

the correlation between `row_i` and `row_j` is calculated and stored in:

```
result[i + j * ny]
```

Only the **lower triangular matrix** is computed to avoid redundant calculations.

All arithmetic operations use **double precision** for numerical stability.

---

## 2. Implemented Versions

Three implementations were developed and compared:

| Version | Description |
|----------|------------|
| Sequential | Single-threaded baseline implementation |
| OpenMP | Multi-threaded parallel implementation |
| Fast (Optimized) | OpenMP + SIMD + compiler optimizations |

---

## 3. Compilation

Compile the project using:

```bash
make
```

Compiler flags used:

```
-O3 -march=native -fopenmp
```

- `-O3` → enables aggressive compiler optimizations  
- `-march=native` → enables CPU-specific vector instructions (AVX2)  
- `-fopenmp` → enables OpenMP multithreading support  

---

## 4. Execution

Run the program using:

```bash
./correlate <ny> <nx> <threads> <mode>
```

Where:

- `threads` → number of OpenMP threads  
- `mode` →  
  - `0` = Sequential  
  - `1` = OpenMP  
  - `2` = Fast (Optimized)

Example:

```bash
./correlate 2000 2000 8 2
```

---

## 5. Experimental Setup

**Processor:** AMD Ryzen 5 5500U  
- 6 Physical Cores  
- 12 Hardware Threads  
- AVX2 Support  
- 4 MB L3 Cache  

**Compiler:** GCC 11.4.0  
**Environment:** WSL2  

---

# 6. Performance Results (2000 × 2000 Matrix)

## Execution Time Comparison

### 1 Thread

| Version | Time (sec) |
|----------|------------|
| Sequential | 10.56 |
| OpenMP | 10.56 |
| Fast | 6.45 |

---

### 4 Threads

| Version | Time (sec) |
|----------|------------|
| Sequential | 10.56 |
| OpenMP | 2.69 |
| Fast | 1.80 |

---

### 8 Threads

| Version | Time (sec) |
|----------|------------|
| Sequential | 10.56 |
| OpenMP | ~1.21 |
| Fast | ~1.03 |

---

# 7. Speedup Analysis (Fast Version)

Speedup formula:

```
Speedup = T1 / Tn
```

| Threads | Time (sec) | Speedup |
|----------|------------|----------|
| 1 | 6.45 | 1.00× |
| 2 | 3.34 | 1.93× |
| 4 | 1.89 | 3.41× |
| 6 | 1.39 | 4.64× |
| 8 | 1.21 | 5.33× |
| 12 | 1.03 | 6.26× |

Parallel Efficiency at 12 threads:

```
Efficiency ≈ 6.26 / 12 ≈ 52%
```

Efficiency peaks near the physical core count (6 threads) and decreases beyond that due to SMT overhead and memory limitations.

---

# 8. Larger Matrix Scaling (3000 × 3000)

Fast Version:

| Threads | Time (sec) | Speedup |
|----------|------------|----------|
| 1 | 21.95 | 1.00× |
| 6 | 4.90 | 4.48× |
| 12 | 3.49 | 6.29× |

Observation:

Scaling improves for larger matrices, indicating better utilization of computational resources for heavier workloads.

---

# 9. Performance Counter Highlights (perf)

Sequential (1 Thread):
- ~80B instructions  
- IPC ≈ 1.98  
- Full single-core utilization  

Optimized (12 Threads):
- ~72B instructions  
- IPC ≈ 1.59  
- ~10 CPU cores actively utilized  

Interpretation:

- Instruction count reduced due to optimization.
- Lower IPC in parallel mode due to memory contention.
- High CPU utilization confirms effective multithreading.

---

# 10. Optimization Techniques Used

- Precomputation of row means and normalization factors  
- Lower triangular computation only  
- OpenMP parallelization  
- Loop collapse for nested loops  
- SIMD reduction pragma  
- Compiler optimization with `-O3`  
- Improved memory access locality  

---

# 11. Observations

- Time complexity grows approximately **O(ny² × nx)**.
- Parallel implementation significantly reduces runtime.
- Speedup scales nearly linearly up to physical core count.
- Hyper-threading provides smaller additional gains.
- Memory bandwidth limits scaling at higher thread counts.
- SIMD improves inner-loop arithmetic throughput.

---

# 12. Conclusion

The optimized implementation successfully combines:

- Thread-level parallelism (OpenMP)
- Instruction-level parallelism (SIMD)
- Compiler-level optimizations

It achieves over **6× speedup** on a 6-core processor.

The experiment demonstrates effective parallel design while highlighting real-world limitations such as memory bandwidth and cache hierarchy constraints.

---