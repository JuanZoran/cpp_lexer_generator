#pragma once
#include <NFA.hpp>
#include <Type.hpp>
#include <fmt/format.h>
#include <string>

class DFA
{
public:
    using state_t = Type::state_t;
    using size_t = uint32_t;
    using str_t = Type::str_t;

public:
    DFA(NFA&& nfa) noexcept;


    // clang-format off
    // DFA() noexcept  = default;
    DFA(DFA&&)      = default;
    DFA(const DFA&) = default;
    DFA& operator   = (DFA&&) = default;
    DFA& operator   = (const DFA&) = default;
    ~DFA()          = default;
    // clang-format on

public:
    IMPL_DRAGRAM;


private:
    state_t _newState() noexcept { return _state_count++; }

    void _toMarkdown(const str_t& filename, const std::ios_base::openmode) const noexcept;
    void _toDotFile(const str_t& filename, const std::ios_base::openmode) const noexcept;
    void _toImage(const str_t& filename) const noexcept;

private: // INFO :Private members
    /**
     * @brief All states of this DFA
     */
    // Type::state_set_t _state_set {};

    /**
     * @brief final state info
     */
    Type::map_t<state_t, std::pair<NFA::priority_t, NFA::str_t>> _state_info {};

    /**
     * @brief state transition map
     */
    Type::transition_map_t _state_transition_map {};

    /**
     * @brief final state set
     */
    Type::state_set_t _final_state_set {};

    /**
     * @brief input charset
     */
    Type::set_t<Type::char_t> _charset {};

    state_t _start_state {};
    size_t _state_count {};
};

inline DFA::DFA(NFA&& nfa) noexcept:
    _state_count { 1 },
    _start_state { 0 },
    _charset { std::move(nfa._charset) }

{
    using state_set = Type::state_set_t;
    auto q0 = *nfa.getReachedStates(nfa._start_state);

    // WARN : this will destroy the nfa charset

    // TODO :improve memory usage via use pointer to store state_set
    std::stack<state_set> st;
    st.push(q0);

    Type::map_t<state_set, state_t> states_map {
        {
         q0, _start_state,
         }
    };

    while (!st.empty()) {
        auto& q = st.top();
        st.pop();

        for (auto ch : _charset) {
            auto q_next_ptr = nfa.getReachedStates(q, ch);
            if (!q_next_ptr) { continue; }

            // not found in existed states then create a new state
            if (states_map.find(*q_next_ptr) == states_map.end()) {
                states_map[*q_next_ptr] = _newState();
                st.push(*q_next_ptr);
            }

            _state_transition_map[{ states_map[q], ch }] = states_map[*q_next_ptr];
        }
    }
}

inline void
    DFA::_toMarkdown(const str_t& filename, const std::ios_base::openmode flag) const noexcept
{
    using namespace fmt::literals;
    using ofs = std::ofstream;

    ofs fout { filename, flag };
    assert(fout.is_open());


    // FIXME : final state to be printed
    fout << fmt::format(
        "```dot\n"
        "digraph G {{\n"

        "rankdir=LR;\n"
        "{start} [color = green];\n"
        "node [shape = circle];\n"

        "{transition_map}\n"

        "}}\n"
        "```\n",

        "start"_a = _start_state,
        "transition_map"_a = _state_transition_map);
}

inline void DFA::_toDotFile(const str_t& filename, const std::ios_base::openmode) const noexcept
{
    // TODO :
}

inline void DFA::_toImage(const str_t& filename) const noexcept
{
    // TODO :
}
