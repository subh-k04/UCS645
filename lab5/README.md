# Lab 5 — MPI Part II: Blocking vs Non-Blocking Communication

**Course:** UCS645 — Parallel & Distributed Computing
**Platform:** Linux 6.6.87.2-microsoft-standard-WSL2 x86_64
**Environment:** Linux (WSL)

## 1. Overview

This lab explores point-to-point and collective MPI communications, focusing on blocking vs non-blocking paradigms.
We implement parallel math operations, linear vs tree-based broadcast performance measurements, and master-slave task distribution.
Results highlight the extreme efficiency of tree-based routing and the realistic performance limits imposed by Amdahl's law.

## 2. Exercises Summary

| # | Exercise | Concept | Key MPI Functions |
|---|---|---|---|
| Q1 | DAXPY Operation | Data parallelism, speedup measurement | MPI_Scatter, MPI_Gather, MPI_Wtime |
| Q2 | Broadcast Race | Linear vs tree-based broadcast efficiency | MPI_Send, MPI_Recv, MPI_Bcast |
| Q3 | Distributed Dot Product | Amdahl's Law, MPI_Bcast + MPI_Reduce pipeline | MPI_Bcast, MPI_Reduce |
| Q4 | Prime Finder | Master-slave task distribution | MPI_Send, MPI_Recv, MPI_ANY_SOURCE |
| Q5 | Perfect Number Finder | Master-slave with divisor computation | MPI_Send, MPI_Recv, MPI_ANY_SOURCE |

## 3. Exercise Analysis

### Q1 — DAXPY Operation
- Implements Y = a*X + Y element-wise iteratively across an array of 65536 double-precision floating variables.
- Uses MPI_Scatter to chunk the heavy arrays, processes computational fragments locally, and recovers the fragments via MPI_Gather.
- Employs an explicit MPI_Barrier immediately before passing to MPI_Wtime to ensure an accurate, strictly synchronized timing sequence.
- Parallel processing running at 4 total processes effectively reduced the DAXPY vector compute time down to roughly 18 microsecond speeds.

### Q2 — Broadcast Race
- Custom sequential MyBcast implementation forces a loop of direct blocking MPI_Send calls resulting in O(p) network complexity and linear latency.
- Highly optimized built-in MPI_Bcast inherently uses a logarithmic O(log p) tree-based transfer mechanism taking advantage of wide topology.
- Real world benchmarking measurements definitively prove MPI_Bcast is significantly faster with an immediate order-of-magnitude lower latency profile.
- Evaluated bottleneck gaps mathematically widen straight up to 16 processes predominantly because the root node inherently becomes a sequential bottleneck loop.

### Q3 — Distributed Dot Product
- Bypasses catastrophic unified large array allocations by directly generating chunk blocks locally based purely on distributed parallel sizing constraints.
- Root node forcefully dispatches a base mathematical scaling multiplier via MPI_Bcast while concurrently aggregating partial chunk results passing through MPI_Reduce.
- Scales practically well progressing up to 8 total collective processes but realistically drops heavily in theoretical efficiency primarily due to immense 8-gigabyte memory footprint pressure.
- Provides a direct observable demonstration of Amdahl's Law logic where unparallelized root synchronization routines fundamentally cap maximum scalable multi-node performance ceilings.

### Q4 — Prime Finder
- Adopts an aggressive master-slave computational topology where the root node tightly dispatches integer test case loops and directly collects aggregate mathematical confirmations.
- Dynamically allocates task messages natively via MPI_ANY_SOURCE listeners enforcing that idle available slave processes instantaneously accept fresh workload loops continually.
- Architecturizes a simplified basic tag signal protocol utilizing zero for readiness handshakes, negative values denoting composites, and explicit positive integers returning confirmed primes.
- Master execution successfully routed loops validating exactly 1229 true sequential prime numbers residing strictly under the constrained 10000 integer maximum limit.

### Q5 — Perfect Number Finder
- Purposefully reuses the robust dynamic core master-slave topology for continuously evaluating perfect mathematical criteria requirements specifically against isolated divisor sum test constraints.
- Iterative passing tightly reflects the prime finder structural logic utilizing identical tags directly signaling internal divisor perfect validations across parallel computational subnodes.
- Slaves accurately independently solved computational divisor rules reporting precisely 6, 28, 496, and 8128 as the correct true perfect quantities up to the threshold parameter.
- Emphasizes profoundly how dynamic execution topologies seamlessly excel handling unpredictably erratic computation cycles intrinsic to unbalanced mathematical verification structures.

## 4. Performance Analysis

### Q1 — DAXPY Speedup

