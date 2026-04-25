# Lab 5 — MPI Part II: Blocking vs Non-Blocking Communication

**Course:** UCS645 — Parallel & Distributed Computing
**Platform:** Linux 6.6.87.2-microsoft-standard-WSL2 x86_64
**Environment:** Linux (WSL)

## 1. Overview

Features blocking vs non-blocking MPI and collective operations.
Highlights master-slave dynamic distribution and limits of Amdahl's Law.

## 2. Exercises Summary

| # | Exercise | Concept | Key MPI Functions |
|---|---|---|---|
| Q1 | DAXPY Operation | Data parallelism + speedup | MPI_Scatter, MPI_Gather |
| Q2 | Broadcast Race | Linear vs tree broadcast | MPI_Send, MPI_Bcast |
| Q3 | Distributed Dot Product | Amdahl's Law in practice | MPI_Bcast, MPI_Reduce |
| Q4 | Prime Finder | Master-slave task dispatch | MPI_Send, MPI_Recv, MPI_ANY_SOURCE |
| Q5 | Perfect Number Finder | Master-slave divisor testing | MPI_Send, MPI_Recv, MPI_ANY_SOURCE |

## 3. Exercise Analysis

### Q1 — DAXPY Operation

- Implements Y = a*X + Y over 65536 doubles
- Uses MPI_Scatter to deploy and MPI_Gather to recover
- Employs MPI_Barrier before MPI_Wtime for strict timing constraints
- Achieves successful 2.72x speedup curve at 4 total processes

### Q2 — Broadcast Race

- MyBcast uses manual loop creating O(p) iteration latency
- MPI_Bcast uses tree topology bringing O(log p) scaling
- Performance gap wildly widens as node count increases
- Root node becomes bottleneck in linear loop test

### Q3 — Distributed Dot Product

- Creates local memory chunks avoiding huge array allocations
- Uses MPI_Bcast for distributing scalar multiplier values
- MPI_Reduce performs parallel aggregation of partial sums
- Serial overhead portions trigger Amdahl's Law scaling limits

### Q4 — Prime Finder

- Master dispatches number ranges while parallel slaves test
- MPI_ANY_SOURCE provides robust dynamic workload balancing behavior
- Tag protocol sends 0=start, positive=prime, negative=composite
- System found exactly 1229 primes under 10000 maximum

### Q5 — Perfect Number Finder

- Reuses dynamic master-slave pattern over perfect number properties
- Slaves individually sum proper divisors across designated tasks
- System found specific perfect numbers: 6, 28, 496, 8128
- Dynamic dispatch beautifully suits this unpredictable irregular workload

## 4. Performance Analysis

### Q1 — DAXPY Speedup

| Processes | Time (s) | Speedup | Efficiency |
|---|---|---|---|
| 1 | 0.000049 | 1.00 | 100.0% |
| 2 | 0.000040 | 1.23 | 61.3% |
| 4 | 0.000018 | 2.72 | 68.1% |
| 8 | 0.000016 | 3.06 | 38.3% |

DAXPY Speedup Chart:
Processes
    1 | ■ 1.00
    2 | ■■ 1.23
    4 | ■■■■ 2.72
    8 | ■■■■■ 3.06

### Q2 — Broadcast Race Comparison

| Processes | MyBcast (s) | MPI_Bcast (s) | Speedup Ratio |
|---|---|---|---|
| 2  | 0.239036 | 0.020914 | 11.43x |
| 4  | 0.711881 | 0.041010 | 17.36x |
| 8  | 1.381668 | 0.190880 | 7.24x  |
| 16 | 4.729401 | 0.619101 | 7.64x  |

Broadcast Time Comparison Chart:
Processes
    2 | ■■               | ·
    4 | ■■■■■■           | ·
    8 | ■■■■■■■■■■■      | ■
   16 | ■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■ | ■■■■■
(Each ■ ≈ 0.12s)

### Q3 — Dot Product Speedup

| Processes | Time (s) | Speedup | Efficiency |
|---|---|---|---|
| 1 | 158.314575 | 1.00 | 100.0% |
| 2 | 106.249255 | 1.49 | 74.5%  |
| 4 | 64.263446  | 2.46 | 61.5%  |
| 8 | 44.558343  | 3.55 | 44.4%  |

Dot Product Speedup vs Ideal:
Speedup
8 |                              (ideal)
7 |                         *
6 |                    *
5 |               *
4 |          *
3 |     *              ● 3.55 (actual at 8p)
2 |  *         ● 2.46 (actual at 4p)
1 | ●─────────────────────────── Processes
1    2    4    8

## 5. Scaling Observations

- DAXPY efficiency collapses at 8 processes — workload too small
- Dot product scales better — 500M elements justify communication cost
- MyBcast time grows 20x from 2→16 processes; MPI_Bcast grows 30x less
- Efficiency for dot product halves from 1→8 processes (Amdahl's Law)
- 4 processes is the sweet spot for both Q1 and Q3 on this hardware
- Beyond 4 processes: communication overhead exceeds compute savings

## 6. Blocking vs Non-Blocking Analysis

- Blocking MPI_Send stalls until buffer is safe — simple but CPU sits idle
- Non-blocking MPI_Isend returns immediately — CPU overlaps compute and comms
- Ring deadlock occurs with blocking when array size overflows MPI buffers
- MPI_Waitall enforces synchronization after all non-blocking ops complete
- Use blocking for simple correctness-critical flows, non-blocking to hide latency

## 7. Key Takeaways

- MPI_Bcast beats manual for-loop broadcast by up to 17x at 4 processes
- Non-blocking calls hide latency — critical for large data transfers
- MPI_ANY_SOURCE enables dynamic load balancing in master-slave patterns
- Amdahl's Law: serial sync phases (root reduce/gather) cap max speedup
- Small workloads don't scale — communication overhead dominates
- MPI_Scatter + MPI_Gather is the cleanest pattern for data-parallel ops

## 8. Conclusion

Tree-based collective ops vastly outperform sequential manual loops.
Master-slave architecture handles irregular and unpredictable workloads cleanly.
Amdahl's Law enforces hard ceilings on every parallel iteration.
