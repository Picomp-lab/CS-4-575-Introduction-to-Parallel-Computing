# Lab 0: Simple OpenMP Experiment

This lab is based on the Oregon State CS 475/575 Project 0 assignment:
<https://web.engr.oregonstate.edu/~mjb/cs575/Projects/proj00.html>

## Goal

Measure the performance difference between running a simple array multiplication loop with:

- `NUMT=1`
- `NUMT=4`

Then compute:

- Peak performance in `MegaMults/Sec`
- The 1-thread to 4-thread speedup
- The parallel fraction `Fp`

## Assignment Summary

The program multiplies two arrays element-by-element:

```c
C[i] = A[i] * B[i];
```

The required loop must use OpenMP:

```c
#pragma omp parallel for
```

Run the same program twice:

- once with `NUMT=1`
- once with `NUMT=4`

Use `omp_get_wtime()` to time the multiplication loop and report the best observed performance.

## Files

- `proj00.cpp`: starter implementation for the assignment
- `Makefile`: build helpers for the 1-thread and 4-thread runs

## Build

```bash
make run1
make run4
```

Or build manually:

```bash
g++ -O2 -fopenmp -DNUMT=1 -DSIZE=16384 proj00.cpp -o proj00_1
g++ -O2 -fopenmp -DNUMT=4 -DSIZE=16384 proj00.cpp -o proj00_4
```

You can also try a larger array size if the threading overhead dominates on your machine:

```bash
make clean all SIZE=1000000
make run1 SIZE=1000000
make run4 SIZE=1000000
```

## Compute The Required Metrics

After running both versions, compute:

```text
speedup = performance_4_threads / performance_1_thread
Fp = (4./3.) * (1. - (1./speedup))
```

## Commentary Checklist

Your write-up should include:

- The machine used
- Performance results for 1 thread
- Performance results for 4 threads
- The 1-thread to 4-thread speedup
- Why the speedup is less than 4
- The computed parallel fraction `Fp`

## Notes

- The default `SIZE` in this lab scaffold is `16384`, matching the example scale suggested on the assignment page.
- You can override `SIZE` at compile time with `-DSIZE=...` if you want to experiment.
- If the 4-thread run is slower than the 1-thread run, try a larger `SIZE` so the loop work is large enough to overcome OpenMP overhead.
