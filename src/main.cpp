#include <chrono>
#include <concepts>
#include <cstddef>
#include <iostream>
#include <utility>

#include <omp.h>

#include "api/concepts.hpp"
#include "datatype/bodies.hpp"
#include "datatype/forces.hpp"
#include "impl/force_model.hpp"
#include "impl/time_integration.hpp"

constexpr double DT = 0.001;
constexpr size_t N = 1000;
constexpr int NB_ITER = 100;

using namespace ncorps;

template <ForceModelC FM, TimeIntegrator TI>
void run(Bodies &b, const double dt, const int nb_iter) {
    Forces f(b.m_n);
    for (int t = 0; t < nb_iter; t++) {
        FM::step_all(b, f);
        TI::step_all(b, f, dt);
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
    const int max_threads = omp_get_max_threads();

    omp_set_num_threads(1);

    Bodies b_seq_esi(N);
    auto res_seq_esi = run_benchmark(b_seq_esi, [&]() {
        run<ForceModel, EulerSemiImplicit>(b_seq_esi, DT, NB_ITER);
    });

    Bodies b_seq(N);
    auto res_seq = run_benchmark(
        b_seq, [&]() { run<ForceModel, EulerExplicit>(b_seq, DT, NB_ITER); });

    omp_set_num_threads(max_threads);

    Bodies b_omp(N);
    auto res_omp = run_benchmark(b_omp, [&]() {
        run<ForceModel, EulerSemiImplicit>(b_omp, DT, NB_ITER);
    });

    auto end_total = std::chrono::high_resolution_clock::now();
    auto total_duration = std::chrono::duration_cast<std::chrono::milliseconds>(
        end_total - start_total);

    std::cout << "Total Execution Time: " << total_duration.count() << " ms\n";
    std::cout << "SEQ/EX  Time: " << res_seq.duration.count() << " ms\n";
    std::cout << "SEQ/ESI Time: " << res_seq_esi.duration.count() << " ms\n";
    std::cout << "OMP/ESI Time: " << res_omp.duration.count() << " ms ("
              << max_threads << " threads)\n";
    std::cout << "SEQ/EX energy difference: " << res_seq.energy_diff
              << " Joules\n";
    std::cout << "SEQ/ESI energy difference: " << res_seq_esi.energy_diff
              << " Joules\n";
    std::cout << "OMP/ESI energy difference: " << res_omp.energy_diff
              << " Joules\n";

    return 0;
}
