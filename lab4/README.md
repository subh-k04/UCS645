# Lab 4 — MPI Programming Exercises

**Course:** UCS645 — Parallel & Distributed Computing
**Platform:** Linux 6.6.87.2-microsoft-standard-WSL2 x86_64
**Environment:** Linux (WSL)

---

## 1. Overview

This lab explores distributed memory parallelism using the Message Passing Interface (MPI). We implemented point-to-point messaging for fine-grained control and collective operations to distribute and aggregate data payloads efficiently across multiple independent nodes.

---

## 2. Exercises Summary

| # | Exercise | Communication | Key MPI Functions | Expected Result |
|---|---|---|---|---|
| Q1 | Ring Communication | Point-to-Point | MPI_Send, MPI_Recv | 106 returned to P0 |
| Q2 | Parallel Array Sum | Collective | MPI_Scatter, MPI_Reduce | Sum=5050, Avg=50.5 |
| Q3 | Global Max and Min | Collective + Location | MPI_Reduce, MPI_MAXLOC, MPI_MINLOC | Correct extremes with owning rank |
| Q4 | Parallel Dot Product | Collective | MPI_Scatter, MPI_Reduce | 120 |

---

## 3. Exercise Analysis

### Q1 — Ring Communication
- Processes transmit a value logically in a circle using modular arithmetic to dictate targets
- Process 0 initiates the transmission sequence first, ensuring no circular waiting blocks execution
- The initial 100 accumulated accurately through the ranks (100+0+1+2+3), returning 106 to Process 0
- Output naturally executed sequentially as the sequential ring dependency inherently regulates execution order

### Q2 — Parallel Array Sum
- Process 0 fragmented an initialized 100-element array into evenly spaced 25-element distributed chunks
- Local summation perfectly partitioned the workload, summing 325, 950, 1575, and 2200 independently
- The reduction collective accurately centralized the partial sums mathematically back into 5050 exactly as expected
- Process outputs arrived out of strict logical order, reliably confirming typical asynchronous standard MPI execution

### Q3 — Global Max and Min
- Relying on rank-based sequence seeds securely forced independent numerical generation across all isolated processes
- Utilizing MPI_MAXLOC uniquely attaches the physical hardware rank to the extremum packet during the reduction protocol
- Both global extremes randomly originated from Process 0 (992 and 1) entirely due to pseudo-random coincidences
- Passing an MPI_2INT struct successfully carried the dual payload (value and origin) through the distributed reduction

### Q4 — Parallel Dot Product
- Identically matched array portions from two opposing arrays natively scattered into distributed pairs
- Symmetric partial computations predictably emerged (22, 38, 38, 22) owing directly to the mirrored inputs
- The global combined result of 120 definitively validated manual arithmetic checking and collective efficacy
- Reduction operations inherently abstract timing variability from node execution rates, ensuring stable algorithmic outputs

---

## 4. Performance Analysis

Benchmarks run using MPI_Wtime() on scaled inputs across 1, 2, 4, 8 processes.
Speedup: Sp = T1 / Tp | Efficiency: Ep = (Sp / p) × 100%

### Q2 — Array Sum (1M Elements)

| Processes | Time (s) | Speedup | Efficiency |
|---|---|---|---|
| 1 | 0.00848 | 1.00 | 100.0% |
| 2 | 0.01056 | 0.80 | 40.0% |
| 4 | 0.01286 | 0.66 | 16.5% |
| 8 | 0.01581 | 0.54 | 6.7% |

### Q4 — Dot Product (Scaled Vectors)

| Processes | Time (s) | Speedup | Efficiency |
|---|---|---|---|
| 1 | 0.01988 | 1.00 | 100.0% |
| 2 | 0.01720 | 1.16 | 58.0% |
| 4 | 0.02225 | 0.89 | 22.3% |
| 8 | 0.02461 | 0.81 | 10.1% |

---

## 5. Scaling Observations

- Expanding process counts aggressively penalized Array Sum speedup from the very first parallel jump
- Dot Product experienced minor benefit switching to 2 processes before swiftly degrading
- Trivial addition workloads completely expose communication bottlenecks compared to computation
- Utilizing 8 distinct processes compounded interconnect latency radically instead of solving computational drag
- Mathematical operations per byte heavily dictates overall systemic scaling efficacy

---

## 6. Communication Overhead Analysis

- Serialized dependencies make Point-to-Point models highly susceptible to isolated interconnect stutters
- Collective abstractions simplify the written code but physically manifest extreme data dispatching costs
- Sending and scattering massive 1M arrays creates overwhelming bandwidth choke points between memory strata
- Native tree-based reductions generally scale logarithmically but still suffer if computational weight is nonexistent

---

## 7. Key Takeaways

- Explicit point-to-point logic provides exact synchronization granularity though requires strict anti-deadlock designs
- Leveraging collective routines like Scatter and Reduce represent gold-standard efficiency defaults for parallel algorithms
- Specialized operators including MAXLOC circumvent wasteful reverse queries and multi-staged synchronizations
- Workloads mathematically require a certain volume to meaningfully eclipse interconnect payload dispatch penalties
- Linear speedups conceptually ignore raw hardware latency which invariably steals fractions of overall performance

---

## 8. Conclusion

MPI serves as a powerful foundational layer for designing complex distributed computations efficiently. The observed scaling collapse clearly reiterates that splitting small workloads across physical cores inherently harms execution speeds. To reap practical speedup from collective communication structures, computational density per vector element must vastly outweigh data transmission times.
