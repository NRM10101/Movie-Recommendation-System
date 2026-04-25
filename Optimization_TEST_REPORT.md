# Recommender System Algorithms and Computational Optimizations

## Movie Recommendation System — C Implementation

## 1. Executive Summary

This report documents profiling, baseline testing, and optimization validation for the C-based collaborative filtering movie recommender. Testing was done on both:

- `baseline_original/` (reference baseline implementation)
- optimized root implementation

The key optimization effect is reduced recommendation latency from eliminating redundant work in core loops and improving candidate selection/ranking paths while preserving the collaborative filtering flow.

### Result Snapshot

| Metric | Value |
| --- | --- |
| Baseline mean runtime (fresh, 5 runs) | 0.102 s |
| Optimized mean runtime (fresh, 10 runs) | 0.077 s |
| Average improvement | ~24.5% faster |
| Algorithm preserved | Yes (CF + Pearson + neighborhood + prediction + ranking) |

## 2. Execution Environment

| Component | Details |
| --- | --- |
| Operating System | Linux 6.8.0-85-generic |
| Shell | bash |
| Compiler | gcc |
| Dataset | `Dataset/ratings_learn.csv`, `Dataset/movies.csv`, `Dataset/movies_genres.csv` |

## 3. Profiling Tools and Methodology

### 3.1 Tools Used

| Tool | Purpose |
| --- | --- |
| `/usr/bin/time -f` / `-v` | Wall-clock and resource timing |
| `perf` | Hardware-level CPU sampling (call graph and self-time per function) |
| `gprof` | Function-level runtime profile and call costs |
| `gcc -pg` | Generates `gmon.out` for `gprof` |
| `make prof` | Convenience target to build with profiling flags (`-pg`) |

### 3.2 gprof Workflow

1. Build with profiling flags (`-pg`)
2. Run benchmark executable once to generate `gmon.out`
3. Generate profile report with `gprof`
4. Inspect flat profile and call graph

### 3.3 perf Workflow

1. Record sampled call graph:
   - `perf record -g ./bench_opt`
2. Inspect profile:
   - `perf report`

Note: on this machine, `perf` access can be blocked by kernel setting `perf_event_paranoid` and may require admin permission to collect hardware event samples.

## 4. Profiling Results and Analysis

### 4.1 Baseline (`baseline_original`) gprof highlights

From `baseline_original/gprof_base_fresh.txt`:

- `normalize_matrix`: 25.00%
- `pearson_correlation`: 18.75%
- `make_prediction`: 18.75%
- `sort`: 18.75%
- `calc_similarity`: 6.25% (+ children)

### 4.2 Optimized gprof highlights

From `gprof_opt_fresh.txt`:

- `pearson_correlation`: 27.27%
- `normalize_matrix`: 27.27%
- `calc_average`: 18.18%
- `get_utility_matrix`: 18.18%
- `make_prediction`: 9.09%

### 4.3 Root cause and optimization focus

The runtime remains concentrated in similarity and normalization paths. Main improvements came from reducing overhead around neighbor selection and data handling rather than changing recommendation semantics.

### 4.4 Proof artifacts included

- `gprof_opt_fresh.txt` — optimized profiling output
- `baseline_original/gprof_base_fresh.txt` — baseline profiling output
- `optimized_times_fresh.txt` and `baseline_original/baseline_times_fresh.txt` — fresh timing evidence

## 5. Screenshots and Evidence Attachments

Add screenshots for the following outputs in this section:

1. Optimized timing output (`/usr/bin/time -v ./bench_opt`)
2. Baseline timing output (`/usr/bin/time -v ./bench_base`)
3. Optimized gprof summary (`gprof_opt_fresh.txt`)
4. Baseline gprof summary (`baseline_original/gprof_base_fresh.txt`)
5. `optimized_times_fresh.txt` contents
6. `baseline_original/baseline_times_fresh.txt` contents

### Screenshot placeholders

- **Figure 1**: Optimized benchmark timing (`bench_opt`)
- **Figure 2**: Baseline benchmark timing (`bench_base`)
- **Figure 3**: Optimized gprof flat profile
- **Figure 4**: Baseline gprof flat profile
- **Figure 5**: Optimized repeated timing samples
- **Figure 6**: Baseline repeated timing samples

## 6. Optimizations Implemented

1. **Centroid initialization fix in baseline path**
   - Updated baseline centroid selection to use unique random indices with bounded retries.
   - Avoids repeated duplicate-pick retry loops and prevents potential stalling.

2. **Path portability updates**
   - Standardized dataset paths to Linux-style (`Dataset/...`) in baseline files.

