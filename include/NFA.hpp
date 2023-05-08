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
public: // INFO : type alias
    friend class DFA;

    // clang-format off
    using state_t      = uint32_t;
    using size_t     = uint32_t;
    using str_t        = std::string;
    using priority_t = uint32_t;
    using str_view_t   = std::string_view;
    // clang-format on

public: // INFO : built-in method
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

public: // INFO : member method
    friend std::ostream& operator<< (std::ostream& os, const NFA& nfa) noexcept;

    /**
     * @brief Generate the diagram of NFA via dot language [markdown format]
     *
     * @param filename the output filename [relative {relative to the binary file} | absolute]
     * @param flag the open mode
     */
    void toDiagram(
        const str_t& filename, const std::ios_base::openmode flag = std::ios::app) const noexcept;
    void parse(const str_t RE, auto&& info = "", NFA::priority_t priority = 1) noexcept;
    void clear() noexcept;
    NFA& operator+ (NFA& rhs) noexcept;

    bool match(const str_view_t& str) const noexcept;


    /**
     * @brief Get the set of stated reached by the given string
     *
     * @param state the start state
     */
    std::set<state_t> getReachedStates(const state_t state) const noexcept;


public: // INFO : static method
    static NFA::str_t stateInfo() noexcept;

private: // INFO : private member method
    static state_t new_state()
    {
        return _size++;
    }

private: // INFO : private member variable
    std::map<std::pair<state_t, char>, state_t> _transition {};
    std::map<state_t, std::set<state_t>> _epsilon_transition {};
    state_t _start {};
    state_t _end {};


private:
    static std::map<state_t, std::pair<priority_t, str_t>> _state_info;
    static size_t _size;
    str_t _RE {};
    str_t _postfix {};
    str_t _pre_process {};
};

inline NFA::size_t NFA::_size {};
inline std::map<NFA::state_t, std::pair<NFA::priority_t, NFA::str_t>> NFA::_state_info {};

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
    for (const auto& [key, value] : nfa._transition) {
        os << Blue << key.first << End << " -" << Blue << key.second << End << "-> " << value
           << '\n';
    }


    os << Green << "epsilon transition : \n" << End;
    for (const auto& [key, value] : nfa._epsilon_transition) {
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
    using namespace fmt::literals;
    using ofs = std::ofstream;


    ofs fout { filename, flag };
    assert(fout.is_open());

    auto get_transition = [this]() {
        str_t transition {};

        for (const auto& [key, value] : _transition) {
            transition +=
                fmt::format("{} -> {} [ label = \"{}\" ];\n", key.first, value, key.second);
        }

        for (const auto& [key, value] : _epsilon_transition) {
            for (const auto& v : value) {
                transition += fmt::format("{} -> {} [ label = \"ε\" ];\n", key, v);
            }
        }

        return transition;
    };


    fout << fmt::format(
        "## RE: {RE}\n"
        "### Preprocess : {pre_process}\n"
        "### Postfix : {postfix}\n"
        "\n"

        "```dot\n"
        "digraph G {{\n"

        "rankdir=LR;\n"
        "{start} [color = green];\n"
        "{end} [shape = doublecircle];\n"
        "node [shape = circle];\n"

        "{transition}\n"

        "}}\n"
        "```\n",



        "RE"_a = _RE,
        "pre_process"_a = _pre_process,
        "postfix"_a = _postfix,
        "start"_a = _start,
        "end"_a = _end,
        "transition"_a = get_transition());
}

inline void NFA::clear() noexcept
{
    _transition.clear();
    _epsilon_transition.clear();
    _start = _end = 0;
    _RE.clear();
    _postfix.clear();
    _pre_process.clear();
    _state_info.erase(this->_end);
}

inline void NFA::parse(NFA::str_t RE, auto&& info, NFA::priority_t priority) noexcept
{
    using stack = std::stack<std::pair<state_t, state_t>>;
    this->clear();
    stack st {};

    this->_RE = RE;
    Util::addConcatOperator(RE);
    this->_pre_process = RE;
    Util::toPostfix(RE);
    this->_postfix = RE;

    auto Kleene = [this, &st]() {
        auto new_start = new_state(), new_end = new_state();
        auto [start, end] = st.top();
        st.pop();

        _epsilon_transition[new_start].emplace(start);
        _epsilon_transition[new_start].emplace(new_end);

        _epsilon_transition[end].emplace(start);
        _epsilon_transition[end].emplace(new_end);

        st.push({ new_start, new_end });
    };

    auto Concat = [this, &st]() {
        auto [start1, end1] = st.top();
        st.pop();
        auto [start2, end2] = st.top();
        st.pop();

        _epsilon_transition[end2].emplace(start1);

        st.push({ start2, end1 });
    };

    auto Union = [this, &st]() {
        auto new_start = new_state(), new_end = new_state();
        auto [start1, end1] = st.top();
        st.pop();
        auto [start2, end2] = st.top();
        st.pop();

        _epsilon_transition[new_start].emplace(start1);
        _epsilon_transition[new_start].emplace(start2);

        _epsilon_transition[end1].emplace(new_end);
        _epsilon_transition[end2].emplace(new_end);

        st.push({ new_start, new_end });
    };

    auto Char = [this, &st](const char ch) {
        auto start = new_state(), end = new_state();
        _transition[{ start, ch }] = end;
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
        _state_info[_end] = { priority, info };
}

inline NFA& NFA::operator+ (NFA& other) noexcept
{
    _transition.merge(other._transition);
    _epsilon_transition.merge(other._epsilon_transition);


    auto new_start = new_state();
    auto new_end = new_state();

    _epsilon_transition[new_start].emplace(_start);
    _epsilon_transition[new_start].emplace(other._start);
    _epsilon_transition[_end].emplace(new_end);
    _epsilon_transition[other._end].emplace(new_end);

    _start = new_start;
    _end = new_end;
    return *this;
}

inline bool NFA::match(const NFA::str_view_t& str) const noexcept
{
    fmt::print("TODO: {}", __func__);
    return false;
}

inline NFA::str_t NFA::stateInfo() noexcept
{
    NFA::str_t info;
    for (const auto& [key, value] : NFA::_state_info) {
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

inline std::set<NFA::state_t> NFA::getReachedStates(const state_t state) const noexcept
{
    std::set<state_t> reached_states {};
    std::stack<state_t> st {};
    st.emplace(state);


    while (!st.empty()) {
        auto state = st.top();
        st.pop();
        if (_epsilon_transition.find(state) == _epsilon_transition.end())
            continue;

        for (const auto& v : _epsilon_transition.at(state)) {
            if (reached_states.find(v) != reached_states.end())
                continue;
            st.emplace(v);
            reached_states.emplace(v);
        }
    }

    return reached_states;
}
