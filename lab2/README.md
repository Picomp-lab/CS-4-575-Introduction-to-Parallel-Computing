# Lab 2: Functional Decomposition


## Introduction

The project uses OpenMP threads for simulation structure rather than raw throughput. You will build a month-by-month model of grain growth in which separate agents update the deer population, grain height, and one additional quantity of your own choosing.

## Requirements

1. Simulate these shared state variables:

| Variable | Meaning | Range |
| --- | --- | --- |
| `NowYear` | Current year | `2026` to `2031` |
| `NowMonth` | Current month | `0` to `11` |
| `NowPrecip` | Monthly precipitation | inches |
| `NowTemp` | Monthly temperature | degrees Fahrenheit |
| `NowHeight` | Grain height | inches |
| `NowNumDeer` | Deer population | count |

2. Use these constants:

```c
const float GRAIN_GROWS_PER_MONTH = 12.0;
const float ONE_DEER_EATS_PER_MONTH = 1.0;
const float AVG_PRECIP_PER_MONTH = 7.0;
const float AMP_PRECIP_PER_MONTH = 6.0;
const float RANDOM_PRECIP = 2.0;

const float AVG_TEMP = 60.0;
const float AMP_TEMP = 20.0;
const float RANDOM_TEMP = 10.0;

const float MIDTEMP = 40.0;
const float MIDPRECIP = 10.0;
```

3. Use `#pragma omp parallel sections` to run one thread each for:

- Deer population
- Grain growth
- Watcher / logger
- Your own additional agent

4. Add this global variable near the top of the program:

```c
unsigned int seed = 0;
```

5. Compute the monthly weather from the current month:

```c
float ang = (30.0f * (float)NowMonth + 15.0f) * (M_PI / 180.0f);

float temp = AVG_TEMP - AMP_TEMP * cos(ang);
NowTemp = temp + Ranf(-RANDOM_TEMP, RANDOM_TEMP);

float precip = AVG_PRECIP_PER_MONTH + AMP_PRECIP_PER_MONTH * sin(ang);
NowPrecip = precip + Ranf(-RANDOM_PRECIP, RANDOM_PRECIP);
if (NowPrecip < 0.0f)
    NowPrecip = 0.0f;
```

6. Start with:

```c
NowMonth = 0;
NowYear = 2026;
NowNumDeer = 2;
NowHeight = 5.0f;
```

7. Add one more phenomenon of your own choosing that directly or indirectly affects the grain and/or deer population.
8. Simulate six years total, stopping when the year reaches `2031`.

## Quantity Interactions

Use the current grain height as the deer carrying capacity:

```c
int nextNumDeer = NowNumDeer;
int carryingCapacity = (int)NowHeight;

if (nextNumDeer < carryingCapacity)
    nextNumDeer++;
else if (nextNumDeer > carryingCapacity)
    nextNumDeer--;

if (nextNumDeer < 0)
    nextNumDeer = 0;
```

Compute grain growth using temperature and precipitation factors:

```c
float tempFactor = exp(-SQR((NowTemp - MIDTEMP) / 10.0f));
float precipFactor = exp(-SQR((NowPrecip - MIDPRECIP) / 10.0f));

float nextHeight = NowHeight;
nextHeight += tempFactor * precipFactor * GRAIN_GROWS_PER_MONTH;
nextHeight -= (float)NowNumDeer * ONE_DEER_EATS_PER_MONTH;

if (nextHeight < 0.0f)
    nextHeight = 0.0f;
```

## Synchronization

Each agent should:

1. Compute its next value using the current global state
2. Wait at a first barrier
3. Assign its new value to the shared state
4. Wait at a second barrier
5. Let the watcher print the state, advance the month, and recompute weather
6. Wait at a third barrier

Do not use `#pragma omp barrier` in separate functions for this project. Instead, use the provided barrier helper pattern with:

- `InitBarrier(int n)`
- `WaitBarrier()`

## Local Files

- `proj02.cpp`: compile-ready starter/example implementation

The included starter uses `wolves` as the extra agent so the simulation runs end-to-end immediately. You can keep that model or replace it with your own.

## Build And Run

```bash
g++ -O2 -fopenmp proj02.cpp -o proj02
./proj02
```

## Turn-In

Turn in your PDF file and your `cpp` file on Canvas. Your write-up should include:

- A description of your added quantity and how it affects the simulation
- A table of temperature, precipitation, deer, grain height, and your extra quantity versus month
- One graph showing those values versus month
- Commentary describing the behavior you observe

## Grading

| Feature | Points |
| --- | ---: |
| Simulate grain growth and deer population | 20 |
| Simulate your own quantity | 20 |
| Table of results | 10 |
| Graph of results | 20 |
| Commentary | 30 |
| Potential Total | 100 |