3. **Optimized implementation (main branch) enhancements**
   - Reduced unnecessary copying/allocation in similarity path.
   - Improved ranking efficiency (non-quadratic approach).
   - Improved robustness in normalization/similarity edge cases.

## 7. Commands Executed (for reproducibility)

The following commands were executed during profiling and validation.

### 7.1 Build optimized benchmark

```bash
gcc -DBENCHMARK benchmark_main.c kmeans.c matrix_normalization.c pearsons.c predictions.c recommender.c sorting.c utility_matrix.c -lm -O2 -o bench_opt
```

### 7.2 Build baseline benchmark

```bash
cd baseline_original
gcc -DBENCHMARK benchmark_main.c kmeans.c matrix_normalization.c pearsons.c predictions.c recommender.c sorting.c utility_matrix.c -lm -O2 -o bench_base
cd ..
```

### 7.3 Collect fresh runtime samples

```bash
# optimized (10 runs)
rm -f optimized_times_fresh.txt
for i in $(seq 1 10); do /usr/bin/time -f '%e' ./bench_opt > /dev/null 2>> optimized_times_fresh.txt; done

# baseline (5 runs)
rm -f baseline_original/baseline_times_fresh.txt
for i in $(seq 1 5); do /usr/bin/time -f '%e' ./baseline_original/bench_base > /dev/null 2>> baseline_original/baseline_times_fresh.txt; done
```

### 7.4 Collect verbose timing output for screenshots

```bash
/usr/bin/time -v ./bench_opt > /dev/null
/usr/bin/time -v ./baseline_original/bench_base > /dev/null
```

### 7.5 Build and generate gprof outputs

```bash
# optimized gprof
gcc -DBENCHMARK -pg benchmark_main.c kmeans.c matrix_normalization.c pearsons.c predictions.c recommender.c sorting.c utility_matrix.c -lm -O0 -o bench_opt_pg
./bench_opt_pg > /dev/null
gprof ./bench_opt_pg gmon.out > gprof_opt_fresh.txt

# baseline gprof
cd baseline_original
gcc -DBENCHMARK -pg benchmark_main.c kmeans.c matrix_normalization.c pearsons.c predictions.c recommender.c sorting.c utility_matrix.c -lm -O0 -o bench_base_pg
./bench_base_pg > /dev/null
gprof ./bench_base_pg gmon.out > gprof_base_fresh.txt
cd ..
```

### 7.6 Optional perf commands

```bash
perf record -g ./bench_opt
perf report
```

## 8. Build and Run Instructions

### 6.1 Build baseline benchmark

```bash
cd baseline_original
gcc -DBENCHMARK benchmark_main.c kmeans.c matrix_normalization.c pearsons.c predictions.c recommender.c sorting.c utility_matrix.c -lm -O2 -o bench_base
```

### 6.2 Build optimized benchmark

```bash
cd ..
gcc -DBENCHMARK benchmark_main.c kmeans.c matrix_normalization.c pearsons.c predictions.c recommender.c sorting.c utility_matrix.c -lm -O2 -o bench_opt
```

### 6.3 Collect timings

```bash
# baseline (5 runs)
for i in $(seq 1 5); do /usr/bin/time -f '%e' ./baseline_original/bench_base > /dev/null; done

# optimized (10 runs)
for i in $(seq 1 10); do /usr/bin/time -f '%e' ./bench_opt > /dev/null; done
```

## 9. Time Measurements

### 7.1 Baseline (`baseline_original`) fresh runs

Values (s): `0.11, 0.10, 0.10, 0.10, 0.10`  
Mean: `0.102 s`

### 7.2 Optimized fresh runs

Values (s): `0.08, 0.08, 0.07, 0.08, 0.08, 0.08, 0.08, 0.08, 0.07, 0.07`  
Mean: `0.077 s`

### 7.3 Aggregated improvement

Improvement = `(0.102 - 0.077) / 0.102 * 100` = **24.5%**

## 10. Conclusion

Baseline and optimized implementations were tested side-by-side with consistent commands and controlled inputs. The optimized implementation is measurably faster (about **24.5%** lower average runtime in current measurements) while preserving the recommender pipeline behavior.

Remaining hotspots are still centered around matrix normalization and Pearson-based similarity computation, which are expected computationally dominant operations in this approach. Further gains would require deeper algorithmic or data-structure changes (e.g., sparse representations or approximate neighbor search).

---

**Algorithm preserved**: Collaborative Filtering + Pearson similarity + clustering/neighborhood + prediction + ranking  
**Measured speedup in this run**: **~24.5%**
