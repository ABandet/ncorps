#pragma once

#include <concepts>

#include "datatype/bodies.hpp"
#include "datatype/forces.hpp"

namespace ncorps {

template <typename T, typename Real>
concept ForceModelC =
    requires(const Bodies<Real> &b, Forces<Real> &f, const Real g) {
      { T::step_all(b, f, g) } -> std::same_as<void>;
    };

template <typename T, typename Real>
concept TimeIntegrator =
    requires(Bodies<Real> &b, const Forces<Real> &f, Real dt) {
      { T::step_all(b, f, dt) } -> std::same_as<void>;
    };

} // namespace ncorps
