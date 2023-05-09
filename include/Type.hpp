#pragma once
#include <concepts>
#include <map>
#include <set>
#include <string>

namespace Type {
using str = std::string;

template <typename T>
concept str_c = std::is_same_v<T, str>;
using char_t = char;
using state_t = uint32_t;

template <typename key, typename value>
using map = std::map<key, value>;

template <typename value>
using set = std::set<value>;

} // namespace Type
