#ifndef BODIES_H_
#define BODIES_H_

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
