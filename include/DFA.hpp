#pragma once
#include <FSA.hpp>
#include <NFA.hpp>
#include <fmt/format.h>
#include <queue>
#include <range/v3/all.hpp>
#include <string>

// (D)eterministic (F)inite (A)utomata
class DFA: public FSA {
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

    void minimal() noexcept;

    state_t getStartState() const noexcept
    {
        return _start_state;
    }

    bool isFinalState(state_t state) const noexcept
    {
        return _final_state_set.count(state) != 0;
    }

    state_info_t getStateInfo(state_t state) const noexcept
    {
        return _state_info.at(state);
    }

    void printStateInfo() const
    {
        assert(!_state_info.empty());
        for (auto& [state, info] : _state_info) {
            fmt::print("state: {}, info: {}\n", state, info);
        }
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


private: // INFO :Private members
    /**
     * @brief final state info
     */
    map_t<state_t, state_info_t> _state_info {};

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
        if (nfa.hasFinalState(q))
            _final_state_set.insert(new_state);

        int priority = -1;
        for (auto state : q) {
            auto res = nfa.getStateInfo(state);
            if (!res)
                continue;

            if (res->first > priority) {
                priority = res->first;
                _state_info[new_state] = std::move(res->second);
            }
        }
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

inline void DFA::minimal() noexcept
{
    using namespace ranges;
#if 1
    // INFO :Hopcroft's Algorith for DFA minimization

    // divide into two groups: final state and non-final state
    // clang-format off
    set_t<state_set_t> groups { 
        views::ints(state_t{0}, _state_count) 
        | views::filter([&](state_t state) { return _final_state_set.count(state) == 0; })
        | to<state_set_t>(), 

        _final_state_set
    };
    // clang-format on
    std::stack<state_set_t> st {};
    {
        auto cur = groups.begin();
        st.emplace(*cur++);
        st.emplace(*cur);
    }

    auto split = [this](state_set_t& set) -> std::optional<std::pair<state_set_t, state_set_t>> {
        for (auto ch : _charset) {
            auto cur = set.begin();
            std::vector<state_set_t> split_groups { 2 };

            split_groups[0].emplace(*cur);
            auto should_be = _state_transition_map.count({ *cur++, ch });
            // TODO :
            for (; cur != set.end(); cur++) {
                decltype(split_groups.size()) index =
                    should_be != _state_transition_map.count({ *cur, ch });

                split_groups[index].emplace(*cur);
            }

            if (split_groups[1].size())
                return std::make_pair(split_groups[0], split_groups[1]);
        }

        return std::nullopt;
    };

    // split until no more split
    while (st.size()) {
        auto cur = st.top();
        st.pop();

        if (cur.size() <= 1)
            continue;

        auto res = split(cur);
        if (!res)
            continue;

        groups.erase(cur);
        groups.emplace(res->first);
        groups.emplace(res->second);

        st.emplace(res->first);
        st.emplace(res->second);
    }

    // convert original state to new state
    map_t<state_t, state_t> new_state_map {};
    decltype(_state_transition_map) new_state_transition_map {};

    for (auto& group : groups) {
        auto new_state = _newState();
        for (auto state : group)
            new_state_map.emplace(state, new_state);
    }

    { // INFO : Update
        // update start state transition
        _start_state = new_state_map.at(_start_state);

        // update final state set
        // clang-format off
        _final_state_set = _final_state_set
            | views::transform([&new_state_map](state_t state) { return new_state_map[state]; })
            | to<decltype(_final_state_set)>();
        // clang-format on

        // update state transition map
        // clang-format off
        _state_transition_map = _state_transition_map
            | views::transform([&new_state_map](auto&& pair) {
                return std::make_pair(
                      std::make_pair(new_state_map[pair.first.first], pair.first.second),
                      new_state_map[pair.second]
                );
              })
            | to<decltype(_state_transition_map)>();
        // clang-format on

        // update state info
        // clang-format off
        _state_info = _state_info
            | views::transform([&new_state_map](auto&& pair) {
                return std::make_pair(new_state_map[pair.first], pair.second);
            })
            | to<decltype(_state_info)>();
        // clang-format on
    }
#endif
    // INFO :Brzozowski's Algorithm for DFA minimization
}
