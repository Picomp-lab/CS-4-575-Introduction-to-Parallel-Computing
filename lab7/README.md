# Lab 7: Distributed Data-Parallel Training with MPI

## Introduction

This lab introduces the parallel pattern behind modern deep-learning frameworks
(PyTorch DDP, Horovod, DeepSpeed): **synchronous data-parallel training**.
You will train a logistic-regression classifier on a synthetic binary-
classification dataset using multiple MPI processes. Each rank owns a shard of
the data, computes a local gradient on every mini-batch step, and the ranks
combine their gradients with `MPI_Allreduce` so every rank applies the
identical parameter update. The math is equivalent to single-process SGD on
the full global batch — only the work is split across ranks.

Logistic regression is intentionally simple so you can focus on the *parallel
training loop*, not on the model.

## Requirements

1. Use MPI for parallelism. Use the provided `proj07.cpp` as your starting
   point. Build with:

   ```sh
   mpicxx -O3 -std=c++17 -o proj07 proj07.cpp
   ```

2. Run with at least these process counts: `1, 2, 4, 8`. Example:

   ```sh
   mpiexec -np 4 ./proj07
   ```

3. The training loop must:
   - Initialize the model weights identically on every rank (the provided
     code uses zeros).
   - For each mini-batch step on each rank:
     1. Compute the local gradient over its `BATCH_SIZE` samples.
     2. Call `MPI_Allreduce(..., MPI_SUM, ...)` on the gradient vector.
     3. Apply an SGD update using the averaged global gradient.
   - Use `MPI_Reduce` (only to rank 0) to report per-epoch loss and accuracy.

4. The dataset is generated deterministically inside the program. Because the
   per-rank batch size is fixed, the **global** batch size grows with the
   number of ranks (`BATCH_SIZE * nprocs`), so different rank counts follow
   slightly different SGD trajectories. The final loss and accuracy should
   still be very close (within ~0.01) across rank counts — verify this as
   your correctness check. For a stricter test, override `BATCH_SIZE` so
   that `BATCH_SIZE * nprocs` is the same for every run you compare.

5. Time the training loop with `MPI_Wtime()`.

## Constants (compile-time)

| Constant | Default | Meaning |
| --- | --- | --- |
| `NUM_FEATURES` | `32` | Input dimensionality |
| `NUM_SAMPLES` | `65536` | Total training samples (must divide evenly across ranks) |
| `NUM_EPOCHS` | `20` | Passes over the dataset |
| `BATCH_SIZE` | `64` | Per-rank mini-batch size; global batch = `BATCH_SIZE * nprocs` |
| `LR` | `0.05` | Learning rate |

Override at compile time, e.g.:

```sh
mpicxx -O3 -std=c++17 -DNUM_SAMPLES=131072 -DBATCH_SIZE=128 -o proj07 proj07.cpp
```

## Deliverables

Submit a short PDF or markdown write-up containing:

1. **Correctness check.** Final-epoch loss and accuracy for `nprocs = 1, 2,
   4, 8`. They should agree within ~0.01 (see requirement 4).

2. **Performance table.** Wall-clock time and samples/sec for each `nprocs`.

3. **Speedup plot.** Speedup `S(P) = T(1) / T(P)` versus number of ranks `P`.

4. **One-paragraph discussion** answering:
   - Where is the speedup sub-linear, and why? (Hint: `MPI_Allreduce` cost,
     batch size relative to features, single-node vs. multi-node.)
   - What happens if you increase `NUM_FEATURES` to `512`? Re-run with
     `nprocs = 1` and `nprocs = 8` and report the new speedup.


## Grading Rubric (100 points)

| # | Category | Points | Criteria |
| --- | --- | --- | --- |
| 1 | **Builds & runs** | 10 | Code compiles cleanly with `mpicxx -O3 -std=c++17` and runs without crashes for `nprocs = 1, 2, 4, 8`. |
| 2 | **Correct data-parallel training loop** | 25 | Per-rank local gradient is computed correctly; `MPI_Allreduce(MPI_SUM)` is used on the gradient vector once per SGD step; update uses the averaged global gradient (divide by `BATCH_SIZE * nprocs`); initial weights are identical on all ranks. |
| 3 | **Correctness check (numerical agreement)** | 15 | Final-epoch loss and accuracy reported for `nprocs = 1, 2, 4, 8`; values agree within ~0.01. Discrepancy larger than that is explained (e.g. global batch size grew). |
| 4 | **Performance table** | 10 | Table of wall-clock time and samples/sec for `nprocs = 1, 2, 4, 8`, measured with `MPI_Wtime()` around the training loop only (not init/I/O). |
| 5 | **Speedup plot** | 10 | Plot of `S(P) = T(1) / T(P)` vs `P`. Axes labeled, legend present, ideal-linear reference line included. |
| 6 | **Discussion** | 20 | Paragraph answers all three prompts: (a) where/why speedup is sub-linear, (b) effect of `NUM_FEATURES = 512` |
| 7 | **report clarity** | 10 | Minimal modifications limited to what is needed; no dead/commented-out code; report is well-organized, results tables are readable, plot is legible. |
