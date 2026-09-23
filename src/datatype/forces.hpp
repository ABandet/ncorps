#pragma once

#include <cstddef>
#include <vector>

namespace ncorps {

struct Forces {
    explicit Forces(const size_t n)
        : m_n(n), m_fx(n, 0.0), m_fy(n, 0.0), m_fz(n, 0.0) {}

    size_t m_n{0};
    std::vector<double> m_fx{};
    std::vector<double> m_fy{};
    std::vector<double> m_fz{};
};

} // namespace ncorps
