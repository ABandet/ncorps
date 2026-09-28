#pragma once

#include <cstddef>
#include <fstream>
#include <iomanip>
#include <limits>
#include <random>
#include <stdexcept>
#include <string>
#include <vector>

namespace ncorps {

class Bodies {
public:
  Bodies() = delete;

  Bodies(const size_t n, const unsigned seed = 42)
      : m_n(n), m_rx(n, 0.0), m_ry(n, 0.0), m_rz(n, 0.0), m_vx(n, 0.0),
        m_vy(n, 0.0), m_vz(n, 0.0), m_m(n, 1.0) {

    std::mt19937 gen(seed);
    std::uniform_real_distribution<double> dis(1.0, 1000.0);
    std::uniform_real_distribution<double> dis_m(1.0, 10.0);
    for (size_t i = 0; i < m_n; i++) {
      m_rx[i] = dis(gen);
      m_ry[i] = dis(gen);
      m_rz[i] = dis(gen);
      m_m[i] = dis_m(gen);
    }
  }

  void write_to_file(const std::string &filename) const {
    std::ofstream file{filename};
    if (!file) {
      throw std::runtime_error("Impossible d'ouvrir le fichier : " + filename);
    }

    file << std::setprecision(std::numeric_limits<double>::max_digits10);
    file << m_n << std::endl;
    for (size_t i = 0; i < m_n; i++) {
      file << m_rx[i] << " ";
      file << m_ry[i] << " ";
      file << m_rz[i] << "\n";
    }
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
