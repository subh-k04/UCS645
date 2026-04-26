# Lab 7 — CUDA Part II

**Course:** UCS645 — Parallel & Distributed Computing
**GPU:** NVIDIA GeForce GTX 1650 (Turing, Compute 7.5)
**CUDA Version:** 13.2
**Platform:** Windows 11


## 1. Overview

Explores thread task differentiation and parallel merge sort implementation constraints.
Analyzes vector addition bandwidth profiling and memory throughput efficiency limits.


## 2. Problem Summary

| # | Problem | Approach | Key CUDA Concepts |
|---|---|---|---|
| Q1 | Thread Task Differentiation | Thread 0 iterative, Thread 1 formula | SIMT divergence, shared output buffer |
| Q2 | Parallel Merge Sort | CPU recursive vs GPU bottom-up | cudaEvent timing, kernel pipelining |
| Q3 | Vector Add Profiling | Static globals, bandwidth measurement | cudaMemcpyToSymbol, theoretical vs measured BW |


## 3. Problem Analysis

### Q1 — Different Tasks Per Thread

- Array size N=1024 threads launched.
- Thread 0 computes iterative sum yielding 524800.
- Thread 1 uses mathematical formula yielding 524800.
- Both methods match exactly showing correctness.
- Threads 2 through 1023 remain idle.
- Highlights SIMT divergence point among threads.


### Q2 — Merge Sort

- Array size N=1000 elements processed.
- CPU recursive timed via QueryPerformanceCounter.
- GPU utilizes bottom-up sorting phases.
- CPU took 0.294400ms, GPU took 0.481568ms.
- GPU achieved slow speedup of 0.611336x.
- GPU lost from launch overhead and phase dependencies.
- Amdahl's Law limits overall parallel acceleration.

### Q3 — Vector Addition Bandwidth Profiling

**1.1 Static Global Variables**

- Arrays declared as device global variables.
- Uses cudaMemcpyToSymbol and cudaMemcpyFromSymbol API.
- No dynamic cudaMalloc allocations needed here.

**1.2 Kernel Timing**

- Kernel time measured exactly 0.186016ms.
- Utilizes cudaEvent_t API for precision timing.

**1.3 Theoretical Bandwidth**

- Formula: 2 × memClockRate × busWidth/8 / 1e6.
- Peak theoretical bandwidth is 192.032000 GB/s.

**1.4 Measured Bandwidth**

- Read bytes equal 2 × N × 4.
- Write bytes equal 1 × N × 4.
- Measured bandwidth achieved 67.644247 GB/s (35.225508%).

## 4. Performance Tables

### Q2 — Merge Sort Comparison

| Method | Time (ms) | Speedup | Notes |
|---|---|---|---|
| CPU Merge Sort | 0.294400 | 1.00 | QueryPerformanceCounter |
| GPU Merge Sort | 0.481568 | 0.611x | cudaEvent, bottom-up phases |


### Q3 — Bandwidth Analysis

| Metric | Value |
|---|---|
| Kernel Execution Time | 0.186016 ms |
| Theoretical Bandwidth | 192.032000 GB/s |
| Measured Bandwidth | 67.644247 GB/s |
| Bandwidth Efficiency | 35.225508% |


## 5. Key Observations

- GPU lost to CPU; kernel launch overhead dominates at N=1000.
- Phase dependencies prevent true parallelism; bitonic sort is native alternative.
- Only 35.225508% theoretical bandwidth achieved; typical for naive vector add.
- Static globals avoid malloc overhead but need symbol memory API.
- Q1 SIMT divergence is harmless but serializes at warp scale.
- Amdahl's Law visible; serial merge phases cap maximum parallel speedup.


## 6. Key Takeaways

- GPU wins when compute outweighs launch overhead and memory costs.
- Merge sort fails on GPU; sequential phase dependencies kill parallelism.
- Theoretical BW rarely achieved; memory latency and access patterns limit throughput.
- cudaMemcpyToSymbol is the correct API for static device globals.
- SIMT divergence within a warp serializes execution; minimize kernel branching.
- Amdahl's Law applies to GPU constraints just like CPU.


## 7. Conclusion

GPU thread organization requires minimizing divergence and understanding memory limitations.
Hardware acceleration helps only when compute density overcomes memory transfer latency.
Significant gap exists between theoretical bandwidth and measured kernel throughput.
