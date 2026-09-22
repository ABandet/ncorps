#ifndef FORCE_MODEL_H_
#define FORCE_MODEL_H_

#include <cmath>

#include "datatype/bodies.hpp"

namespace ncorps {

class ForceModel {
  public:
    ForceModel() = delete;

    static void step(Bodies &b, const size_t i, const double dt, double &fx,
                     double &fy, double &fz) {
        fx = 0;
        fy = 0;
        fz = 0;

        for (size_t j = 0; j < b.m_n; j++) {
            if (i == j)
                continue;
            else {
                auto dist_ij = compute_dist_ij(b, i, j);
                double f = g_ * b.m_m[j] / (dist_ij * dist_ij * dist_ij);
                fx += f * (b.m_rx[j] - b.m_rx[i]);
                fy += f * (b.m_ry[j] - b.m_ry[i]);
                fz += f * (b.m_rz[j] - b.m_rz[i]);
            }
        }
    }

  private:
    static constexpr double g_ = 6.674e-11;

    static double compute_dist_ij(const Bodies &b, const int i, const int j) {
        double dist =
            sqrt(pow(b.m_rx[j] - b.m_rx[i], 2) + pow(b.m_ry[j] - b.m_ry[i], 2) +
                 pow(b.m_rz[j] - b.m_rz[i], 2));
        return dist;
    }
};
} // namespace ncorps

#endif // FORCE_MODEL_H_
