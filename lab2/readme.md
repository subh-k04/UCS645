# Lab 2 — Parallel Performance Experiments using OpenMP

**Course:** UCS645 — Parallel & Distributed Computing
**Platform:** AMD Ryzen 5 5500U (6 cores / 12 threads)
**Environment:** Linux (WSL)

---

# Experiment 1 — Molecular Dynamics Kernel (Strong Scaling)

## Objective

Evaluate how a compute-intensive nested molecular interaction kernel scales with increasing OpenMP threads while keeping the total workload fixed.

---

## Results

| Threads | Time (s) | Speedup | Efficiency |
| ------- | -------- | ------- | ---------- |
| 1       | 1.6619   | 1.00    | 1.00       |
| 2       | 1.2724   | 1.31    | 0.66       |
| 3       | 0.9478   | 1.75    | 0.58       |
| 4       | 0.7591   | 2.19    | 0.55       |
| 5       | 0.6174   | 2.69    | 0.54       |
| 6       | 0.5377   | 3.09    | 0.52       |
| 7       | 0.4601   | 3.61    | 0.52       |
| 8       | 0.4322   | 3.85    | 0.48       |
| 9       | 0.3852   | 4.31    | 0.48       |
| 10      | 0.3607   | 4.61    | 0.46       |
| 11      | 0.3390   | 4.90    | 0.45       |
| 12      | 0.3241   | 5.13    | 0.43       |

---

## Observations

* Execution time decreases steadily with thread count.
* Speedup is strong but sub-linear due to synchronization overhead.
* Efficiency declines gradually as shared memory contention increases.
* Scaling beyond physical cores shows diminishing returns.

---

## Graph — Scaling Behavior

![Q1 Scaling](q1_molecular/q1graph.png)

---

## Interpretation

The nested interaction kernel benefits significantly from parallel execution. Performance is bounded by shared memory access and thread coordination overhead, demonstrating realistic strong scaling behavior.

---

---

# Experiment 2 — Smith–Waterman DNA Alignment (Scheduling Study)

## Objective

Analyze how OpenMP scheduling strategies affect performance of a dependency-aware wavefront dynamic programming algorithm.

---

## Q2 — Scheduling Comparison

This table compares execution time of the Smith–Waterman wavefront algorithm under different OpenMP scheduling strategies.

| Threads | Static Time (s) | Dynamic Time (s) | Guided Time (s) |
| ------- | --------------- | ---------------- | --------------- |
| 1       | 0.0592          | 0.1219           | 0.0592          |
| 2       | 0.0306          | 0.3276           | 0.0378          |
| 3       | 0.0345          | 0.3810           | 0.0392          |
| 4       | 0.0325          | 0.3746           | 0.0419          |
| 5       | 0.0303          | 0.3688           | 0.0447          |
| 6       | 0.0314          | 0.3112           | 0.0482          |
| 7       | 0.0352          | 0.3121           | 0.0470          |
| 8       | 0.0362          | 0.2822           | 0.0488          |
| 9       | 0.0358          | 0.2674           | 0.0539          |
| 10      | 0.0371          | 0.2847           | 0.0565          |
| 11      | 0.0431          | 0.5469           | 0.0635          |
| 12      | 0.0870          | 0.7498           | 0.1856          |

### Observations

* Static scheduling provides the best low-thread performance due to minimal overhead.
* Dynamic scheduling introduces significant scheduling overhead, resulting in slower execution.
* Guided scheduling offers moderate load balancing but still suffers from dependency limits.
* Wavefront dependencies restrict effective parallel scaling.

---

## Graph — Scheduling Comparison

![Q2 Scheduling](q2_alignment/q2graph.png)

---

## Interpretation

The algorithm’s diagonal dependency structure limits scalability. Scheduling overhead dominates at higher thread counts, demonstrating that algorithm structure governs achievable parallelism.

---

---

# Experiment 3 — Heat Diffusion Simulation

## Objective

Evaluate spatial OpenMP parallelization of a 2D heat diffusion model using execution time, speedup, efficiency, and throughput.

---

## Performance Results

| Threads | Time (s) | Speedup | Efficiency | Throughput (Mcells/s) |
| ------- | -------- | ------- | ---------- | --------------------- |
| 1       | 1.8938   | 1.00    | 1.00       | 421.58                |
| 2       | 1.2363   | 1.53    | 0.77       | 645.79                |
| 3       | 1.0606   | 1.79    | 0.60       | 752.76                |
| 4       | 0.9435   | 2.01    | 0.50       | 846.19                |
| 5       | 0.8716   | 2.17    | 0.43       | 916.06                |
| 6       | 0.8310   | 2.28    | 0.38       | 960.72                |
| 7       | 0.8054   | 2.35    | 0.34       | 991.30                |
| 8       | 0.8498   | 2.23    | 0.28       | 939.48                |
| 9       | 0.7755   | 2.44    | 0.27       | 1029.47               |
| 10      | 0.8009   | 2.36    | 0.24       | 996.84                |
| 11      | 0.8411   | 2.25    | 0.20       | 949.19                |
| 12      | 0.9041   | 2.09    | 0.17       | 883.07                |

---

## Observations

* Performance improves until memory bandwidth limits scaling.
* Throughput peaks at moderate thread counts.
* Efficiency declines due to memory contention.
* Numerical correctness is preserved.

---

## Graph — Performance Scaling

![Q3 Performance](q3_heat/q3graph.png)

---

## Interpretation

The stencil-based heat simulation parallelizes effectively but becomes memory-bound at higher concurrency. Scheduling strategy and memory behavior determine performance limits.

---

---

# Overall Discussion

* Compute-heavy kernels scale best.
* Dependency-driven algorithms limit concurrency.
* Memory bandwidth becomes a bottleneck at high thread counts.
* Scheduling strategy strongly influences performance.

---

# Conclusion

OpenMP parallelization delivers measurable acceleration, but scalability is governed by algorithm structure, synchronization overhead, and hardware limits. Effective optimization balances concurrency with system constraints.

---
