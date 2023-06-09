#pragma once
#include <FSA.hpp>
#include <Util.hpp>
#include <cassert>
#include <color.h>
#include <cstdint>
#include <fmt/format.h>
#include <fstream>
#include <optional>
#include <stack>
#include <string>
#include <string_view>
#include <utility>

// NOTE : Comments are grenarated by GPT4

/**
 * @class NFA
 * @brief Thompson algorithm's NFA
 *
 */

// (N)on-Deterministic (F)inite (A)utomata
class NFA: public FSA {
public: // INFO : type alias
    // Define type aliases for readability

    // clang-format off
    using priority_t = int32_t;
    using str_view_t = std::string_view;
    using epsilon_transition_map_t = FSA::map_t<state_t, NFA::state_set_t>;
    // clang-format on

public: // INFO : built-in method
    // Define default and copy constructors, and assignment operators
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


public: // INFO : member method
    IMPL_DRAGRAM;
    friend std::ostream& operator<< (std::ostream& os, const NFA& nfa) noexcept;

    void parse(str_t& RE) noexcept;
    void parse(str_t& RE, str_t& info, NFA::priority_t priority = 1) noexcept;

    void clear() noexcept;
    NFA& operator+ (NFA& rhs) noexcept;
    bool match(const str_view_t& str) const noexcept;


    // Methods to get states reached by transitions
    /**
     * @brief Get the set of stated reached by the given string
     *
     * @param state the start state
     */
    std::optional<state_set_t> getReachedStates(const state_t state) const noexcept;

    /**
     * @brief Get next states_set which reached by the given state and char
     * this will return empty set if the given state and char is not in the transition map
     * @param state current state
     * @param ch input char
     */
    std::optional<state_set_t> getReachedStates(const state_t state, char_t ch) const noexcept;


    std::optional<state_set_t>
        getReachedStates(const state_set_t& state_set, char_t ch) const noexcept;

    // TODO :
    void getReachedStates(const state_t state, state_set_t& reached_states) const noexcept;
    void getReachedStates(
        const state_t state, char_t ch, state_set_t& reached_states) const noexcept;

    // Getters for charset, start state, and final state
    set_t<char_t> getCharset() const noexcept
    {
        return _charset;
    }

    state_t getStartState() const noexcept
    {
        return _start_state;
    }

    bool hasFinalState(const set_t<state_t> set) const noexcept
    {
        return set.count(_final_state) == 1;
    }

    std::optional<std::pair<priority_t, state_info_t>>
        getStateInfo(const state_t state) const noexcept
    {
        return _state_info.count(state) == 1 ? std::make_optional(_state_info.at(state))
                                             : std::nullopt;
    }

public: // INFO : static method
    // Static method to get state information
    void printStateInfo() const
    {
        assert(!_state_info.empty());
        for (const auto& [key, value] : _state_info) {
            fmt::print("state : {}\n", key);
            fmt::print("priority : {}\n", value.first);
            fmt::print("info : {}\n", value.second);
        }
    }


private: // INFO : private static member method
    static state_t _newState()
    {
        return _state_count++;
    }

    __attribute__((used)) str_t toDotString() noexcept;

private: // INFO : private member method
    // Private methods to output the NFA to different formats
    void _toMarkdown(std::ostream& os) noexcept;
    void _toDotFile(std::ostream& os) noexcept;

    // TODO :
    void _toMarkdown(const str_t& filename, const std::ios_base::openmode flag) noexcept;
    void _toDotFile(const str_t& filename, const std::ios_base::openmode flag) noexcept;
    void _toImage(const str_t& filename) noexcept;



private: // INFO : private member variable
    transition_map_t _state_transition_map {};
    epsilon_transition_map_t _epsilon_transition_map {};
    state_t _start_state {};
    state_t _final_state {};


private:
    static map_t<state_t, std::pair<priority_t, str_t>> _state_info;
    static size_t _state_count;
    str_t _RE {};
    str_t _postfix {};
    set_t<char_t> _charset;
    str_t _pre_process {};
};

