# Lab 6 — Introduction to CUDA

**Course:** UCS645 — Parallel & Distributed Computing
**GPU:** NVIDIA GeForce GTX 1650 (Turing, Compute 7.5)
**CUDA Version:** 13.2
**Platform:** Windows 11

## 1. Overview
Explores parallel computation with CUDA runtime features.
Demonstrates device queries, shared memory reduction, and matrix arithmetic.

## 2. GPU Specifications (from device_query output)

| Property | Value |
|---|---|
| GPU Name | NVIDIA GeForce GTX 1650 |
| Compute Capability | 7.5 |
| Global Memory | 4095.69 MB |
| Shared Memory / Block | 48.00 KB |
| Constant Memory | 64.00 KB |
| Warp Size | 32 |
| Max Threads / Block | 1024 |
| Max Block Dimensions | 1024, 1024, 64 |
| Max Grid Dimensions | 2147483647, 65535, 65535 |
| Multiprocessors | 14 |
| Double Precision | Supported (CC >= 1.3) |

## 3. Assignment A — Device Query

1. Architecture and compute capability of your GPU?
- Architecture is Turing
- Compute Capability is 7.5

2. Maximum block dimensions?
- X maximum is 1024
- Y maximum is 1024
- Z maximum is 64

3. If max grid dim = 65535 and max block dim = 512, max threads launchable?
- Max grid blocks times max block threads
- 65535 * 512 = 33,553,920 threads launchable

4. Why might a programmer choose NOT to launch max threads?
- Hardware physically lacks sufficient registers
- Shared memory demands cause resource starvation
- Thread scheduling overhead degrades actual throughput

5. What can limit max thread launch?
- Hardware register allocation per block
- Maximum shared memory thresholds
- Physical multiprocessor occupancy limits

6. What is shared memory? How much on your GPU?
- User-managed extraordinarily fast local L1 cache
- Block threads perfectly synchronize variables here
- Available amount is 48.00 KB per block

7. What is global memory? How much on your GPU?
- Standard massive VRAM accessible by all execution threads
- Slowest memory interface requiring coalesced access optimization
- Total available is 4095.69 MB

8. What is constant memory? How much on your GPU?
- Read-only cached memory incredibly fast for uniform parallel access
- Specifically optimized for broadcast reads across warps
- Total capacity is 64.00 KB

9. What does warp size signify? What is your GPU's warp size?
- Threads permanently group into fixed hardware execution bundles
- Warps strictly execute instructions synchronously in lockstep
- GTX 1650 uses a strict warp size of 32

10. Is double precision supported?
- Yes, supported directly by compute capability 7.5

## 4. Assignment B — Array Sum

- Parallel reduction utilizes shared memory to compute partial sums
- Configures 256 threads per block spanning ceil(1M/256) blocks
- Result validates expected 1000000.0 exactly against calculated 1000000.0
- Kernel computes entire array strictly in 0.960480 ms

## 5. Assignment C — Matrix Addition

- Matrix processes large 1024x1024 integer coordinate mapping
- Threads run 16x16 grids spanning 64x64 complete blocks
- Additions total perfectly to 1,048,576 unique floating operations
- Generates 2,097,152 discrete global memory fetch requests
- Performs exactly 1,048,576 discrete global memory record writes
- Sample logic evaluates C[0][0]=0, C[512][512]=263168, C[1023][1023]=1048575
- Kernel finishes mathematical arithmetic exactly in 0.329440 ms

## 6. Performance Analysis

| Kernel | Grid Config | Block Config | Time (ms) | Throughput |
|---|---|---|---|---|
| Array Sum | ceil(1M/256) x 1 | 256 x 1 | 0.960480 | 4.16 GB/s |
| Matrix Add | 64 x 64 | 16 x 16 | 0.329440 | 38.19 GB/s |

## 7. Key Takeaways

- CUDA exposes massive parallelism — GTX 1650 has 14 multiprocessors
- Parallel reduction is non-trivial: needs shared memory + sync to avoid race conditions
- 2D thread blocks map naturally to 2D problems like matrix operations
- Global memory is large but slow — shared memory is fast but limited to 48.00 KB/block
- Warp size of 32 means threads execute in lockstep groups — divergence kills performance
- Double precision is supported on CC 7.5 but runs slower than single precision on consumer GPUs

## 8. Conclusion
CUDA programming fundamentally demands structuring algorithms directly towards hardware capabilities.
The GTX 1650 powerfully processes mathematical parallel loads demonstrating massive bandwidth outputs.
Optimizing memory handling and strict thread boundaries dictate maximum computational efficiency.
