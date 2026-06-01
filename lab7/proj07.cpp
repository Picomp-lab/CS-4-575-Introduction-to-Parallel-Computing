// Lab 7: Distributed Data-Parallel Training with MPI
//
// Train a logistic-regression classifier on a synthetic binary-classification
// dataset using synchronous data-parallel mini-batch SGD. Each MPI rank owns
// an equal shard of the training set, computes a local gradient on every
// step, and the ranks combine their gradients with MPI_Allreduce so that all
// ranks apply the identical parameter update (mathematically equivalent to
// single-process SGD on the full batch).
//
// Build:   mpicxx -O3 -std=c++17 -o proj07 proj07.cpp
// Run:     mpiexec -np <P> ./proj07
//
// The number of features (NUM_FEATURES), training samples (NUM_SAMPLES),
// epochs (NUM_EPOCHS), learning rate (LR), and batch size (BATCH_SIZE) are
// compile-time constants. NUM_SAMPLES must be divisible by the process
// count P, and (NUM_SAMPLES / P) must be divisible by BATCH_SIZE.

#include <mpi.h>

#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <vector>

#ifndef NUM_FEATURES
#define NUM_FEATURES 32
#endif

#ifndef NUM_SAMPLES
#define NUM_SAMPLES 65536
#endif

#ifndef NUM_EPOCHS
#define NUM_EPOCHS 20
#endif

#ifndef BATCH_SIZE
#define BATCH_SIZE 64
#endif

#ifndef LR
#define LR 0.05f
#endif

static float
Sigmoid(float z)
{
	// Numerically stable sigmoid.
	if (z >= 0.0f) {
		float e = std::exp(-z);
		return 1.0f / (1.0f + e);
	}
	float e = std::exp(z);
	return e / (1.0f + e);
}

// Generate a deterministic synthetic dataset shared by every rank. Each rank
// only materializes its own shard, but uses the same global seed so the
// "ground-truth" weights and per-sample features are reproducible.
static void
GenerateShard(int rank, int nprocs,
              std::vector<float> &X, std::vector<int> &y,
              const std::vector<float> &true_w, float true_b)
{
	int per_rank = NUM_SAMPLES / nprocs;
	X.assign((size_t)per_rank * NUM_FEATURES, 0.0f);
	y.assign(per_rank, 0);

	// Linear congruential generator seeded per sample for full reproducibility
	// across any number of ranks.
	for (int i = 0; i < per_rank; i++) {
		int global_i = rank * per_rank + i;
		unsigned int s = (unsigned int)(global_i * 2654435761u + 1u);

		float z = true_b;
		for (int j = 0; j < NUM_FEATURES; j++) {
			s = s * 1103515245u + 12345u;
			// Uniform in [-1, 1].
			float xij = ((s >> 16) & 0x7fff) / 16383.5f - 1.0f;
			X[(size_t)i * NUM_FEATURES + j] = xij;
			z += true_w[j] * xij;
		}

		// 10% label noise so the problem is non-trivial.
		s = s * 1103515245u + 12345u;
		float noise = ((s >> 16) & 0x7fff) / 32767.0f;
		int label = (z > 0.0f) ? 1 : 0;
		if (noise < 0.10f)
			label = 1 - label;
		y[i] = label;
	}
}

