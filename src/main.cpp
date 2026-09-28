#include <charconv>
#include <chrono>
#include <concepts>
#include <cstddef>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <omp.h>
#include <utility>

#include "api/concepts.hpp"
#include "api/real.hpp"
#include "datatype/bodies.hpp"
#include "datatype/forces.hpp"
#include "impl/force_model.hpp"
#include "impl/time_integration.hpp"

using real_t = double;

constexpr real_t DT = 0.001;
constexpr size_t N = 1000;
constexpr int NB_ITER = 100;
constexpr real_t G = 1;

using namespace ncorps;

template <typename FM, typename TI, SupportedReal Real>
  requires ForceModelC<FM, Real> && TimeIntegrator<TI, Real>
void run(Bodies<Real> &b, const Real dt, const int nb_iter, const Real g) {
  Forces<Real> f(b.m_n);
  for (int t = 0; t < nb_iter; t++) {
    FM::step_all(b, f, g);
    TI::step_all(b, f, dt);
  }
}

struct BenchResult {
  std::chrono::milliseconds duration;
  double energy_diff;
};

template <typename FM, std::invocable Func, SupportedReal Real>
  requires ForceModelC<FM, Real>
BenchResult run_benchmark(Bodies<Real> &b, Func &&func, const Real g) {
  const auto init_nrj = FM::compute_system_nrj(b, g);
  auto start = std::chrono::high_resolution_clock::now();

  std::forward<Func>(func)();

  auto end = std::chrono::high_resolution_clock::now();
  const auto end_nrj = FM::compute_system_nrj(b, g);

  auto duration =
      std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

  return {duration, end_nrj - init_nrj};
}

void print_usage() {
  std::cout << "Usage:" << std::endl;
  std::cout << "    nbody                             "
            << "Launch nbdoy with default parameters" << std::endl;
  std::cout << "    nbody <n_particule>               "
            << "Launch nbody with user number of particule" << std::endl;
  std::cout << "    nbody <n_particule> <G Constante> "
            << "Launch nbdoy with custom number of particule and "
               "gravitationnal constante"
            << std::endl;
}

int main(int argc, char **argv) {
  if (argc > 3) {
    std::cerr << "Incorrect number of argument." << std::endl;
    print_usage();
    return EXIT_FAILURE;
  }

  std::size_t n{N};
  real_t g{G};

  if (argc > 1) {
    const char *end = argv[1] + std::strlen(argv[1]);
    auto [ptr, ec] = std::from_chars(argv[1], end, n);
    if (argc > 2) {
      const char *end_g = argv[2] + std::strlen(argv[2]);
      auto [ptr, ec] = std::from_chars(argv[2], end_g, g);
    }
  }

  std::cout << "Running simulation on " << NB_ITER << " time iteration\n";
  std::cout << "With " << n << " bodies\n";
  std::cout << "Gravitational constante: " << g << std::endl;

  auto start_total = std::chrono::high_resolution_clock::now();
  const int max_threads = omp_get_max_threads();
  omp_set_num_threads(max_threads);

  Bodies<real_t> b_omp(n);
  auto res_omp = run_benchmark<ForceModel>(
      b_omp,
      [&]() { run<ForceModel, EulerSemiImplicit>(b_omp, DT, NB_ITER, g); }, g);
  b_omp.write_to_file("b_omp.data");

  auto end_total = std::chrono::high_resolution_clock::now();
  auto total_duration = std::chrono::duration_cast<std::chrono::milliseconds>(
      end_total - start_total);

  std::cout << "Total Execution Time: " << total_duration.count() << " ms\n";
  std::cout << "OMP/ESI Time: " << res_omp.duration.count() << " ms ("
            << max_threads << " threads)\n";
  std::cout << "OMP/ESI energy difference: " << res_omp.energy_diff
            << " Joules\n";

  return 0;
}
