# Movie-Recommendation-System

Implementation of a movie recommendation system using **collaborative filtering** in C: utility matrix, Pearson-style (centered cosine) similarity, neighbor selection, rating prediction, and top-N ranking.

This fork includes **performance optimizations** and an **assignment report** (see [ASSIGNMENT2_REPORT.md](ASSIGNMENT2_REPORT.md)). A **baseline reference** copy lives under [`baseline_original/`](baseline_original/).

## Algorithm (high level)

1. Build a user–movie utility matrix from ratings CSV.
2. Compute similarity between the active user and all other users (centered cosine / Pearson correlation on normalized vectors).
3. Select similar users (optimized build: top-similarity users; baseline: k-means on 1D similarity values).
4. Predict ratings for unseen movies via weighted collaborative filtering.
5. Rank and print top recommendations.

## Requirements

- **OS**: Linux (or any POSIX environment with `gcc`).
- **Compiler**: `gcc` with C99 support.
- **Data**: Place CSV files under `Dataset/` (see layout below). Paths are set in code to `Dataset/...` (no manual path editing needed on Linux).

## Dataset layout

```text
Dataset/
├── ratings_learn.csv
├── movies.csv
└── movies_genres.csv
```

Run all commands from the **repository root** (the directory that contains `ui.c`, `recommender.c`, and `Dataset/`).

## Build and run (interactive UI)

```bash
gcc ui.c kmeans.c matrix_normalization.c pearsons.c predictions.c recommender.c sorting.c utility_matrix.c -lm -O2 -o app
./app
```

Follow the menu: login, then choose recommendations or rate movies. The program expects to read/write `Dataset/ratings_learn.csv` relative to the current working directory.

## Build and run (benchmark / timing)

For reproducible timing without the interactive menu, use the benchmark entry point:

```bash
gcc -DBENCHMARK benchmark_main.c kmeans.c matrix_normalization.c pearsons.c predictions.c recommender.c sorting.c utility_matrix.c -lm -O2 -o bench_opt
/usr/bin/time -v ./bench_opt > /dev/null
```

Repeated samples:

```bash
for i in $(seq 1 10); do /usr/bin/time -f '%e' ./bench_opt > /dev/null; done
```

## Profiling (gprof)

```bash
gcc -DBENCHMARK -pg benchmark_main.c kmeans.c matrix_normalization.c pearsons.c predictions.c recommender.c sorting.c utility_matrix.c -lm -O0 -o bench_pg
./bench_pg > /dev/null
gprof ./bench_pg gmon.out > gprof_report.txt
```

Use `-O0` for clearer profiles; use `-O2` for release-style speed measurements.

## Baseline reference (`baseline_original/`)

To build and run the **baseline** snapshot (original-style pipeline with k-means clustering):

```bash
cd baseline_original
gcc ui.c kmeans.c matrix_normalization.c pearsons.c predictions.c recommender.c sorting.c utility_matrix.c -lm -O2 -o app_baseline
./app_baseline
```

Benchmark from repo root (paths to sources under `baseline_original/`):

```bash
gcc -DBENCHMARK baseline_original/benchmark_main.c baseline_original/kmeans.c baseline_original/matrix_normalization.c baseline_original/pearsons.c baseline_original/predictions.c baseline_original/recommender.c baseline_original/sorting.c baseline_original/utility_matrix.c -lm -O2 -o baseline_original/bench_base
./baseline_original/bench_base
```

## Original project

Based on: [shubham-bhatnagar/Movie-Recommendation-System](https://github.com/shubham-bhatnagar/Movie-Recommendation-System).

### Original README notes

- Example output: <https://imgur.com/a/dk3IY>
- Reported RMS error (original project): 0.865843

## Authors and maintainers (upstream)

1. Shubham Bhatnagar — <https://github.com/shubham-bhatnagar>  
2. Udhav Sharma — <https://github.com/UdhavSharma>

If you encounter problems with the **upstream** project, use their issue tracker. For this fork’s optimizations and report, see [ASSIGNMENT2_REPORT.md](ASSIGNMENT2_REPORT.md).
