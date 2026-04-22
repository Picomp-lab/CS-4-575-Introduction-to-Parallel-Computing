# Lab 3: K-means Parallel Challenge

## Introduction

This project applies OpenMP to a simplified K-means clustering problem. The input is a list of the 331 largest cities in the contiguous United States. The goal is to regroup those cities into `NUMCAPITALS` new states by repeatedly assigning each city to its nearest capital and then recomputing each capital as the centroid of its assigned cities.

## Requirements

1. Start from the provided `proj03.cpp` and `UsCities.data` files.
2. Use the initial capital locations from uniformly spaced cities in the input list rather than random selection.
3. Repeat the K-means update for `100` iterations:

- Assign every city to the nearest capital using Pythagorean distance in longitude-latitude space
- Accumulate the longitude and latitude totals for each capital
- Replace each capital with the average longitude and latitude of its assigned cities

4. Use OpenMP to parallelize the loop over all cities.
5. Vary both `NUMT` and `NUMCAPITALS` and record performance data for multiple runs.
6. Produce:

- A rectangular table of performance data
- A graph of performance versus `NUMT`, with a separate curve for each `NUMCAPITALS`
- A graph of performance versus `NUMCAPITALS`, with a separate curve for each `NUMT`
- Commentary describing the patterns you observed

## Local Files

- `proj03.cpp`: OpenMP K-means implementation
- `UsCities.data`: city coordinate dataset included by the program

## Build And Run

Compile one configuration:

```bash
g++ proj03.cpp -o proj03 -lm -fopenmp
./proj03
```

Compile and sweep several combinations, matching the course handout style:

```bash
for t in 1 2 4 6 8
do
  for n in 2 3 4 5 10 15 20 30 40 50
  do
    g++ proj03.cpp -DNUMT=$t -DNUMCAPITALS=$n -o proj03 -lm -fopenmp
    ./proj03
  done
done
```

## Commentary Checklist

Your PDF write-up should include:

- The machine you ran on
- The operating system you used
- The compiler you used
- The full rectangular performance table
- A graph of performance vs. `NUMT`, with colored curves for `NUMCAPITALS`
- A graph of performance vs. `NUMCAPITALS`, with colored curves for `NUMT`
- A short discussion of the trends you found

## Extra Credit

The implementation also finds the actual city nearest to each final capital centroid and prints those city names when running with `NUMT=1`.

## Grading

| Feature | Points |
| --- | ---: |
| Data table | 25 |
| Graph of performance vs. number of threads | 25 |
| Graph of performance vs. number of capitals | 25 |
| Commentary | 25 |
| Extra credit: print final capital cities | 5 |
| Potential Total | 105 |
