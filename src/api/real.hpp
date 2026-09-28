#pragma once

#include <concepts>

namespace ncorps {

template <typename T>
concept SupportedReal = std::same_as<T, float> || std::same_as<T, double>;

} // namespace ncorps
