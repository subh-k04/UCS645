---

# Lab 4 — MPI Programming Exercises

**Course:** UCS645 — Parallel & Distributed Computing
**Platform:** Linux 6.6.87.2-microsoft-standard-WSL2 x86_64
**Environment:** Linux (WSL)

---

## 1. Overview

This lab explores distributed memory parallelism using the Message Passing Interface (MPI). The exercises demonstrate fundamental communication patterns in a distributed setting, covering both point-to-point messaging mechanisms for controlled data flow and collective operations for efficient, aggregated data-parallel workloads.

---

## 2. Exercises Summary

| # | Exercise | Problem | Communication Type | Key MPI Operations |
|---|---|---|---|---|
| Q1 | Ring Communication | Value accumulates as it passes through a ring of processes | Point-to-Point | `MPI_Send`, `MPI_Recv` |
| Q2 | Parallel Array Sum | Sum a large array distributed across processes | Collective | `MPI_Scatter`, `MPI_Reduce` |
| Q3 | Global Max and Min | Find global extremes across independently generated datasets | Collective + Location | `MPI_Reduce`, `MPI_MAXLOC`, `MPI_MINLOC` |
| Q4 | Parallel Dot Product | Compute dot product of two vectors in parallel | Collective | `MPI_Scatter`, `MPI_Reduce` |

---

## 3. Exercise Analysis

### Q1 — Ring Communication

**Objective:** Demonstrate value accumulation along a process chain using point-to-point message passing in a ring topology.

**Approach:**
- Process ranks conceptually form a closed ring using modular arithmetic to dictate the previous and next nodes.
- To prevent a deadlock where every process initially waits to receive, process 0 acts as the initiator, executing a send before its receive.
- Each intermediate process receives the current tally, adds its own rank to the value, and transmits it to the subsequent process.

**Result:**
Process 0 initiated with value 100. After traversing all 4 processes (each adding its rank), the final value returned to process 0 was 106, matching the expected result of 100 + (0+1+2+3) = 106.

**Observations:**
- Output ordering was inherently deterministic, as the strict point-to-point ring dependency chain forces processes to execute sequentially upon receiving their message.
- The staggered send/receive logic in process 0 effectively bypassed the circular wait condition, ensuring safe progression without deadlock.
- The final value confirms that the correct, unmodified path was taken through the designated sequence of processes without data loss.

---

### Q2 — Parallel Array Sum

**Objective:** Distribute array elements and compute their collective sum and average using collective scatter and reduce operations.

**Approach:**
- `MPI_Scatter` partitions an initialized array across all available processes in evenly sized chunks.
- Each process independently loops over its assigned subspace to calculate a local partial sum.
- `MPI_Reduce` aggregates all partial sums back to the root process using the `MPI_SUM` operation.

**Result:**
Global sum = 5050, Average = 50.5. Both match the closed-form result n(n+1)/2 = 5050 for n=100.

**Observations:**
- Work was accurately partitioned such that no boundary elements were missed or duplicated (25 distinct elements distributed per process).
- The non-deterministic output print order from individual processes correctly reflects standard MPI behavior, where asynchronous processes reach execution points at varying times.
- `MPI_Reduce` cleanly hides the complexity of data collection, yielding a mathematically sound global aggregation.

---

### Q3 — Global Max and Min

**Objective:** Identify the global maximum and minimum values across all nodes along with the process rank that originated them.

**Approach:**
- Each node seeded its random number generator distinctively using its rank to ensure disparate datasets across the system.
- Processes parsed their local arrays to identify their respective maximum and minimum numerical boundaries.
- `MPI_Reduce` leveraged the `MPI_MAXLOC` and `MPI_MINLOC` operations coupled with an `MPI_2INT` structure to concurrently find the global extremes and map their origin ranks.

**Result:**
Global Maximum: 992 (Process 0). Global Minimum: 1 (Process 0). `MPI_MAXLOC` and `MPI_MINLOC` correctly identified both the extreme values and their owning processes.

**Observations:**
- Both global extremes serendipitously landed on Process 0, which is entirely consistent with the expected probability distribution of random generation algorithms.
- `MPI_MAXLOC` proved vastly superior to a standard `MPI_MAX`, as it intrinsically maps the data ownership, eliminating the need for a secondary broadcast to locate the element.
- The rank-based seeding effectively prevented nodes from generating perfectly cloned datasets (a common peril of `srand(0)` and `srand(1)` in standard C libraries).

---

### Q4 — Parallel Dot Product

**Objective:** Compute the dot product of two vectors in parallel utilizing collective variable scatter operations.

**Approach:**
- Matching array segments from two vectors were dispersed to processes via `MPI_Scatter`.
- Individual nodes performed vector element-wise multiplication, yielding localized accumulation results.
- A final `MPI_Reduce` combined these partial sums down into a singular scalar global dot product result.

