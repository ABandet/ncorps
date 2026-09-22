#pragma once

#include "datatype/bodies.hpp"

namespace ncorps {

template <typename T>
concept TimeIntegrator =
    requires(Bodies &b, int i, double dt, double fx, double fy, double fz) {
        { T::step(b, i, dt, fx, fy, fz) } -> std::same_as<void>;
    };

template <typename T>
concept ForceModelC =
    requires(Bodies &b, int i, double dt, double fx, double fy, double fz) {
        { T::step(b, i, dt, fx, fy, fz) } -> std::same_as<void>;
    };

} // namespace ncorps
