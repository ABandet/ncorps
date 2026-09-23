#ifndef BODIES_H_
#define BODIES_H_

#include <cmath>
#include <cstddef>
#include <iostream>
#include <random>
#include <vector>

namespace ncorps {

class Bodies {
  public:
    Bodies() = delete;

    Bodies(const size_t n) {
        this->m_n = n;

        this->m_rx = std::vector<double>(n, 0);
        this->m_ry = std::vector<double>(n, 0);
        this->m_rz = std::vector<double>(n, 0);

        this->m_vx = std::vector<double>(n, 0);
        this->m_vy = std::vector<double>(n, 0);
        this->m_vz = std::vector<double>(n, 0);

        this->m_m = std::vector<double>(n, 1);

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
        const double speed = std::sqrt(
            std::pow(m_vx[i], 2) * std::pow(m_vy[i], 2) * std::pow(m_vz[i], 2));

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

#endif // BODIES_H_
