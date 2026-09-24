#pragma once

#include <cstddef>

#include "datatype/bodies.hpp"
#include "datatype/forces.hpp"

namespace ncorps {

class EulerExplicit {
public:
  EulerExplicit() = delete;

  static void step_all(Bodies &b, const Forces &f, const double dt) {
    const size_t n = b.m_n;
    double *__restrict rx = b.m_rx.data();
    double *__restrict ry = b.m_ry.data();
    double *__restrict rz = b.m_rz.data();
    double *__restrict vx = b.m_vx.data();
    double *__restrict vy = b.m_vy.data();
    double *__restrict vz = b.m_vz.data();
    const double *__restrict fx = f.m_fx.data();
    const double *__restrict fy = f.m_fy.data();
    const double *__restrict fz = f.m_fz.data();

#pragma omp parallel for simd schedule(static)
    for (size_t i = 0; i < n; ++i) {
      rx[i] += vx[i] * dt;
      ry[i] += vy[i] * dt;
      rz[i] += vz[i] * dt;

      vx[i] += fx[i] * dt;
      vy[i] += fy[i] * dt;
      vz[i] += fz[i] * dt;
    }
  }
};

class EulerSemiImplicit {
public:
  EulerSemiImplicit() = delete;

  static void step_all(Bodies &b, const Forces &f, const double dt) {
    const size_t n = b.m_n;
    double *__restrict rx = b.m_rx.data();
    double *__restrict ry = b.m_ry.data();
    double *__restrict rz = b.m_rz.data();
    double *__restrict vx = b.m_vx.data();
    double *__restrict vy = b.m_vy.data();
    double *__restrict vz = b.m_vz.data();
    const double *__restrict fx = f.m_fx.data();
    const double *__restrict fy = f.m_fy.data();
    const double *__restrict fz = f.m_fz.data();

#pragma omp parallel for simd schedule(static)
    for (size_t i = 0; i < n; ++i) {
      vx[i] += fx[i] * dt;
      vy[i] += fy[i] * dt;
      vz[i] += fz[i] * dt;

      rx[i] += vx[i] * dt;
      ry[i] += vy[i] * dt;
      rz[i] += vz[i] * dt;
    }
  }
};

} // namespace ncorps
