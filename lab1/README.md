# Lab 1: OpenMP Monte Carlo Simulation

## Introduction

This project uses a Monte Carlo simulation to estimate how often a cannonball destroys a castle when the input parameters vary within fixed ranges. Because the same computation is repeated across many random trials, the workload is a good fit for OpenMP and multicore execution.

## Scenario

A castle sits above a cliff, and an amateur group is trying to hit it with a cannon. Their estimates for distance, height, velocity, and launch angle are imprecise, so each trial uses random values from the published ranges. The goal is to estimate the probability of destroying the castle.

## Requirements

1. Use these parameter ranges:

| Variable | Meaning | Range |
| --- | --- | --- |
| `g` | Ground distance to the cliff face | `10.0` to `20.0` |
| `h` | Height of the cliff face | `20.0` to `30.0` |
| `d` | Upper-deck distance to the castle | `10.0` to `20.0` |
| `v` | Cannonball initial velocity | `20.0` to `30.0` |
| `theta` | Cannon firing angle in degrees | `70.0` to `80.0` |

2. Use these supporting values and conversions:

```c
thetaRadians = (F_PI / 180.f) * thetaDegrees
vx = v * cos(thetaRadians)
vy = v * sin(thetaRadians)
TOL = 5.0
GRAVITY = -9.8
```

`TOL` is the acceptable hit distance, and the negative sign on `GRAVITY` must remain.

3. Run multiple combinations of Monte Carlo trial counts and OpenMP thread counts. Include at least `1`, `2`, `4`, `6`, and `8` threads.
4. Time each configuration across multiple tries and keep only the peak performance.
5. Produce:

- A rectangular table of results
- A graph of performance versus trial count, with separate lines for thread count
- A graph of performance versus thread count, with separate lines for trial count

6. For one representative run, preferably the run with the largest number of trials, report the estimated probability of destroying the castle.
7. Compute the parallel fraction `Fp`.
8. Using that `Fp`, compute the maximum theoretical speedup.

## Equations

1. To find when the ball returns to ground level, solve:

```text
0 = vy * t + 0.5 * GRAVITY * t^2
```

Ignore the `t = 0` solution.

2. Horizontal distance at that time:

```text
x = vx * t
```

If `x < g`, the cannonball never reaches the cliff.

3. Time to reach the cliff face:

```text
t = g / vx
```

4. Height at the cliff face:

```text
y = vy * t + 0.5 * GRAVITY * t^2
```

If `y < h`, the cannonball hits the cliff face.

5. To find when the ball reaches the upper deck height, solve:

```text
h = vy * t + 0.5 * GRAVITY * t^2
```

6. Treat that as a quadratic equation `A * t^2 + B * t + C = 0` and use the quadratic formula.
7. Choose the larger time value because it corresponds to the downward intersection with the upper deck height.
8. Compute the upper-deck landing distance:

```text
x = vx * t
```

If `fabs(x - g - d) <= TOL`, the castle is destroyed.

## Local Files

- `proj01.cpp`: Monte Carlo OpenMP starter implementation for this lab

## Output Notes

For normal output, print:

- Number of threads
- Number of trials
- Probability of destroying the castle
- MegaTrialsPerSecond

For CSV-style output, print:

- Number of threads
- Number of trials
- MegaTrialsPerSecond

## Turn-In

Turn in your PDF file and your `cpp` file on Canvas. Go to the Canvas Week `#1` or Week `#2` modules, find Project `#1`, open the assignment page, click `Start Assignment`, and upload your files.

## Grading

| Feature | Points |
| --- | ---: |
| Close estimate of the actual probability | 10 |
| Good graph of performance vs. number of trials | 25 |
| Good graph of performance vs. number of threads | 25 |
| Compute `Fp`, the Parallel Fraction, and show your work | 20 |
| Compute `Smax`, the Maximum Speedup, and show your work | 20 |
| Potential Total | 100 |
