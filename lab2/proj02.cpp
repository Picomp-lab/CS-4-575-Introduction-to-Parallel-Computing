#include <math.h>
#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

const float GRAIN_GROWS_PER_MONTH = 12.0f;
const float ONE_DEER_EATS_PER_MONTH = 1.0f;
const float AVG_PRECIP_PER_MONTH = 7.0f;
const float AMP_PRECIP_PER_MONTH = 6.0f;
const float RANDOM_PRECIP = 2.0f;

const float AVG_TEMP = 60.0f;
const float AMP_TEMP = 20.0f;
const float RANDOM_TEMP = 10.0f;

const float MIDTEMP = 40.0f;
const float MIDPRECIP = 10.0f;

const float PI = 3.14159265358979323846f;
const int START_YEAR = 2025;
const int END_YEAR = 2031;
const int NUM_SECTIONS = 4;

// Shared simulation state for the current month.
int NowYear;
int NowMonth;
float NowPrecip;
float NowTemp;
float NowHeight;
int NowNumDeer;
int NowNumWolves;

unsigned int seed = 0;

// Custom reusable barrier state required by the assignment.
omp_lock_t Lock;
volatile int NumInThreadTeam;
volatile int NumAtBarrier;
volatile int NumGone;

float Ranf(float low, float high);
float SQR(float x);
void Grain();
void Deer();
void Watcher();
void Wolves();
void UpdateEnvironment();
void InitBarrier(int n);
void WaitBarrier();

int
main()
{
#ifndef _OPENMP
	fprintf(stderr, "OpenMP is not supported here.\n");
	return 1;
#endif

	seed = (unsigned int)time(NULL);

	NowMonth = 0;
	NowYear = START_YEAR;
	NowNumDeer = 2;
	NowHeight = 5.0f;
	NowNumWolves = 1;

	UpdateEnvironment();

	// CSV output makes it easy to graph the month-by-month results later.
	printf("Month,Year,TempF,PrecipIn,GrainHeightIn,NumDeer,NumWolves\n");

	omp_set_num_threads(NUM_SECTIONS);
	InitBarrier(NUM_SECTIONS);

#pragma omp parallel sections
	{
#pragma omp section
		{ Deer(); }

#pragma omp section
		{ Grain(); }

#pragma omp section
		{ Watcher(); }

#pragma omp section
		{ Wolves(); }
	}

	return 0;
}

void
Grain()
{
	while (NowYear < END_YEAR)
	{
		// Grain grows best near the ideal temperature and precipitation.
		float tempFactor = expf(-SQR((NowTemp - MIDTEMP) / 10.0f));
		float precipFactor = expf(-SQR((NowPrecip - MIDPRECIP) / 10.0f));

		float nextHeight = NowHeight;
		nextHeight += tempFactor * precipFactor * GRAIN_GROWS_PER_MONTH;
		nextHeight -= (float)NowNumDeer * ONE_DEER_EATS_PER_MONTH;
		if (nextHeight < 0.0f)
			nextHeight = 0.0f;

		// Phase 1: finish computing next values.
		WaitBarrier();
		// Phase 2: publish this agent's next value.
		NowHeight = nextHeight;
		WaitBarrier();
		// Phase 3: wait for the watcher to print and advance time.
		WaitBarrier();
	}
}

void
Deer()
{
	while (NowYear < END_YEAR)
	{
		// Deer move toward the carrying capacity set by the grain height.
		int nextNumDeer = NowNumDeer;
		int carryingCapacity = (int)NowHeight;

		if (nextNumDeer < carryingCapacity)
			nextNumDeer++;
		else if (nextNumDeer > carryingCapacity)
			nextNumDeer--;

		nextNumDeer -= NowNumWolves / 2;
		if (nextNumDeer < 0)
			nextNumDeer = 0;

		WaitBarrier();
		NowNumDeer = nextNumDeer;
		WaitBarrier();
		WaitBarrier();
	}
}

void
Wolves()
{
	while (NowYear < END_YEAR)
	{
		// Wolves increase when there is plenty of prey and decline otherwise.
		int nextNumWolves = NowNumWolves;

		if (NowNumDeer > 2 * NowNumWolves)
			nextNumWolves++;
		else if (NowNumDeer < NowNumWolves)
			nextNumWolves--;

		if (nextNumWolves < 0)
			nextNumWolves = 0;

		WaitBarrier();
		NowNumWolves = nextNumWolves;
		WaitBarrier();
		WaitBarrier();
	}
}

void
Watcher()
{
	while (NowYear < END_YEAR)
	{
		// Wait until the other agents have computed and assigned this month.
		WaitBarrier();
		WaitBarrier();

		printf("%d,%d,%.2f,%.2f,%.2f,%d,%d\n",
			NowMonth + 1, NowYear, NowTemp, NowPrecip, NowHeight, NowNumDeer, NowNumWolves);

		// The watcher is the only thread that advances time and weather.
		NowMonth++;
		if (NowMonth >= 12)
		{
			NowMonth = 0;
			NowYear++;
		}

		if (NowYear < END_YEAR)
			UpdateEnvironment();

		WaitBarrier();
	}
}

void
UpdateEnvironment()
{
	// Seasonal temperature and precipitation follow sine/cosine waves with noise.
	float ang = (30.0f * (float)NowMonth + 15.0f) * (PI / 180.0f);

	float temp = AVG_TEMP - AMP_TEMP * cosf(ang);
	NowTemp = temp + Ranf(-RANDOM_TEMP, RANDOM_TEMP);

	float precip = AVG_PRECIP_PER_MONTH + AMP_PRECIP_PER_MONTH * sinf(ang);
	NowPrecip = precip + Ranf(-RANDOM_PRECIP, RANDOM_PRECIP);
	if (NowPrecip < 0.0f)
		NowPrecip = 0.0f;
}

float
Ranf(float low, float high)
{
	float r = (float)rand_r(&seed);
	return low + r * (high - low) / (float)RAND_MAX;
}

float
SQR(float x)
{
	return x * x;
}

void
InitBarrier(int n)
{
	// Register how many threads must arrive before the barrier releases.
	NumInThreadTeam = n;
	NumAtBarrier = 0;
	NumGone = 0;
	omp_init_lock(&Lock);
}

void
WaitBarrier()
{
	// Count this thread's arrival and let the last thread release the group.
	omp_set_lock(&Lock);
	{
		NumAtBarrier++;
		if (NumAtBarrier == NumInThreadTeam)
		{
			NumGone = 0;
			NumAtBarrier = 0;
			while (NumGone != NumInThreadTeam - 1)
				;
			omp_unset_lock(&Lock);
			return;
		}
	}
	omp_unset_lock(&Lock);

	while (NumAtBarrier != 0)
		;

#pragma omp atomic
	NumGone++;
}
