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
    state_t _newState() noexcept
    {
        return _state_count++;
    }

    void _toMarkdown(std::ostream& os) const noexcept;
    void _toDotFile(std::ostream& os) const noexcept;

    void _toMarkdown(const str_t&, const std::ios_base::openmode) const noexcept;
    void _toDotFile(const str_t&, const std::ios_base::openmode) const noexcept;
    void _toImage(const str_t&) const noexcept;

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
    // _state_count { 1 },
    // _start_state { 0 },
    // WARN : this will destroy the nfa charset
    _charset { std::move(nfa._charset) }

{
    using state_set = Type::state_set_t;
    auto result = nfa.getReachedStates(nfa._start_state);
    assert(result);
    auto q0 = *result;

    // TODO :improve memory usage via use pointer to store state_set
    std::stack<state_set> st {};
    Type::map_t<state_set, state_t> states_map {};

    auto create_new_state = [this, &states_map, &st, &nfa](const state_set& q) {
        auto new_state = _newState();
        states_map[q] = new_state;
        st.push(q);

        // check if this state is final state
        if (q.find(nfa._final_state) != q.end())
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

inline void
    DFA::_toMarkdown(const str_t& filename, const std::ios_base::openmode openmode) const noexcept
{
    std::ofstream fout { filename, openmode };
    assert(fout.is_open());
    _toMarkdown(fout);
}

inline void
    DFA::_toDotFile(const str_t& filename, const std::ios_base::openmode openmode) const noexcept
{
    std::ofstream fout { filename, openmode };
    assert(fout.is_open());
    _toDotFile(fout);
}

inline void DFA::_toImage(const str_t& filename) const noexcept
{
    // std::remove(filename.c_str());
    // TODO :use system call to generate image
}

inline void DFA::_toMarkdown(std::ostream& os) const noexcept
{
    os << "```dot\n";
    _toDotFile(os);
    os << "```\n";
}

inline void DFA::_toDotFile(std::ostream& os) const noexcept
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
        "rankdir=LR;\n"
         "{start} [color = green];\n"
         "node [shape = circle];\n"

         "{_final_state_set}"

         "{transition_map}\n"

         "}}\n";
    // clang-format on

    os << fmt::format(
        fmt,
        "start"_a = _start_state,
        "_final_state_set"_a = get_final_state_set(),
        "transition_map"_a = _state_transition_map);
}
