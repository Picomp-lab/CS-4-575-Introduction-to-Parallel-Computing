# Lab 4: Project #4

## Vectorized Array Multiplication and Multiplication/Reduction using SSE

### 60 Points


This README follows the Spring Quarter 2026 project page for CS 475/575 Project #4.

## Introduction

This project compares standard C/C++ array operations against SIMD SSE code for two cases:

- array multiplication: `C[i] = A[i] * B[i]`
- multiply/reduction: `sum = Σ A[i] * B[i]`

For the baseline benchmarks, use normal C/C++ `for` loops only. Do not use OpenMP parallel loops for the non-extra-credit work. OpenMP is used only for timing.

## Requirements

1. Use the supplied SIMD SSE assembly code to time array multiplication, then run the same experiment with your own C/C++ implementation.
2. Use the supplied SIMD SSE assembly code to time array multiplication/reduction, then run the same experiment with your own C/C++ implementation.
3. Test multiple array sizes from `1K` up to `8M`. Choose enough intermediate sizes to make a useful graph.
4. Run each array-size case multiple times and record the peak performance.
5. Graph speedup, not raw performance.
6. For `C[i] = A[i] * B[i]`, create a table and graph of SSE versus non-SSE speedup as a function of array size.
7. For `sum = Σ A[i] * B[i]`, create a table and graph of SSE versus non-SSE speedup as a function of array size.
8. You can use either two separate graphs with one curve each, or one graph with two curves, but both experiment curves must be shown.
9. This is not a multithreading assignment, so do not use `NUMT`. Avoid OpenMP features other than timing.
10. The Y-axis should be Speed-Up, which is unitless. Do not use units such as operations per second on the graph axis.
11. Do not compute Parallel Fraction for this project.
12. Your PDF commentary must include:
    - the machine you ran on
    - the two performance tables and corresponding speedups
    - the graph or graphs of SIMD versus non-SIMD speedup versus array size
    - the patterns you observe in the speedups
    - whether those patterns stay consistent across array sizes
    - your explanation for why or why not

Speedup for both experiments is:

```text
S = Psse / Pnon-sse = Tnon-sse / Tsse
```

## SSE SIMD Code

- Starter file: [all04.cpp](https://github.com/Picomp-lab/CS-4-575-Introduction-to-Parallel-Computing/blob/main/lab4/all04.cpp)
- The OpenMP library is linked only because the starter uses it for timing.
- Because the code uses assembly, it is not portable. The course page specifically notes that it works on `flip` and `rabbit`, and that it does not work in Visual Studio.
- You may run tests one at a time or script them by setting the array size with a `#define` from outside the program.

## Warning

- Do not compile with optimization flags. The project page warns that optimization can interfere with register usage in this assembly-based code.
- Do not insert `printf` statements into the SSE code. That can also interfere with the register assumptions.

## Extra Credit: Combining SIMD with OpenMP

Combine SIMD and multithreading in a single test. Vary both array size and `NUMT`, show a performance table, and produce a graph similar to Slide 21 in the SIMD Vector notes. Include a short discussion of what the curves show and why you think they behave that way.

## Grading

| Feature | Points |
| --- | ---: |
| Table of array multiply performances and speedups | 10 |
| Graph of array multiply speedups | 10 |
| Table of array multiply/reduction performances and speedups | 10 |
| Graph of array multiply/reduction speedup curve | 10 |
| Commentary | 20 |
| Extra Credit | 5 |
| Potential Total | 65 |
