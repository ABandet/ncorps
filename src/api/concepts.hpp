#pragma once

#include <concepts>

#include "datatype/bodies.hpp"
#include "datatype/forces.hpp"

namespace ncorps {

template <typename T>
concept ForceModelC = requires(const Bodies &b, Forces &f, const double g) {
  { T::step_all(b, f, g) } -> std::same_as<void>;
};

template <typename T>
concept TimeIntegrator = requires(Bodies &b, const Forces &f, double dt) {
  { T::step_all(b, f, dt) } -> std::same_as<void>;
};

} // namespace ncorps
