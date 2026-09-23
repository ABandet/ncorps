#include <chrono>
#include <cmath>
#include <concepts>
#include <cstddef>
#include <iostream>
#include <utility>
#include <vector>

#include "api/concepts.hpp"
#include "datatype/bodies.hpp"
#include "impl/force_model.hpp"
#include "impl/time_integration.hpp"

constexpr double DT = 0.001;
constexpr size_t N = 1000;
constexpr int NB_ITER = 100;

using namespace ncorps;

template <ForceModelC FM, TimeIntegrator TI>
void run_seq(Bodies &b, double dt, int nb_iter) {
    std::vector<double> fx(N), fy(N), fz(N);
    for (int t = 0; t < NB_ITER; t++) {
        // update force for n
        for (size_t i = 0; i < N; i++) {
            FM::step(b, i, DT, fx[i], fy[i], fz[i]);
        }
        // update position for n+1
        for (size_t i = 0; i < N; i++) {
            TI::step(b, i, DT, fx[i], fy[i], fz[i]);
        }
    }
}

template <ForceModelC FM, TimeIntegrator TI>
void run_omp(Bodies &b, double dt, int nb_iter) {
    std::vector<double> fx(N), fy(N), fz(N);
#pragma omp parallel for
    for (int t = 0; t < NB_ITER; t++) {
        // update force for n
        for (size_t i = 0; i < N; i++) {
            FM::step(b, i, DT, fx[i], fy[i], fz[i]);
        }
        // update position for n+1
#pragma omp parallel for
        for (size_t i = 0; i < N; i++) {
            TI::step(b, i, DT, fx[i], fy[i], fz[i]);
        }
    }
}

struct BenchResult {
    std::chrono::milliseconds duration;
    double energy_diff;
};

template <std::invocable Func>
BenchResult run_benchmark(Bodies &b, Func &&func) {
    const auto init_nrj = b.get_system_energy();
    auto start = std::chrono::high_resolution_clock::now();

    std::forward<Func>(func)();

    auto end = std::chrono::high_resolution_clock::now();
    const auto end_nrj = b.get_system_energy();

    auto duration =
        std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

    return {duration, end_nrj - init_nrj};
}

int main(void) {
    std::cout << "Running simulation on " << NB_ITER << " time iteration\n";
    std::cout << "With " << N << " bodies\n";

    auto start_total = std::chrono::high_resolution_clock::now();

    Bodies b_seq(N);
    auto res_seq = run_benchmark(b_seq, [&]() {
        run_seq<ForceModel, EulerExplicit>(b_seq, DT, NB_ITER);
    });

    Bodies b_omp(N);
    auto res_omp = run_benchmark(b_omp, [&]() {
        run_omp<ForceModel, EulerExplicit>(b_omp, DT, NB_ITER);
    });

    auto end_total = std::chrono::high_resolution_clock::now();
    auto total_duration = std::chrono::duration_cast<std::chrono::milliseconds>(
        end_total - start_total);

    std::cout << "Total Execution Time: " << total_duration.count() << " ms\n";
    std::cout << "SEQ/EX Time: " << res_seq.duration.count() << " ms\n";
    std::cout << "OMP/EX Time: " << res_omp.duration.count() << " ms\n";
    std::cout << "SEQ/EX energy difference: " << res_seq.energy_diff
              << " Joules\n";
    std::cout << "OMP/EX energy difference: " << res_omp.energy_diff
              << " Joules\n";

    return 0;
}
