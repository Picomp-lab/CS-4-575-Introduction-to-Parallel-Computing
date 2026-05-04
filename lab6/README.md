# Lab 6

## Fourier Analysis using MPI

### Based on CS 475/575 Project #7

### 120 Points

### Due: June 9, 2025 at 23:59

Source page: [CS 475/575 Project #7](https://web.engr.oregonstate.edu/~mjb/cs575/Projects/proj07.html)

This README follows the Spring Quarter 2025 Fourier Analysis using MPI page, which is labeled Project #7 on the course site and was last updated on June 6, 2025.

## Notes

- This is a CPU cluster project.
- The `flip` machines are not a cluster, so MPI will not run there.
- You need to use the `submit-*` machines.
- This is not a pivot-table project because the only benchmark input is the number of MPI processors.
- Pro tip from the course page: check your `MPI_Send` and `MPI_Recv` calls carefully. The number of items received should match the number sent.

## Introduction

Fourier analysis is used to determine whether a signal contains hidden periodic structure. In this project, you will take a large noisy-looking signal, compute Fourier sums across many candidate periods, identify the embedded sine-wave periods, and compare performance across different MPI processor counts.

## What The Fourier Sums Mean

The idea is to multiply the signal by a sine wave of one period, sum the products, and repeat for many periods. If the signal is truly random, the positive and negative products mostly cancel out. But if a hidden sine wave exists at a matching period, the corresponding Fourier sum becomes much larger than zero and stands out clearly in the plot.

## Requirements

1. Read one of the provided signal files:
   - `bigsignal.txt`
   - `bigsignal.bin`
2. Each file contains `1,048,576` signal amplitudes.
3. Verify that your download is complete using these file sizes:

| File | Size in bytes |
| --- | ---: |
| `bigsignal.bin` | `4194304` |
| `bigsignal.txt` | `9437184` |

4. The non-parallel C/C++ structure for the computation is:

```cpp
BigSums[0] = 0.;
for( int p = 1; p < MAXPERIODS; p++ )
{
    BigSums[p] = 0.;
    float omega = F_2_PI / (float)p;
    for( int t = 0; t < NUMELEMENTS; t++ )
    {
        BigSums[p] += BigSignal[t] * sin( omega*(float)t );
    }
}
```

5. Implement this computation using MPI parallelism.
6. Each processor is responsible for Fourier-transforming `NUMELEMENTS / NumCpus` elements.
7. Scatterplot the `MAXPERIODS` Fourier `Sums[*]` versus period.
8. There are two secret sine waves embedded in the signal. The scatterplot will show two obvious spikes.
9. Report what you think the two secret sine-wave periods are.
10. Draw a graph showing performance versus number of MPI processors used.
11. Use appropriate performance units and make faster performance go upward on the graph.
12. Turn in:
   - your source code file (`.cpp`)
   - your PDF commentary

## PDF Commentary

Your PDF should include:

1. The `Sums[*]` versus period scatterplot
2. The two secret sine-wave periods
3. Your graph of performance versus number of processors used
4. The patterns you see in the performance graph
5. Why you think the performance behaves that way

## Starter Files

- Skeleton code: [proj07.cpp](https://web.engr.oregonstate.edu/~mjb/cs575/Projects/proj07.cpp)
- Signal data: [bigsignal.txt](https://web.engr.oregonstate.edu/~mjb/cs575/Projects/bigsignal.txt)
- Signal data: [bigsignal.bin](https://web.engr.oregonstate.edu/~mjb/cs575/Projects/bigsignal.bin)

## Grading

| Feature | Points |
| --- | ---: |
| Fourier `Sums[*]` vs. period graph | 30 |
| Report the two correct sine-wave periods | 30 |
| Performance graph using at least `1`, `2`, `4`, `6`, `8` processor counts | 30 |
| Commentary in the PDF file | 30 |
| Potential Total | 120 |
