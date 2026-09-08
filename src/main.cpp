#include <cmath>
#include <cstddef>
#include <vector>

using namespace std;

constexpr double DT = 0.001;
constexpr size_t N = 100;
constexpr int NB_ITER = 100;

class Bodies {
  public:
    Bodies() = delete;

    Bodies(const size_t n, const double dt) {
        this->m_n = n;
        this->m_dt = dt;

        this->m_rx = vector<double>(n, 0);
        this->m_ry = vector<double>(n, 0);
        this->m_rz = vector<double>(n, 0);

        this->m_vx = vector<double>(n, 0);
        this->m_vy = vector<double>(n, 0);
        this->m_vz = vector<double>(n, 0);

        this->m_m = vector<double>(n, 1);
    }

    double compute_dist_ij(const int i, const int j) {
        double dist =
            sqrt(pow(m_rx[j] - m_rx[i], 2) + pow(m_ry[j] - m_ry[i], 2) +
                 pow(m_rz[j] - m_rz[i], 2));
        return dist;
    }

    void compute() {
        for (int i = 0; i < m_n; i++) {
            // compute acc
            double fx{0}, fy{0}, fz{0};
            for (int j = 0; j < m_n; j++) {
                if (i == j)
                    continue;
                else {
                    auto dist_ij = compute_dist_ij(i, j);
                    double f = m_m[j] / (dist_ij * dist_ij * dist_ij);
                    fx += f * (m_rx[j] - m_rx[i]);
                    fy += f * (m_ry[j] - m_ry[i]);
                    fz += f * (m_rz[j] - m_rz[i]);
                }
            }

            // compute position
            m_rx[i] = m_rx[i] + m_vx[i] * m_dt;
            m_ry[i] = m_ry[i] + m_vy[i] * m_dt;
            m_rz[i] = m_rz[i] + m_vz[i] * m_dt;
            // compute speed
            m_vx[i] = m_vx[i] + fx * m_dt;
            m_vy[i] = m_vy[i] + fy * m_dt;
            m_vz[i] = m_vz[i] + fz * m_dt;
        }
    }

  private:
    size_t m_n{0};
    double m_dt{0};
    // position
    vector<double> m_rx{};
    vector<double> m_ry{};
    vector<double> m_rz{};
    // speed
    vector<double> m_vx{};
    vector<double> m_vy{};
    vector<double> m_vz{};
    // mass
    vector<double> m_m{};
};

int main(void) {
    Bodies b(N, DT);
    for (int i = 0; i < NB_ITER; i++)
        b.compute();
    return 0;
}
