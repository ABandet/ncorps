#pragma once

#include <cmath>
#include <cstddef>

#include "datatype/bodies.hpp"
#include "datatype/forces.hpp"

namespace ncorps {

class ForceModel {
public:
  ForceModel() = delete;

  static void step_all(const Bodies &b, Forces &f) {
    const size_t n = b.m_n;

    const double *__restrict rx = b.m_rx.data();
    const double *__restrict ry = b.m_ry.data();
    const double *__restrict rz = b.m_rz.data();
    const double *__restrict m = b.m_m.data();

    double *__restrict fx = f.m_fx.data();
    double *__restrict fy = f.m_fy.data();
    double *__restrict fz = f.m_fz.data();

#pragma omp parallel for schedule(static)
    for (size_t i = 0; i < n; ++i) {
      const double xi = rx[i];
      const double yi = ry[i];
      const double zi = rz[i];
      double ax = 0.0;
      double ay = 0.0;
      double az = 0.0;

#pragma omp simd reduction(+ : ax, ay, az)
      for (size_t j = 0; j < n; ++j) {
        const double dx = rx[j] - xi;
        const double dy = ry[j] - yi;
        const double dz = rz[j] - zi;
        const double d2 = dx * dx + dy * dy + dz * dz + eps2_;
        const double inv_d = 1.0 / std::sqrt(d2);
        const double s = g_ * m[j] * inv_d * inv_d * inv_d;
        ax += s * dx;
        ay += s * dy;
        az += s * dz;
      }

      fx[i] = ax;
      fy[i] = ay;
      fz[i] = az;
    }
  }

private:
  static constexpr double g_ = 6.674e-11;
  static constexpr double eps2_ = 1e-9;
};

} // namespace ncorps
