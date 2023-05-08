#pragma once
#include <Util.hpp>
#include <cassert>
#include <color.h>
#include <cstdint>
#include <fmt/format.h>
#include <fstream>
#include <map>
#include <set>
#include <stack>
#include <string>
#include <string_view>
#include <utility>

/**
 * @class NFA
 * @brief Thompson algorithm's NFA
 *
 */
class NFA
{
public:
    using state = uint32_t;
    using size_t = uint32_t;
    using str = std::string;
    using priority_t = uint32_t;
    using str_view = std::string_view;

public:
    NFA() = default;
    /**
     * @brief The wrapper of parse function
     *
     * @tparam Args see parse function
     * @param args parse method's arguments
     */
    template <typename... Args>
    explicit NFA(Args&&... args) noexcept;
    NFA(NFA&&) = default;
    NFA(const NFA&) = default;
    NFA& operator= (NFA&&) = default;
    NFA& operator= (const NFA&) = default;
    ~NFA() = default;

public:
    void toDiagram(
        const str& filename, const std::ios_base::openmode flag = std::ios::app) const noexcept;
    void parse(const str RE, auto&& info = "", NFA::priority_t priority = 1) noexcept;
    void clear() noexcept;
    friend std::ostream& operator<< (std::ostream& os, const NFA& nfa) noexcept;
    NFA& operator+ (NFA& rhs) noexcept;

    bool match(const str_view& str) const noexcept;


public:
    static NFA::str stateInfo() noexcept;

private:
    static state new_state()
    {
        return _size++;
    }

private:
    std::map<std::pair<state, char>, state> transition {};
    std::map<state, std::set<state>> epsilon_transition {};
    state _start {};
    state _end {};


private:
    static std::map<state, std::pair<priority_t, str>> state_info;
    static size_t _size;
    str RE {};
    str postfix {};
    str pre_process {};
};

inline NFA::size_t NFA::_size {};
inline std::map<NFA::state, std::pair<NFA::priority_t, NFA::str>> NFA::state_info {};

/*
 *
 * INFO :
 * Thompson algorithm
 * meta characters : ( ) | * + ?
 */
template <typename... Args>
inline NFA::NFA(Args&&... args) noexcept
{
    parse(std::forward<Args>(args)...);
}

inline std::ostream& operator<< (std::ostream& os, const NFA& nfa) noexcept
{
    using namespace Color;

    os << Green << "start : " << End << nfa._start << '\n';
    os << Green << "end : " << End << nfa._end << '\n';
    os << Green << "transition : \n";
    for (const auto& [key, value] : nfa.transition) {
        os << Blue << key.first << End << " -" << Blue << key.second << End << "-> " << value
           << '\n';
    }


    os << Green << "epsilon transition : \n" << End;
    for (const auto& [key, value] : nfa.epsilon_transition) {
        os << Yellow << (key) << End << " -"
           << "epsilon"
           << "-> ";
        for (const auto& v : value) {
            os << v << ' ';
        }
        os << '\n';
    }
    return os;
}

inline void
    NFA::toDiagram(const std::string& filename, const std::ios_base::openmode flag) const noexcept
{
    using os = std::ofstream;
    os out { filename, flag };
    assert(out.is_open());

    out << fmt::format(
        "## RE: {}\n"
        "### Preprocess : {}\n"
        "### Postfix : {}\n"
        "\n"
        "```dot\n"
        "digraph G {{\n"
        "rankdir=LR;\n"
        "{} [shape = doublecircle];\n"
        "{} [color = green];\n"
        "node [shape = circle];\n",
        RE,
        pre_process,
        postfix,
        _end,
        _start);

    for (const auto& [key, value] : transition) {
        out << fmt::format("{} -> {} [ label = \"{}\" ];\n", key.first, value, key.second);
    }

    for (const auto& [key, value] : epsilon_transition) {
        for (const auto& v : value) {
            out << fmt::format("{} -> {} [ label = \"ε\" ];\n", key, v);
        }
    }
    out << "}\n"
           "```\n";
}

inline void NFA::clear() noexcept
{
    transition.clear();
    epsilon_transition.clear();
    _start = _end = 0;
    RE.clear();
    postfix.clear();
    pre_process.clear();
    state_info.erase(this->_end);
}

inline void NFA::parse(NFA::str RE, auto&& info, NFA::priority_t priority) noexcept
{
    using stack = std::stack<std::pair<state, state>>;
    this->clear();
    stack st {};

    this->RE = RE;
    Util::addConcatOperator(RE);
    this->pre_process = RE;
    Util::toPostfix(RE);
    this->postfix = RE;

    auto Kleene = [this, &st]() {
        auto new_start = new_state(), new_end = new_state();
        auto [start, end] = st.top();
        st.pop();

        epsilon_transition[new_start].emplace(start);
        epsilon_transition[new_start].emplace(new_end);

        epsilon_transition[end].emplace(start);
        epsilon_transition[end].emplace(new_end);

        st.push({ new_start, new_end });
    };

    auto Concat = [this, &st]() {
        auto [start1, end1] = st.top();
        st.pop();
        auto [start2, end2] = st.top();
        st.pop();

        epsilon_transition[end2].emplace(start1);

        st.push({ start2, end1 });
    };

    auto Union = [this, &st]() {
        auto new_start = new_state(), new_end = new_state();
        auto [start1, end1] = st.top();
        st.pop();
        auto [start2, end2] = st.top();
        st.pop();

        epsilon_transition[new_start].emplace(start1);
        epsilon_transition[new_start].emplace(start2);

        epsilon_transition[end1].emplace(new_end);
        epsilon_transition[end2].emplace(new_end);

        st.push({ new_start, new_end });
    };

    auto Char = [this, &st](const char ch) {
        auto start = new_state(), end = new_state();
        transition[{ start, ch }] = end;
        st.push({ start, end });
    };

    for (const auto ch : RE) {
        switch (ch) {
            case '*':
                Kleene();
                break;
            case '^':
                Concat();
                break;

            case '|':
                Union();
                break;

            default:
                Char(ch);
                break;
        }
    }

    assert(st.size() == 1);
    _start = st.top().first;
    _end = st.top().second;

    if (info.size() > 0)
        state_info[_end] = { priority, info };
}

inline NFA& NFA::operator+ (NFA& other) noexcept
{
    transition.merge(other.transition);
    epsilon_transition.merge(other.epsilon_transition);


    auto new_start = new_state();
    auto new_end = new_state();

    epsilon_transition[new_start].emplace(_start);
    epsilon_transition[new_start].emplace(other._start);
    epsilon_transition[_end].emplace(new_end);
    epsilon_transition[other._end].emplace(new_end);

    _start = new_start;
    _end = new_end;
    return *this;
}

inline bool NFA::match(const NFA::str_view& str) const noexcept
{
    fmt::print("TODO: {}", __func__);
    return false;
}

inline NFA::str NFA::stateInfo() noexcept
{
    NFA::str info;
    for (const auto& [key, value] : NFA::state_info) {
        info += fmt::format(
            "Index: {} \n"
            "\tpriority : {} \n"
            "\tinfo: {} \n",
            key,
            value.first,
            value.second);
    }
    return info;
}
