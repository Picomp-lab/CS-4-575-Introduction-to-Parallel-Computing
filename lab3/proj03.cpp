#include <stdio.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include <omp.h>
#include <string>

// setting the number of threads:
#ifndef NUMT
#define NUMT 2
#endif

// setting the number of capitals we want to try:
#ifndef NUMCAPITALS
#define NUMCAPITALS 5
#endif

// maximum iterations to allow looking for convergence:
#define MAXITERATIONS 100

// how many tries to discover the maximum performance:
#define NUMTRIES 30

#define CSV

struct city
{
	std::string name;
	float longitude;
	float latitude;
	int capitalnumber;
	float mindistance;
};

#include "UsCities.data"

// setting the number of cities we want to try:
#define NUMCITIES (sizeof(Cities) / sizeof(struct city))

struct capital
{
	std::string name;
	float longitude;
	float latitude;
	float longsum;
	float latsum;
	int numsum;
};

struct capital Capitals[NUMCAPITALS];

float
Distance(int cityIndex, int capitalIndex)
{
	float dx = Cities[cityIndex].longitude - Capitals[capitalIndex].longitude;
	float dy = Cities[cityIndex].latitude - Capitals[capitalIndex].latitude;
	return sqrtf(dx * dx + dy * dy);
}

void
SeedCapitals()
{
	if (NUMCAPITALS == 1)
	{
		Capitals[0].longitude = Cities[0].longitude;
		Capitals[0].latitude = Cities[0].latitude;
		return;
	}

	for (int k = 0; k < NUMCAPITALS; k++)
	{
		int cityIndex = k * (NUMCITIES - 1) / (NUMCAPITALS - 1);
		Capitals[k].longitude = Cities[cityIndex].longitude;
		Capitals[k].latitude = Cities[cityIndex].latitude;
		Capitals[k].name.clear();
	}
}

void
AssignNearestCitiesAndAccumulate()
{
#pragma omp parallel for default(none) shared(Cities, Capitals)
	for (int i = 0; i < NUMCITIES; i++)
	{
		int capitalnumber = -1;
		float mindistance = 1.e+37f;

		for (int k = 0; k < NUMCAPITALS; k++)
		{
			float dist = Distance(i, k);
			if (dist < mindistance)
			{
				mindistance = dist;
				capitalnumber = k;
			}
		}

		Cities[i].mindistance = mindistance;
		Cities[i].capitalnumber = capitalnumber;

		int k = capitalnumber;
#pragma omp critical
		{
			Capitals[k].longsum += Cities[i].longitude;
			Capitals[k].latsum += Cities[i].latitude;
			Capitals[k].numsum++;
		}
	}
}

void
UpdateCapitalCentroids()
{
	for (int k = 0; k < NUMCAPITALS; k++)
	{
		if (Capitals[k].numsum > 0)
		{
			Capitals[k].longitude = Capitals[k].longsum / (float)Capitals[k].numsum;
			Capitals[k].latitude = Capitals[k].latsum / (float)Capitals[k].numsum;
		}
	}
}

void
FindClosestNamedCities()
{
	for (int k = 0; k < NUMCAPITALS; k++)
	{
		int closestCity = 0;
		float mindistance = 1.e+37f;

		for (int i = 0; i < NUMCITIES; i++)
		{
			float dx = Cities[i].longitude - Capitals[k].longitude;
			float dy = Cities[i].latitude - Capitals[k].latitude;
			float dist = sqrtf(dx * dx + dy * dy);
			if (dist < mindistance)
			{
				mindistance = dist;
				closestCity = i;
			}
		}

		Capitals[k].name = Cities[closestCity].name;
	}
}

int
main(int argc, char *argv[])
{
#ifdef _OPENMP
	fprintf(stderr, "OpenMP is supported -- version = %d\n", _OPENMP);
#else
	fprintf(stderr, "No OpenMP support!\n");
	return 1;
#endif

	omp_set_num_threads(NUMT);

	double maxPerformance = 0.;

	for (int t = 0; t < NUMTRIES; t++)
	{
		SeedCapitals();

		for (int i = 0; i < NUMCITIES; i++)
		{
			Cities[i].capitalnumber = -1;
			Cities[i].mindistance = 1.e+37f;
		}

		double time0 = omp_get_wtime();

		for (int n = 0; n < MAXITERATIONS; n++)
		{
			for (int k = 0; k < NUMCAPITALS; k++)
			{
				Capitals[k].longsum = 0.;
				Capitals[k].latsum = 0.;
				Capitals[k].numsum = 0;
			}

			AssignNearestCitiesAndAccumulate();
			UpdateCapitalCentroids();
		}

		double time1 = omp_get_wtime();
		double megaCityCapitalsPerSecond =
			(double)NUMCITIES * (double)NUMCAPITALS * (double)MAXITERATIONS /
			(time1 - time0) / 1000000.;

		if (megaCityCapitalsPerSecond > maxPerformance)
			maxPerformance = megaCityCapitalsPerSecond;
	}

	FindClosestNamedCities();

	if (NUMT == 1)
	{
		for (int k = 0; k < NUMCAPITALS; k++)
		{
			fprintf(stderr, "\t%3d:  %8.2f , %8.2f , %s\n",
				k,
				Capitals[k].longitude,
				Capitals[k].latitude,
				Capitals[k].name.c_str());
		}
	}

#ifdef CSV
	fprintf(stderr, "%2d , %4d , %4d , %8.3lf\n", NUMT, NUMCITIES, NUMCAPITALS, maxPerformance);
#else
	fprintf(stderr,
		"%2d threads : %4d cities ; %4d capitals ; megacity-capitals/sec = %8.3lf\n",
		NUMT,
		NUMCITIES,
		NUMCAPITALS,
		maxPerformance);
#endif

	return 0;
}
