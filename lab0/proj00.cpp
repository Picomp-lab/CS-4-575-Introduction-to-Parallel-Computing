#include <omp.h>
#include <stdio.h>
#include <stdlib.h>

#ifndef NUMT
#define NUMT 1
#endif

#ifndef SIZE
#define SIZE 16384
#endif

#define NUMTRIES 20

static float A[SIZE];
static float B[SIZE];
static float C[SIZE];

int
main()
{
#ifdef _OPENMP
    fprintf(stderr, "OpenMP version %d is supported here\n", _OPENMP);
#else
    fprintf(stderr, "OpenMP is not supported here - sorry!\n");
    return 1;
#endif

    for (int i = 0; i < SIZE; i++)
    {
        A[i] = 1.0f;
        B[i] = 2.0f;
    }

    omp_set_num_threads(NUMT);

    double maxMegaMults = 0.0;
    for (int t = 0; t < NUMTRIES; t++)
    {
        double time0 = omp_get_wtime();

#pragma omp parallel for
        for (int i = 0; i < SIZE; i++)
        {
            C[i] = A[i] * B[i];
        }

        double time1 = omp_get_wtime();
        double megaMults = (double)SIZE / (time1 - time0) / 1000000.0;
        if (megaMults > maxMegaMults)
        {
            maxMegaMults = megaMults;
        }
    }

    fprintf(stderr, "SIZE = %d, NUMTRIES = %d\n", SIZE, NUMTRIES);
    fprintf(
        stderr,
        "For %d threads, Peak Performance = %8.2lf MegaMults/Sec\n",
        NUMT,
        maxMegaMults
    );

    return 0;
}
