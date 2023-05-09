#pragma once
#include <concepts>
#include <fmt/core.h>
#include <map>
#include <set>
#include <string>

/*
 * NOTE : Naming convention
 * class, enum: BigCamelCase [大驼峰]
 * function, method, variable: smallCamelCase [小驼峰]
 * variable: under_score [下划线]
 * type: under_score_t [下划线_t]
 * concept: under_score_c [下划线_c]
 * enum_value: ALL_CAPS [全大写]
 * private member: _under_score [_下划线]
 * private member method: _smallCamelCase [_小驼峰]
 *
 */

namespace Type {
using str_t = std::string;

template <typename T>
concept str_auto_ref_c =
    std::is_same_v<T, str_t> || std::is_same_v<T, str_t&> || std::is_same_v<T, const str_t&>
    || std::is_same_v<T, str_t&&> || std::is_same_v<T, const str_t&&>;


using char_t = char;
using state_t = uint32_t;
using transition_t = std::pair<state_t, char_t>;
using transition_map_t = std::map<transition_t, state_t>;

template <typename key, typename value>
using map_t = std::map<key, value>;

template <typename value>
using set_t = std::set<value>;

using state_set_t = set_t<state_t>;

enum class DiagramFmt {
    MARKDOWN,
    DOTFILE,
    IMAGE,
};

} // namespace Type

template <>
struct fmt::formatter<Type::transition_map_t>
{
    // auto parse(format_parse_context& ctx)
    constexpr auto parse(format_parse_context& ctx)
    {
        return ctx.begin();
    }

    template <typename FormatContext>
    auto format(const Type::transition_map_t& m, FormatContext& ctx)
    {
        auto out = ctx.out();
        for (const auto& [key, value] : m) {
            out = format_to(
                ctx.out(), "{} -> {} [ label = \"{}\" ];\n", key.first, value, key.second);
        }
        return out;
    }
};
