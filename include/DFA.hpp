#pragma once
#include <NFA.hpp>
#include <Type.hpp>
#include <string>

class DFA
{
public:
    using state_t = Type::state_t;
    using size_t = uint32_t;

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

private:
    state_t _newState() noexcept
    {
        return _size++;
    }


private:
    /**
     * @brief All states of this DFA
     */
    Type::set<state_t> _states {};

    /**
     * @brief final state info
     */
    Type::map<state_t, std::pair<NFA::priority_t, NFA::str_t>> _state_info {};

    /**
     * @brief state transition map
     */
    Type::map<std::pair<state_t, Type::char_t>, state_t> _transition {};

    /**
     * @brief final state set
     */
    Type::set<state_t> _final {};

    /**
     * @brief input charset
     */
    Type::set<Type::char_t> _charset {};

    state_t _start {};
    size_t _size {};
};

inline DFA::DFA(NFA&& nfa) noexcept
{
    using state_set = Type::set<state_t>;
    _start = _newState();
    auto q0 = nfa.getReachedStates(nfa._start);

    // WARN : this will destroy the nfa charset
    _charset = std::move(nfa._charset);
    std::stack<state_set> st;
    st.push(q0);

    Type::map<state_set, state_t> states_map {
        {
         q0, _start,
         }
    };

    while (!st.empty()) {
        auto q = st.top();
        st.pop();

        for (auto c : _charset) {
            // auto q_next = nfa.getSameStates(q, c);
            // if (q_next.empty()) {
            //     continue;
            // }

            // if (states_map.find(q_next) == states_map.end()) {
            //     auto new_state = _newState();
            //     states_map[q_next] = new_state;
            //     st.push(q_next);
            // }

            // _transition[{states_map[q], c}] = states_map[q_next];
        }
    }
}
