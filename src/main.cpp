#include <chrono>
#include <cmath>
#include <cstddef>
#include <iostream>
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

int main(void) {

    std::cout << "Running simulation on " << NB_ITER << " time iteration"
              << std::endl;
    std::cout << "With " << N << " bodies" << std::endl;

    auto start = std::chrono::high_resolution_clock::now();

    // compute for seq model
    Bodies b(N);
    const auto init_b_nrj = b.get_system_energy();
    auto start_seq = std::chrono::high_resolution_clock::now();
    run_seq<ForceModel, EulerExplicit>(b, DT, NB_ITER);
    auto end_seq = std::chrono::high_resolution_clock::now();
    const auto end_b_nrj = b.get_system_energy();

    // compute for omp model
    Bodies b_omp(N);
    const auto init_bomp_nrj = b.get_system_energy();
    auto start_omp = std::chrono::high_resolution_clock::now();
    run_omp<ForceModel, EulerExplicit>(b_omp, DT, NB_ITER);
    auto end_omp = std::chrono::high_resolution_clock::now();
    const auto end_bomp_nrj = b.get_system_energy();

    // compute execution times
    auto end = std::chrono::high_resolution_clock::now();
    auto duration =
        std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

    auto duration_seq = std::chrono::duration_cast<std::chrono::milliseconds>(
        end_seq - start_seq);
    auto duration_omp = std::chrono::duration_cast<std::chrono::milliseconds>(
        end_omp - start_omp);

    // compute energy difference accros models
    const auto nrj_diff_seq = end_b_nrj - init_b_nrj;
    const auto nrj_diff_omp = end_bomp_nrj - init_bomp_nrj;

    // simple prompt
    std::cout << "Execution Time: " << duration.count() << " ms" << std::endl;
    std::cout << "SEQ Time: " << duration_seq.count() << " ms" << std::endl;
    std::cout << "OMP Time: " << duration_omp.count() << " ms" << std::endl;
    std::cout << "SEQ energy difference: " << nrj_diff_seq << " Joules"
              << std::endl;
    std::cout << "OMP energy difference: " << nrj_diff_omp << " Joules"
              << std::endl;

    return 0;
}
