#pragma once
#include <concepts>
#include <string>

namespace Type {
using str = std::string;

template <typename T>
concept str_c = std::is_same_v<T, str>;
} // namespace Type
