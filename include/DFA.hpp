#pragma once
#include <FSA.hpp>
#include <NFA.hpp>
#include <fmt/format.h>
#include <queue>
#include <range/v3/all.hpp>
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

    std::optional<state_t> getReachedState(state_t state, char_t ch) const noexcept
    {
        return (_state_transition_map.count({ state, ch }) == 0)
                 ? std::nullopt
                 : std::make_optional(_state_transition_map.at({ state, ch }));
    }

private:
    state_t _newState() noexcept
    {
        return _state_count++;
    }

    void _toMarkdown(std::ostream& os) noexcept;
    void _toDotFile(std::ostream& os) noexcept;

    void _toMarkdown(const str_t& filename, const std::ios_base::openmode) noexcept;
    void _toDotFile(const str_t& filename, const std::ios_base::openmode) noexcept;
    void _toImage(const str_t& filename) noexcept;

    str_t _toDotString() noexcept;

    void _minimal() noexcept;


private: // INFO :Private members
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
    auto initial_state = *result;

    // TODO :improve memory usage via use pointer to store state_set
    std::stack<state_set_t> state_stack {};
    map_t<state_set_t, state_t> states_map {};

    auto create_new_state = [this, &states_map, &state_stack, &nfa](const state_set_t& q) {
        auto new_state = _newState();
        states_map[q] = new_state;
        state_stack.push(q);

        // check if this state is final state
        if (q.find(nfa.getFinalState()) != q.end())
            _final_state_set.insert(new_state);
    };
    create_new_state(initial_state);

    while (!state_stack.empty()) {
        // INFO :Bug
        // auto& q = st.top();
        auto q = state_stack.top();
        state_stack.pop();

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

    _minimal();
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
    os << _toDotString();
}

inline DFA::str_t DFA::_toDotString() noexcept
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

    return fmt::format(
        fmt,
        "start"_a = _start_state,
        "graph_style"_a = graph_style,
        "_final_state_set"_a = get_final_state_set(),
        "transition_map"_a = _state_transition_map);
}

inline void DFA::_minimal() noexcept
{
    using namespace ranges;

    // INFO :Hopcroft's Algorith for DFA minimization

    // clang-format off
    set_t<state_set_t> groups { 
        views::ints(state_t{0}, _state_count) 
        | views::filter([&](state_t state) { return _final_state_set.count(state) == 0; })
        | to<state_set_t>(), 

        _final_state_set
    };
    // clang-format on
    std::stack<state_set_t> st;
    st.emplace(groups.begin(), groups.end());


    auto split = [this](state_set_t& set) -> std::optional<state_set_t> {
        for (auto ch : _charset) {
            auto cur = set.begin();
            auto should_be = _state_transition_map.count({ *cur++, ch });
            // TODO :
            for (; cur != set.end(); cur++)
                ;
        }

        return std::nullopt;
    };

    while (st.size()) {
        auto cur = st.top();
        st.pop();

        if (cur.size() == 1)
            continue;
    }

    // for (auto& group : groups) {
    //     auto changed = true;
    //     while (changed) {
    //     }
    // }

    // INFO :Brzozowski's Algorithm for DFA minimization

    // Step 1: Initialize partition with non-final and final states
    // set_t<state_set_t> partition { _final_state_set };
    // {
    //     state_set_t non_final_states;
    //     for (state_t state = 0; state < _state_count; ++state)
    //         if (_final_state_set.count(state) == 0)
    //             non_final_states.insert(state);
    //     partition.insert(non_final_states);
    // }

    // // Step 2: Refine partition
    // bool changed = true;
    // while (changed) {
    //     changed = false;
    //     set_t<state_set_t> new_partition;

    //     for (const state_set_t& part : partition) {
    //         map_t<state_set_t, state_set_t> split_parts;
    //         for (state_t state : part) {
    //             state_set_t part_key;
    //             for (char_t ch : _charset) {
    //                 auto reached_state = getReachedState(state, ch);
    //                 if (!reached_state)
    //                     continue;

    //                 for (const state_set_t& p : partition) {
    //                     if (p.count(*reached_state)) {
    //                         part_key.insert(p.begin(), p.end());
    //                         break;
    //                     }
    //                 }
    //             }
    //             split_parts[part_key].insert(state);
    //         }

    //         for (const auto& split_part : split_parts)
    //             new_partition.insert(split_part.second);

    //         if (split_parts.size())
    //             changed = true;
    //     }

    //     partition = new_partition;
    // }
}
