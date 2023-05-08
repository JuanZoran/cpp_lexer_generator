#pragma once
#include <concepts>
#include <string>

namespace Type {
using str = std::string;

template <typename T>
concept str_c = std::is_same_v<T, str>;
using char_t = char;
using state_t = uint32_t;
} // namespace Type
