# Lab 0: Simple OpenMP Experiment

## Introduction

A great use for parallel programming is identical operations on large arrays of numbers.

## Requirements

1. Pick an array size to do the arithmetic on. Something like `16384` (16K) will do. Do not pick something too huge, because your machine may not allow you to use that much memory. Do not pick something too small, because the overhead of using threading might dominate the parallelism gains.
2. Using OpenMP, pairwise multiply two large floating-point arrays, putting the results in another array. Do this in a `for` loop. Be sure to use `#pragma omp parallel for` as the line before the loop.
3. Pairwise multiplication means:

```c
C[i] = A[i] * B[i];
```

4. Do this twice, once for one thread and once for four threads:

```c
#define NUMT 1
#define NUMT 4
```

5. Use `omp_set_num_threads(NUMT);` to set the number of threads to use.
6. Time the two runs using two calls to `omp_get_wtime();`. Convert the timing results into `Mega-Multiplies per Second`.
7. Compute the speedup `S` when moving from 1 thread to 4 threads:

```text
S = (Execution time with one thread) / (Execution time with four threads)
  = (Performance with four threads) / (Performance with one thread)
```

This number should be greater than `1.0`.

8. If your 1-thread-to-4-thread speedup is `S`, compute the parallel fraction with:

```c
float Fp = (4./3.) * (1. - (1./S));
```

Use only the 1-thread-to-4-thread speedup in this equation.

9. Your written commentary, turned in as a PDF file, should include:

- What machine you ran this on
- What performance results you got
- What your 1-thread-to-4-thread speedup was
- Why your 1-thread-to-4-thread speedup is less than `4.0`
- What your parallel fraction `Fp` was

## Local Files

- `proj00.cpp`: starter program from the assignment page

## Turn-In

Turn in your PDF file and your `cpp` file on Canvas. Go to the Canvas Week `#1` or `#2` modules, scroll down to the Projects, go to the Project `#0` row, and click `Submit`. When you get the Project `#0` Assignment page, click the `Start Assignment` button in the upper-right corner and upload your files.

## Grading

| Feature | Points |
| --- | ---: |
| Performance results for 1 thread | 5 |
| Performance results for 4 threads | 5 |
| One-thread-to-four-threads Speedup (`> 1.`) | 5 |
| Parallel Fraction | 10 |
| Commentary | 5 |
| Potential Total | 30 |

<!-- ## Build And Run

```bash
g++ -O2 -fopenmp -DNUMT=1 -DSIZE=16384 proj00.cpp -o proj00_1
g++ -O2 -fopenmp -DNUMT=4 -DSIZE=16384 proj00.cpp -o proj00_4
./proj00_1
./proj00_4
```

If you want to experiment with a larger array size:

```bash
g++ -O2 -fopenmp -DNUMT=1 -DSIZE=1000000 proj00.cpp -o proj00_1
g++ -O2 -fopenmp -DNUMT=4 -DSIZE=1000000 proj00.cpp -o proj00_4
./proj00_1
./proj00_4
``` -->

