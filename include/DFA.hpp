#pragma once
#include <FSA.hpp>
#include <NFA.hpp>
#include <fmt/format.h>
#include <string>

// (D)eterministic (F)inite (A)utomata
class DFA: public FSA
{
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

public:
    IMPL_DRAGRAM;


private:
    state_t _newState() noexcept
    {
        return _state_count++;
    }

    void _toMarkdown(std::ostream& os) noexcept;
    void _toDotFile(std::ostream& os) noexcept;

    void _toMarkdown(const str_t& filename, const std::ios_base::openmode) noexcept;
    void _toDotFile(const str_t& filename, const std::ios_base::openmode) noexcept;
    void _toImage(const str_t& filename)  noexcept;

private: // INFO :Private members
    /**
     * @brief All states of this DFA
     */
    // Type::state_set_t _state_set {};

    /**
     * @brief final state info
     */
    map_t<state_t, std::pair<NFA::priority_t, str_t>> _state_info {};

    /**
     * @brief state transition map
     */
    transition_map_t _state_transition_map {};

    /**
     * @brief final state set
     */
    state_set_t _final_state_set {};

    /**
     * @brief input charset
     */
    set_t<char_t> _charset {};

    state_t _start_state {};
    size_t _state_count {};
};

inline DFA::DFA(const NFA& nfa) noexcept:
    // _state_count { 1 },
    // _start_state { 0 },
    _charset { nfa.getCharset() }

{
    auto result = nfa.getReachedStates(nfa.getStartState());
    assert(result);
    auto q0 = *result;

    // TODO :improve memory usage via use pointer to store state_set
    std::stack<state_set_t> st {};
    map_t<state_set_t, state_t> states_map {};

    auto create_new_state = [this, &states_map, &st, &nfa](const state_set_t& q) {
        auto new_state = _newState();
        states_map[q] = new_state;
        st.push(q);

        // check if this state is final state
        if (q.find(nfa.getFinalState()) != q.end())
            _final_state_set.insert(new_state);
    };
    create_new_state(q0);

    while (!st.empty()) {
        // INFO :Bug
        // auto& q = st.top();
        auto q = st.top();
        st.pop();

        for (auto ch : _charset) {
            auto q_next_ptr = nfa.getReachedStates(q, ch);
            if (!q_next_ptr) {
                continue;
            }

            // not found in existed states then create a new state
            if (states_map.find(*q_next_ptr) == states_map.end())
                create_new_state(*q_next_ptr);

            _state_transition_map[{ states_map[q], ch }] = states_map[*q_next_ptr];
        }
    }
}

inline void DFA::_toMarkdown(const str_t& filename, const std::ios_base::openmode openmode) noexcept
{
    std::ofstream fout { filename, openmode };
    assert(fout.is_open());
    _toMarkdown(fout);
}

inline void DFA::_toDotFile(const str_t& filename, const std::ios_base::openmode openmode) noexcept
{
    std::ofstream fout { filename, openmode };
    assert(fout.is_open());
    _toDotFile(fout);
}

inline void DFA::_toImage(const str_t& filename) noexcept
{
    // std::remove(filename.c_str());
    // TODO :use system call to generate image
}

inline void DFA::_toMarkdown(std::ostream& os) noexcept
{
    os << "```dot\n";
    _toDotFile(os);
    os << "```\n";
}

inline void DFA::_toDotFile(std::ostream& os) noexcept
{
    using namespace fmt::literals;
    auto get_final_state_set = [this]() {
        DFA::str_t str;
        for (const auto state : _final_state_set) {
            str += fmt::format("{} [shape=doublecircle];\n", state);
        }
        return str;
    };


    // clang-format off
    constexpr auto fmt = 
        "digraph DFA {{\n"
        "rankdir=LR;\n"
        "{graph_style}"
         "{start} [color = green];\n"
         "{_final_state_set}"

         "{transition_map}\n"

         "}}\n";
    // clang-format on

    os << fmt::format(
        fmt,
        "start"_a = _start_state,
        "graph_style"_a = graph_style,
        "_final_state_set"_a = get_final_state_set(),
        "transition_map"_a = _state_transition_map);
}