int
main(int argc, char **argv)
{
	MPI_Init(&argc, &argv);

	int rank, nprocs;
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &nprocs);

	if (NUM_SAMPLES % nprocs != 0) {
		if (rank == 0)
			fprintf(stderr,
			        "NUM_SAMPLES (%d) must be divisible by nprocs (%d)\n",
			        NUM_SAMPLES, nprocs);
		MPI_Abort(MPI_COMM_WORLD, 1);
	}
	int per_rank = NUM_SAMPLES / nprocs;
	if (per_rank % BATCH_SIZE != 0) {
		if (rank == 0)
			fprintf(stderr,
			        "per-rank samples (%d) must be divisible by BATCH_SIZE (%d)\n",
			        per_rank, BATCH_SIZE);
		MPI_Abort(MPI_COMM_WORLD, 1);
	}

	// Build a fixed "true" weight vector so every rank generates a consistent
	// dataset.
	std::vector<float> true_w(NUM_FEATURES);
	for (int j = 0; j < NUM_FEATURES; j++)
		true_w[j] = std::sin((float)(j + 1)) * 1.5f;
	float true_b = -0.25f;

	std::vector<float> X;
	std::vector<int> y;
	GenerateShard(rank, nprocs, X, y, true_w, true_b);

	// Model parameters: weights w[0..F-1] and bias w[F]. Initialize to zero
	// so every rank starts from the same point; this means the Allreduce of
	// gradients keeps every rank's parameters in sync without an extra
	// broadcast.
	std::vector<float> w(NUM_FEATURES + 1, 0.0f);
	std::vector<float> local_grad(NUM_FEATURES + 1, 0.0f);
	std::vector<float> global_grad(NUM_FEATURES + 1, 0.0f);

	int steps_per_epoch = per_rank / BATCH_SIZE;
	double t_start = MPI_Wtime();

	for (int epoch = 0; epoch < NUM_EPOCHS; epoch++) {
		double local_loss_sum = 0.0;
		long local_correct = 0;

		for (int step = 0; step < steps_per_epoch; step++) {
			std::fill(local_grad.begin(), local_grad.end(), 0.0f);
			double batch_loss = 0.0;
			long batch_correct = 0;

			for (int b = 0; b < BATCH_SIZE; b++) {
				int i = step * BATCH_SIZE + b;
				const float *xi = &X[(size_t)i * NUM_FEATURES];

				float z = w[NUM_FEATURES];
				for (int j = 0; j < NUM_FEATURES; j++)
					z += w[j] * xi[j];

				float p = Sigmoid(z);
				float err = p - (float)y[i];

				for (int j = 0; j < NUM_FEATURES; j++)
					local_grad[j] += err * xi[j];
				local_grad[NUM_FEATURES] += err;

				// Cross-entropy loss (clamped).
				float pc = p < 1e-7f ? 1e-7f : (p > 1.0f - 1e-7f ? 1.0f - 1e-7f : p);
				batch_loss += -(y[i] * std::log(pc) + (1 - y[i]) * std::log(1.0f - pc));
				if ((p >= 0.5f) == (y[i] == 1))
					batch_correct++;
			}

			// Average gradient across the GLOBAL batch (nprocs * BATCH_SIZE
			// samples). Allreduce sums per-rank gradients; dividing by the
			// global batch size yields the mean gradient. This is the single
			// communication step per SGD update and is what makes this "data
			// parallel" training.
			MPI_Allreduce(local_grad.data(), global_grad.data(),
			              NUM_FEATURES + 1, MPI_FLOAT, MPI_SUM,
			              MPI_COMM_WORLD);

			float scale = LR / (float)(BATCH_SIZE * nprocs);
			for (int j = 0; j < NUM_FEATURES + 1; j++)
				w[j] -= scale * global_grad[j];

			local_loss_sum += batch_loss;
			local_correct += batch_correct;
		}

		// Reduce epoch-level metrics so rank 0 can report progress.
		double global_loss_sum = 0.0;
		long global_correct = 0;
		MPI_Reduce(&local_loss_sum, &global_loss_sum, 1, MPI_DOUBLE, MPI_SUM, 0,
		           MPI_COMM_WORLD);
		MPI_Reduce(&local_correct, &global_correct, 1, MPI_LONG, MPI_SUM, 0,
		           MPI_COMM_WORLD);

		if (rank == 0) {
			double avg_loss = global_loss_sum / (double)NUM_SAMPLES;
			double acc = (double)global_correct / (double)NUM_SAMPLES;
			printf("epoch %2d  loss=%.5f  acc=%.4f\n",
			       epoch + 1, avg_loss, acc);
		}
	}

	double t_end = MPI_Wtime();

	if (rank == 0) {
		double elapsed = t_end - t_start;
		double samples_per_sec =
			(double)NUM_SAMPLES * (double)NUM_EPOCHS / elapsed;
		printf("\n--- summary ---\n");
		printf("ranks            = %d\n", nprocs);
		printf("samples          = %d\n", NUM_SAMPLES);
		printf("features         = %d\n", NUM_FEATURES);
		printf("epochs           = %d\n", NUM_EPOCHS);
		printf("batch (per rank) = %d   (global batch = %d)\n",
		       BATCH_SIZE, BATCH_SIZE * nprocs);
		printf("elapsed seconds  = %.4f\n", elapsed);
		printf("samples/sec      = %.2f\n", samples_per_sec);
	}

	MPI_Finalize();
	return 0;
}
