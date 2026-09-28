#pragma once

#include <cstddef>
#include <vector>

#include "api/real.hpp"

namespace ncorps {

template <SupportedReal Real> struct Forces {
  explicit Forces(const size_t n)
      : m_n(n), m_fx(n, 0.0), m_fy(n, 0.0), m_fz(n, 0.0) {}

  size_t m_n{0};
  std::vector<Real> m_fx{};
  std::vector<Real> m_fy{};
  std::vector<Real> m_fz{};
};

} // namespace ncorps
