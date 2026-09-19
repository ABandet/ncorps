#include <cmath>
#include <cstddef>
#include <vector>

#include "bodies.h"
#include "euler_explicit.h"
#include "force_model.h"

constexpr double DT = 0.001;
constexpr size_t N = 100;
constexpr int NB_ITER = 100;

using namespace ncorps;

int main(void) {
    Bodies b(N);

    std::vector<double> fx(N), fy(N), fz(N);

    for (int t = 0; t < NB_ITER; t++) {
        // update force for n
        for (size_t i = 0; i < N; i++) {
            ForceModel::step(b, i, DT, fx[i], fy[i], fz[i]);
        }
        // update position for n+1
        for (size_t i = 0; i < N; i++) {
            EulerExplicit::step(b, i, DT, fx[i], fy[i], fz[i]);
        }
    }

    return 0;
}
