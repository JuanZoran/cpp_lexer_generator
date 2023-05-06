#pragma once
#include <Util.hpp>
#include <cassert>
#include <color.h>
#include <fstream>
#include <map>
#include <set>
#include <stack>
#include <string>

/**
 * @class NFA
 * @brief Thompson algorithm's NFA
 *
 */
class NFA
{
public:
    using state = int;
    using str = std::string;

public:
    NFA() = default;
    explicit NFA(const char ch) noexcept;
    explicit NFA(const str& RE) noexcept;
    NFA(NFA&&) = default;
    NFA(const NFA&) = default;
    NFA& operator= (NFA&&) = default;
    NFA& operator= (const NFA&) = default;
    ~NFA() = default;

public:
    void toDiagram(const std::string& filename, const std::ios_base::openmode flag = std::ios::app)
        const noexcept;
    void parse(const str RE) noexcept;

    void clear() noexcept;
    friend std::ostream& operator<< (std::ostream& os, const NFA& nfa) noexcept;

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
    static size_t _size;
    static std::map<state, str> state_info;
    str RE {};
    str postfix {};
    str preProcess {};
};

inline size_t NFA::_size {};
inline std::map<NFA::state, NFA::str> NFA::state_info {};

/*
 *
 * INFO :
 * Thompson algorithm
 * meta characters : ( ) | * + ?
 */
inline NFA::NFA(const NFA::str& postfix) noexcept
{
    parse(postfix);
}

inline NFA::NFA(const char ch) noexcept:
    _start { new_state() },
    _end { new_state() }
{
    transition[{ _start, ch }] = _end;
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

    out << "## " << RE << '\n';
    out << "### Preprocess : " << preProcess << '\n';
    out << "### Postfix : " << postfix << '\n';

    out << "```dot\n";

    out << "digraph G {\n";
    out << "rankdir=LR;\n";
    out << "node [shape = doublecircle]; " << _end << ";\n";
    out << "node [shape = circle];\n";

    for (const auto& [key, value] : transition) {
        out << key.first << " -> " << value << " [ label = \"" << key.second << "\" ];\n";
    }

    for (const auto& [key, value] : epsilon_transition) {
        for (const auto& v : value) {
            out << key << " -> " << v << " [ label = \"ε\" ];\n";
        }
    }

    out << "}\n";
    out << "```\n";
}

inline void NFA::clear() noexcept
{
    transition.clear();
    epsilon_transition.clear();
    _start = _end = 0;
    _size = 0;
}

inline void NFA::parse(NFA::str RE) noexcept
{
    using stack = std::stack<std::pair<state, state>>;
    this->clear();
    stack st {};

    this->RE = RE;
    Util::addConcatOperator(RE);
    this->preProcess = RE;
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
}