| Processes | Time (s) | Speedup | Efficiency |
|---|---|---|---|
| 1 | 0.000049 | 1.00 | 100.0% |
| 2 | 0.000040 | 1.23 | 61.3% |
| 4 | 0.000018 | 2.72 | 68.1% |
| 8 | 0.000016 | 3.06 | 38.3% |

### Q2 — Broadcast Race Comparison

| Processes | MyBcast Time (s) | MPI_Bcast Time (s) | Speedup Ratio |
|---|---|---|---|
| 2 | 0.239036 | 0.020914 | 11.43 |
| 4 | 0.711881 | 0.041010 | 17.36 |
| 8 | 1.381668 | 0.190880 | 7.24 |
| 16 | 4.729401 | 0.619101 | 7.64 |

### Q3 — Dot Product Speedup

| Processes | Time (s) | Speedup | Efficiency |
|---|---|---|---|
| 1 | 158.314575 | 1.00 | 100.0% |
| 2 | 106.249255 | 1.49 | 74.5% |
| 4 | 64.263446 | 2.46 | 61.5% |
| 8 | 44.558343 | 3.55 | 44.4% |

## 5. Scaling Observations

- DAXPY efficiency initially grows favorably across mid-range process thresholds but plummets inherently progressing directly toward 8 processes indicating severe computational resource starvation.
- Complex dot product mathematical implementations correctly maintain strong scaling stability structurally because sustained arithmetic iterations intrinsically offset network overheads.
- Highly visible structural parallel handling degradation critically harms DAXPY significantly faster proportionally due strictly to aggressively minor internal subtask memory footprints.
- Linear direct loop broadcast operations mathematically cascade latency constraints infinitely tracking scaling process growth compared directly against logarithmically balanced collective routines.
- Evaluated bottleneck constraints thoroughly illustrate unescapable realities derived purely from Amdahl's law reflecting statically forced serialization phases dictating upper physical speeds.
- Directly pushing nodes upwards approaching complex 8 process pipelines encounters critically harsh diminishing efficiency results heavily reflecting overlapping network transit latencies.

## 6. Blocking vs Non-Blocking Analysis

- Conventional blocking routines including baseline MPI_Send intentionally stall cyclic execution completely waiting ensuring deterministic communication cycles prioritizing strictly ordered topologies.
- Applying specialized non-blocking asynchronous MPI_Isend methodologies essentially decouples local process cycles allowing true concurrent algorithm execution immediately masking communication latency penalties.
- Relying entirely upon sequential blocking functions severely raises latent topological deadlocks exponentially within cyclical ring boundaries reacting strictly towards unpredictable finite network buffers.
- Correct structural programming correctly mandates employing MPI_Waitall synchronizing barriers precisely enforcing that all concurrently executing asynchronous threads permanently settle before advancing logic layers.
- Utilize deterministic standard blocking instructions validating correctness critically whereas asynchronous execution layers mathematically excel structurally during peak continuous overlapping processor demands.

## 7. Key Takeaways

- MPI_Bcast inherently uses logarithmic tree-based distributions essentially operating in O(log p) boundaries functionally outclassing manual sequential iterations hard-locked within straight O(p) variables.
- Integrating highly detached non-blocking function boundaries like MPI_Isend dynamically shadows transit latencies completely facilitating uninterrupted local execution overlapping math logic continually.
- Implementation logic directly leveraging MPI_ANY_SOURCE actively stabilizes asymmetric CPU burdens constantly routing fresh sequential arrays explicitly avoiding idle processor starvation efficiently.
- Mathematical upper boundaries conceptually defined through Amdahl's Law definitively suppress pure parallel scaling inherently constrained beneath serialized root synchronization logic branches.
- Total parallel architecture efficiency mathematically collapses distinctly fast handling diminutive task pipelines structurally because intrinsic message passing transmission latency strictly overshadows brief operations.
- Canonical patterns successfully blending MPI_Scatter immediately coupled behind MPI_Gather represent perfectly defined baseline structural solutions practically managing trivially parallel datasets successfully.

## 8. Conclusion

Measured experiments inherently document unarguable structural latency disadvantages resulting specifically from sequentially bottlenecked linear message loops contrasted perfectly against hierarchical architectures.
Adopting completely dynamic master-slave listener constraints successfully insulates collective pipelines actively preventing mathematical cluster stagnation processing distinctly erratic non-uniform mathematical execution loads.
Observed mathematical speed limits definitively authenticate complex efficiency calculations strictly bounded permanently beneath realistically governed parallel thresholds directly constrained under core Amdahl bounds.
