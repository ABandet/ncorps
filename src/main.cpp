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

int main(void) {

    std::cout << "Running simulation on " << NB_ITER << " time iteration"
              << std::endl;

    std::cout << "With " << N << " bodies" << std::endl;

    auto start = std::chrono::high_resolution_clock::now();

    Bodies b(N);

    run_seq<ForceModel, EulerExplicit>(b, DT, NB_ITER);

    auto end = std::chrono::high_resolution_clock::now();

    auto duration =
        std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

    std::cout << "Execution Time: " << duration.count() << " ms" << std::endl;

    return 0;
}
