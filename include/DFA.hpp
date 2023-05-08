#pragma once
#include <NFA.hpp>
#include <map>
#include <set>
#include <string>

class DFA
{
public:
    using state = uint32_t;

public:
    DFA(const NFA& nfa) noexcept;


    // clang-format off
    // DFA() noexcept  = default;
    DFA(DFA&&)      = default;
    DFA(const DFA&) = default;
    DFA& operator   = (DFA&&) = default;
    DFA& operator   = (const DFA&) = default;
    ~DFA()          = default;
    // clang-format on

private:
    std::set<state> _states;
    std::map<state, std::pair<NFA::priority_t, NFA::str_t>> _state_info;
    std::map<std::pair<state, char>, state> _transition;
    std::set<state> _final;
    state _start;

    // 4 bytes padding
    state misc;
};

inline DFA::DFA(const NFA& nfa) noexcept
{
    std::map<std::set<NFA::state_t>, std::set<NFA::state_t>> states;
    // std::set<NFA::state> start = { nfa._start };
    // states[start] = nfa._start;
    // for (auto& [key, value] : states) {
    //     for (auto& [ch, next] : nfa._states[value]) {
    //         if (ch == NFA::epsilon) {
    //             states[key].insert(next);
    //         }
    //     }
    // }
}
