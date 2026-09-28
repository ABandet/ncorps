#pragma once

#include <cstddef>

#include "api/real.hpp"
#include "datatype/bodies.hpp"
#include "datatype/forces.hpp"

namespace ncorps {

class EulerExplicit {
public:
  EulerExplicit() = delete;

  template <SupportedReal Real>
  static void step_all(Bodies<Real> &b, const Forces<Real> &f, const Real dt) {
    const size_t n = b.m_n;
    Real *__restrict rx = b.m_rx.data();
    Real *__restrict ry = b.m_ry.data();
    Real *__restrict rz = b.m_rz.data();
    Real *__restrict vx = b.m_vx.data();
    Real *__restrict vy = b.m_vy.data();
    Real *__restrict vz = b.m_vz.data();
    const Real *__restrict fx = f.m_fx.data();
    const Real *__restrict fy = f.m_fy.data();
    const Real *__restrict fz = f.m_fz.data();

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

  template <SupportedReal Real>
  static void step_all(Bodies<Real> &b, const Forces<Real> &f, const Real dt) {
    const size_t n = b.m_n;
    Real *__restrict rx = b.m_rx.data();
    Real *__restrict ry = b.m_ry.data();
    Real *__restrict rz = b.m_rz.data();
    Real *__restrict vx = b.m_vx.data();
    Real *__restrict vy = b.m_vy.data();
    Real *__restrict vz = b.m_vz.data();
    const Real *__restrict fx = f.m_fx.data();
    const Real *__restrict fy = f.m_fy.data();
    const Real *__restrict fz = f.m_fz.data();

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