inline NFA::size_t NFA::_state_count {};
inline FSA::map_t<NFA::state_t, std::pair<NFA::priority_t, NFA::str_t>> NFA::_state_info {};

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

    os << Green << "start : " << End << nfa._start_state << '\n';
    os << Green << "end : " << End << nfa._final_state << '\n';
    os << Green << "transition : \n";
    for (const auto& [key, value] : nfa._state_transition_map) {
        os << Blue << key.first << End << " -" << Blue << key.second << End << "-> " << value
           << '\n';
    }


    os << Green << "epsilon transition : \n" << End;
    for (const auto& [key, value] : nfa._epsilon_transition_map) {
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

template <>
struct fmt::formatter<NFA::epsilon_transition_map_t>
{
    // auto parse(format_parse_context& ctx) { return ctx.begin(); }
    constexpr auto parse(format_parse_context& ctx)
    {
        return ctx.begin();
    }

    template <typename FormatContext>
    auto format(NFA::epsilon_transition_map_t& m, FormatContext& ctx)
    {
        auto out = ctx.out();
        for (const auto& [key, value] : m) {
            for (const auto& state : value) {
                out = format_to(ctx.out(), "{} -> {} [ label = \"ε\" ];\n", key, state);
            }
        }
        return out;
    }
};

inline void NFA::_toMarkdown(std::ostream& os) noexcept
{
    using namespace fmt::literals;
    os << fmt::format(
        "## RE: {RE}\n"
        "### Preprocess : {pre_process}\n"
        "### Postfix : {postfix}\n"
        "```dot\n"
        "\n"
        "{dot_string}"
        "```\n",

        "RE"_a = _RE,
        "pre_process"_a = _pre_process,
        "postfix"_a = _postfix,
        "dot_string"_a = toDotString());
}

inline void NFA::_toDotFile(std::ostream& os) noexcept
{
    // TODO :
    os << toDotString();
}

inline NFA::str_t NFA::toDotString() noexcept
{
    using namespace fmt::literals;
    return fmt::format(
        "digraph NFA {{\n"
        "{graph_style}"
        "{start} [color = green];\n"
        "{end} [shape = doublecircle];\n"

        "{_state_transition_map}\n"
        "{_epsilon_transition_map}"
        "}}\n",


        "graph_style"_a = graph_style,
        "start"_a = _start_state,
        "end"_a = _final_state,
        "_state_transition_map"_a = _state_transition_map,
        "_epsilon_transition_map"_a = _epsilon_transition_map);
}

// inline str

inline void NFA::_toMarkdown(const str_t& filename, const std::ios_base::openmode openmode) noexcept
{
    std::ofstream fout { filename, openmode };
    assert(fout.is_open());
    _toMarkdown(fout);
}

inline void NFA::_toDotFile(const str_t& filename, const std::ios_base::openmode openmode) noexcept
{
    std::ofstream fout { filename, openmode };
    assert(fout.is_open());
    _toDotFile(fout);
}

inline void NFA::_toImage(const str_t& filename) noexcept
{
    // TODO :
}

inline void NFA::clear() noexcept
{
    _state_transition_map.clear();
    _epsilon_transition_map.clear();
    _start_state = _final_state = 0;
    _RE.clear();
    _postfix.clear();
    _pre_process.clear();
}

inline void NFA::parse(NFA::str_t& RE) noexcept
{
    using stack = std::stack<std::pair<state_t, state_t>>;
    this->clear();
    stack st {};



    _RE = RE;

    Util::addConcatOperator(RE);
    _pre_process = RE;

    Util::getPostfixAndChatSet(RE, _charset);
    _postfix = RE;



    auto Kleene = [this, &st]() {
        auto new_start = _newState(), new_end = _newState();
        assert(st.size() >= 1);
        auto [start, end] = st.top();

        _epsilon_transition_map[new_start].emplace(start);
        _epsilon_transition_map[new_start].emplace(new_end);

        _epsilon_transition_map[end].emplace(start);
        _epsilon_transition_map[end].emplace(new_end);

        st.top() = { new_start, new_end };
    };

    auto Concat = [this, &st]() {
        assert(st.size() >= 2);
        auto [start1, end1] = st.top();
        st.pop();
        auto [start2, end2] = st.top();

        _epsilon_transition_map[end2].emplace(start1);

        st.top() = { start2, end1 };
    };

    auto Union = [this, &st]() {
        assert(st.size() >= 2);
        auto new_start = _newState(), new_end = _newState();
        auto [start1, end1] = st.top();
        st.pop();
        auto [start2, end2] = st.top();

        _epsilon_transition_map[new_start].emplace(start1);
        _epsilon_transition_map[new_start].emplace(start2);

        _epsilon_transition_map[end1].emplace(new_end);
        _epsilon_transition_map[end2].emplace(new_end);

        st.top() = { new_start, new_end };
    };

    auto Char = [this, &st](const char_t ch) {
        auto start = _newState(), end = _newState();
        _state_transition_map[{ start, ch }] = end;
        st.push({ start, end });
    };

    auto OneOrMore = [this, &st]() {
        assert(st.size() >= 1);

        auto new_start = _newState(), new_end = _newState();
        auto [start, end] = st.top();
        // st.pop();

        _epsilon_transition_map[new_start].emplace(start);
        _epsilon_transition_map[end].emplace(new_end);
        _epsilon_transition_map[end].emplace(start);

        // st.push({ new_start, new_end });
        st.top() = { new_start, new_end };
    };

    auto Optional = [this, &st]() {
        assert(st.size() >= 1);
        auto new_start = _newState(), new_end = _newState();
        auto [start, end] = st.top();

        _epsilon_transition_map[new_start].emplace(start);
        _epsilon_transition_map[new_start].emplace(new_end);

        _epsilon_transition_map[end].emplace(new_end);

        st.top() = { new_start, new_end };
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

            case '+':
                OneOrMore();
                break;

            case '?':
                Optional();
                break;

            default:
                Char(ch);
                break;
        }
    }



    assert(st.size() == 1);
    _start_state = st.top().first;
    _final_state = st.top().second;
}

