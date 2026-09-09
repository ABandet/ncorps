#ifndef EULEUR_EXPLICIT_H_
#define EULEUR_EXPLICIT_H_

#include "bodies.h"
namespace ncorps {
class EulerExplicit {
  public:
    static void step(Bodies &b, const int i, const double dt, const double fx,
                     const double fy, const double fz) {
        // compute position
        b.m_rx[i] = b.m_rx[i] + b.m_vx[i] * dt;
        b.m_ry[i] = b.m_ry[i] + b.m_vy[i] * dt;
        b.m_rz[i] = b.m_rz[i] + b.m_vz[i] * dt;
        // compute speed
        b.m_vx[i] = b.m_vx[i] + fx * dt;
        b.m_vy[i] = b.m_vy[i] + fy * dt;
        b.m_vz[i] = b.m_vz[i] + fz * dt;
    }

    EulerExplicit() = delete;
};

} // namespace ncorps

#endif // EULEUR_EXPLICIT_H_