**Result:**
Global dot product = 120, matching manual calculation. Partial products per process: P0=22, P1=38, P2=38, P3=22.

**Observations:**
- The symmetric nature of the partial products emerged organically due to the mirrored structure of the original ascending and descending input vectors.
- Correctness was confirmed against the mathematical identity: (1×8)+(2×7)+(3×6)+(4×5)+(5×4)+(6×3)+(7×2)+(8×1) = 120.
- Despite execution variations resulting in different stdout sequencing, the reduction accurately collated the components into a deterministic, correct final sum.

---

## 4. Performance Analysis

Benchmarks were run on scaled versions of Q2 (1M element array) and Q4 (scaled vectors) using `MPI_Wtime()` across 1, 2, 4, and 8 processes.

**Speedup:** $S_p = T_1 / T_p$  
**Efficiency:** $E_p = (S_p / p) \times 100\%$  

### Q2 — Array Sum Scaling (1M Elements)

| Processes ($p$) | Time $T_p$ (sec) | Speedup $S_p$ | Efficiency $E_p$ |
|---|---|---|---|
| 1 | 0.00848 | 1.00 | 100.0% |
| 2 | 0.01056 | 0.80 | 40.2% |
| 4 | 0.01286 | 0.66 | 16.5% |
| 8 | 0.01581 | 0.54 | 6.7% |

### Q4 — Dot Product Scaling (Scaled Vectors)

| Processes ($p$) | Time $T_p$ (sec) | Speedup $S_p$ | Efficiency $E_p$ |
|---|---|---|---|
| 1 | 0.01988 | 1.00 | 100.0% |
| 2 | 0.01720 | 1.16 | 57.8% |
| 4 | 0.02225 | 0.89 | 22.3% |
| 8 | 0.02461 | 0.81 | 10.1% |

---

## 5. Scaling Observations

- Speedup trajectories for both arrays revealed a harsh penalty for adding distributed processes to lightweight problems. The Array Sum failed to achieve any speedup factor above 1.0, while the Dot Product achieved a very modest 1.16 speedup at 2 processes before plummeting.
- Efficiency dropped noticeably when exceeding 1 or 2 threads. For Array Sum, dropping from 1 to 2 processes cost nearly 60% of systemic efficiency instantly.
- The Dot Product exercise temporarily benefited more from splitting the workload than Array Sum due to an increased localized arithmetic intensity (incorporating both multiplication and continuous memory fetching across two independent arrays).
- Severe diminishing returns emerged across the 8-process executions. Spreading an already trivial workload across 8 independent memory spaces drastically elevated execution time instead of reducing it.
- Dot product displayed slightly better scaling integrity simply because its underlying computational density (more math per element) offered a larger fraction of concurrent work to offset the heavy data scattering cost. 

---

## 6. Communication Overhead Analysis

A sharp tradeoff exists between point-to-point and collective communications. Point-to-point models (like the Q1 ring) demand strict serialization, meaning communication latency severely dictates execution speed. Conversely, collective abstractions theoretically scale better, yet still incur steep logistical overhead to synchronize. The benchmark numbers emphatically indicate that when the computational workload per element is minimal (such as basic addition), the latency to marshal, physically disperse, and ultimately coalesce the distributed data arrays decisively eclipses the computational gains associated with dividing the array in the first place.

---

## 7. Key Takeaways

- Point-to-point (`MPI_Send`/`MPI_Recv`) grants explicit programmatic control over data, but dramatically elevates deadlock risks natively, necessitating cautious asynchronous or shifted block programming.
- Collective ops (`MPI_Scatter`, `MPI_Reduce`) natively abstract network topologies and data fragmentation, presenting an exceptionally clean integration for data-parallel tasks.
- `MPI_MAXLOC` and `MPI_MINLOC` represent a distinct algorithmic advantage, preserving index localization across distributed memory boundaries without enforcing a secondary tracking payload.
- Systemic parallel efficiency decays violently when the local computational load fails to out-scale the hardware interconnect costs. More cores do not unilaterally translate to shorter execution horizons.
- True, viable parallel efficiency implies an intimate dependency on problem dimensions; massive workloads are functionally required to amortize distribution latencies over local compute units.

---

## 8. Conclusion

This lab successfully illustrated fundamental Message Passing Interface (MPI) structural models, spotlighting the versatility and differing logic structures innate to point-to-point pipelines versus parallel collectives. The scaling metrics yielded crucial insight into real-world algorithm architecture on distributed hardware, underscoring that raw parallel efficiency relies primarily on surpassing a tipping point where computational density decisively eclipses inherent network distribution overheads. 

---
