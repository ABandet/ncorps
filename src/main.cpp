#include <cmath>
#include <cstddef>
#include <vector>

#include "api/concepts.hpp"
#include "datatype/bodies.hpp"
#include "impl/force_model.hpp"
#include "impl/time_integration.hpp"

constexpr double DT = 0.001;
constexpr size_t N = 100;
constexpr int NB_ITER = 100;

using namespace ncorps;

template <ForceModelC FM, TimeIntegrator TI>
void run(Bodies &b, double dt, int nb_iter) {
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
    Bodies b(N);

    run<ForceModel, EulerExplicit>(b, DT, NB_ITER);

    return 0;
}