inline void NFA::parse(NFA::str_t& RE, str_t& info, NFA::priority_t priority) noexcept
{
    parse(RE);
    _state_info[_final_state] = { priority, info };
}

inline NFA& NFA::operator+ (NFA& other) noexcept
{
    _state_transition_map.merge(other._state_transition_map);
    _epsilon_transition_map.merge(other._epsilon_transition_map);
    _charset.merge(other._charset);


    auto new_start = _newState();
    auto new_end = _newState();

    _epsilon_transition_map[new_start].emplace(_start_state);
    _epsilon_transition_map[new_start].emplace(other._start_state);
    _epsilon_transition_map[_final_state].emplace(new_end);
    _epsilon_transition_map[other._final_state].emplace(new_end);

    _start_state = new_start;
    _final_state = new_end;
    return *this;
}

inline bool NFA::match(const NFA::str_view_t& str) const noexcept
{
    fmt::print("TODO: {}", __func__);
    assert(false);
}

inline std::optional<NFA::state_set_t> NFA::getReachedStates(const state_t state) const noexcept
{
    if (state < _state_count) {
        NFA::state_set_t reached_states {};
        getReachedStates(state, reached_states);
        return reached_states;
    }
    return std::nullopt;
}

// 获取通过给定字符从当前状态转移到的状态集合
inline std::optional<NFA::state_set_t>
    NFA::getReachedStates(const NFA::state_t state, char_t ch) const noexcept
{
    auto it = _state_transition_map.find({ state, ch });
    if (it == _state_transition_map.end()) {
        return std::nullopt;
    }

    return getReachedStates(it->second);
}

// 获取通过给定字符从当前状态集合转移到的状态集合
inline void NFA::getReachedStates(
    const NFA::state_t state, char_t ch, NFA::state_set_t& reached_states) const noexcept
{
    auto it = _state_transition_map.find({ state, ch });
    if (it == _state_transition_map.end()) {
        return;
    }

    getReachedStates(it->second, reached_states);
}

// 将通过空字符从当前状态转移到的状态集合添加到 reached_states 中
inline void
    NFA::getReachedStates(const NFA::state_t state, NFA::state_set_t& reached_states) const noexcept
{
    std::stack<state_t> st {};
    st.emplace(state);
    reached_states.emplace(state);


    while (!st.empty()) {
        auto state = st.top();
        st.pop();
        if (_epsilon_transition_map.find(state) == _epsilon_transition_map.end())
            continue;

        for (const auto& s : _epsilon_transition_map.at(state)) {
            if (reached_states.find(s) != reached_states.end())
                continue;

            st.emplace(s);
            reached_states.emplace(s);
        }
    }
}

// 获取通过给定字符从当前状态集合转移到的状态集合
inline std::optional<NFA::state_set_t>
    NFA::getReachedStates(const NFA::state_set_t& state_set, char_t ch) const noexcept
{
    NFA::state_set_t reached_states {};
    for (const auto& state : state_set) {
        getReachedStates(state, ch, reached_states);
    }
    return reached_states.empty() ? std::nullopt : std::make_optional(reached_states);
}
