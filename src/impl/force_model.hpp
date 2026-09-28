#pragma once

#include <cmath>
#include <cstddef>

#include "api/real.hpp"
#include "datatype/bodies.hpp"
#include "datatype/forces.hpp"

namespace ncorps {

class ForceModel {
public:
  ForceModel() = delete;

  template <SupportedReal Real>
  static void step_all(const Bodies<Real> &b, Forces<Real> &f, const Real g) {
    const size_t n = b.m_n;
    const Real eps2 = static_cast<Real>(eps2_);

    const Real *__restrict rx = b.m_rx.data();
    const Real *__restrict ry = b.m_ry.data();
    const Real *__restrict rz = b.m_rz.data();
    const Real *__restrict m = b.m_m.data();

    Real *__restrict fx = f.m_fx.data();
    Real *__restrict fy = f.m_fy.data();
    Real *__restrict fz = f.m_fz.data();

#pragma omp parallel for schedule(static)
    for (size_t i = 0; i < n; ++i) {
      const Real xi = rx[i];
      const Real yi = ry[i];
      const Real zi = rz[i];
      Real ax = 0.0;
      Real ay = 0.0;
      Real az = 0.0;

#pragma omp simd reduction(+ : ax, ay, az)
      for (size_t j = 0; j < n; ++j) {
        const Real dx = rx[j] - xi;
        const Real dy = ry[j] - yi;
        const Real dz = rz[j] - zi;
        const Real d2 = dx * dx + dy * dy + dz * dz + eps2;
        const Real inv_d = Real(1) / std::sqrt(d2);
        const Real s = g * m[j] * inv_d * inv_d * inv_d;
        ax += s * dx;
        ay += s * dy;
        az += s * dz;
      }

      fx[i] = ax;
      fy[i] = ay;
      fz[i] = az;
    }
  }

  template <SupportedReal Real>
  static Real compute_system_nrj(const Bodies<Real> &b, const Real g) {
    const size_t n = b.m_n;
    const Real eps2 = static_cast<Real>(eps2_);
    Real u = 0.0;
    Real k = 0.0;
#pragma omp parallel for reduction(+ : u, k) schedule(dynamic, 16)
    for (size_t i = 0; i < n; i++) {
      // kinetic energy
      const Real v2 =
          b.m_vx[i] * b.m_vx[i] + b.m_vy[i] * b.m_vy[i] + b.m_vz[i] * b.m_vz[i];
      k += 0.5 * b.m_m[i] * v2;
      // potential energy
      for (size_t j = i + 1; j < n; j++) {
        const Real dx = b.m_rx[j] - b.m_rx[i];
        const Real dy = b.m_ry[j] - b.m_ry[i];
        const Real dz = b.m_rz[j] - b.m_rz[i];
        u -= g * b.m_m[i] * b.m_m[j] /
             std::sqrt(dx * dx + dy * dy + dz * dz + eps2);
      }
    }

    return u + k;
  }

private:
  static constexpr double eps2_ = 1e-9;
};

} // namespace ncorps
