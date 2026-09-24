#pragma once

#include <cmath>
#include <cstddef>
#include <random>
#include <vector>

namespace ncorps {

class Bodies {
public:
  Bodies() = delete;

  Bodies(const size_t n)
      : m_n(n), m_rx(n, 0.0), m_ry(n, 0.0), m_rz(n, 0.0), m_vx(n, 0.0),
        m_vy(n, 0.0), m_vz(n, 0.0), m_m(n, 1.0) {

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<double> dis(1.0, 1000.0);
    for (size_t i = 0; i < m_n; i++) {
      m_rx[i] = dis(gen);
      m_ry[i] = dis(gen);
      m_rz[i] = dis(gen);
    }
  }

  double get_body_energy(size_t i) const {
    const double speed = std::sqrt(std::pow(m_vx[i], 2) + std::pow(m_vy[i], 2) +
                                   std::pow(m_vz[i], 2));

    const double energy = 0.5 * 1 * std::pow(speed, 2);
    return energy;
  }

  // compute sum of cinetic energies in the bodies collection
  double get_system_energy() const {
    double sys_energy = 0;
    for (size_t i = 0; i < m_n; i++) {
      sys_energy += get_body_energy(i);
    }

    return sys_energy;
  }

  size_t m_n{0};
  // position
  std::vector<double> m_rx{};
  std::vector<double> m_ry{};
  std::vector<double> m_rz{};
  // speed
  std::vector<double> m_vx{};
  std::vector<double> m_vy{};
  std::vector<double> m_vz{};
  // mass
  std::vector<double> m_m{};
};

} // namespace ncorps
