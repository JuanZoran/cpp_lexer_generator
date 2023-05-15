#pragma once
// #include <Type.hpp>
#include <concepts>
#include <fmt/core.h>
#include <map>
#include <set>

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


// (F)inite (S)tate (A)utomata
class FSA {
public:
    template <typename key, typename value>
    using map_t = std::map<key, value>;

    template <typename value>
    using set_t = std::set<value>;

    using char_t = char;
    using state_t = uint32_t;
    using size_t = uint32_t;

    using transition_t = std::pair<state_t, char_t>;
    using transition_map_t = map_t<transition_t, state_t>;
    using state_set_t = set_t<state_t>;
    using str_t = std::string;
    using state_info_t = str_t;


    enum class DiagramFmt {
        MARKDOWN,
        DOTFILE,
        IMAGE,
    };

    constexpr static state_t INVALID_STATE = -1;
    constexpr static auto graph_style = "rankdir=LR;\n"
                                        "graph [bgcolor = transparent];\n"
                                        "node [color = blue, fontcolor = white]\n"
                                        "edge [color = blue];\n";

public:
    FSA() = default;
    FSA(FSA&&) = default;
    FSA(const FSA&) = default;
    FSA& operator= (FSA&&) = default;
    FSA& operator= (const FSA&) = default;
    ~FSA() = default;

private:
};

template <>
struct fmt::formatter<FSA::transition_map_t>
{
    // auto parse(format_parse_context& ctx)
    constexpr auto parse(format_parse_context& ctx)
    {
        return ctx.begin();
    }

    template <typename FormatContext>
    auto format(const FSA::transition_map_t& m, FormatContext& ctx)
    {
        auto out = ctx.out();
        for (const auto& [key, value] : m) {
            out = format_to(
                ctx.out(), "{} -> {} [ label = \"{}\" ];\n", key.first, value, key.second);
        }
        return out;
    }
};
