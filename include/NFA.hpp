#pragma once
#include <Type.hpp>
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
    using state_t    = Type::state_t;
    using size_t     = uint32_t;
    using str_t      = std::string;
    using priority_t = uint32_t;
    using str_view_t = std::string_view;
    using state_set_t = Type::state_set_t;
    using epsilon_treansion_map_t = Type::map_t<state_t, NFA::state_set_t>;
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
    IMPL_DRAGRAM;
    friend std::ostream& operator<< (std::ostream& os, const NFA& nfa) noexcept;

    void parse(str_t& RE) noexcept;

    void parse(str_t& RE, Type::str_auto_ref_c auto&& info, NFA::priority_t priority = 1) noexcept;

    void clear() noexcept;
    NFA& operator+ (NFA& rhs) noexcept;
    bool match(const str_view_t& str) const noexcept;


    /**
     * @brief Get the set of stated reached by the given string
     *
     * @param state the start state
     */
    std::optional<state_set_t> getReachedStates(const state_t state) const noexcept;

    std::optional<state_set_t>
        getReachedStates(const state_t state, Type::char_t ch) const noexcept;

    std::optional<state_set_t>
        getReachedStates(const state_set_t& state_set, Type::char_t ch) const noexcept;

    // TODO :
    void getReachedStates(const state_t state, state_set_t& reached_states) const noexcept;
    void getReachedStates(
        const state_t state, Type::char_t ch, state_set_t& reached_states) const noexcept;

public: // INFO : static method
    static NFA::str_t stateInfo() noexcept;

private: // INFO : private static member method
    static state_t _newState()
    {
        return _state_count++;
    }

private: // INFO : private member method
    void _toMarkdown(const str_t& filename, const std::ios_base::openmode flag) noexcept;


    // TODO :
    void _toDotFile(const str_t& filename, const std::ios_base::openmode flag) const noexcept;
    void _toImage(const str_t& filename) const noexcept;


private: // INFO : private member variable
    Type::transition_map_t _state_transition_map {};
    epsilon_treansion_map_t _epsilon_transition_map {};
    state_t _start_state {};
    state_t _final_state {};


private:
    static Type::map_t<state_t, std::pair<priority_t, str_t>> _state_info;
    static size_t _state_count;
    str_t _RE {};
    str_t _postfix {};
    Type::set_t<Type::char_t> _charset;
    str_t _pre_process {};
};

inline NFA::size_t NFA::_state_count {};
inline Type::map_t<NFA::state_t, std::pair<NFA::priority_t, NFA::str_t>> NFA::_state_info {};

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
struct fmt::formatter<NFA::epsilon_treansion_map_t>
{
    // auto parse(format_parse_context& ctx) { return ctx.begin(); }
    constexpr auto parse(format_parse_context& ctx)
    {
        return ctx.begin();
    }

    template <typename FormatContext>
    auto format(NFA::epsilon_treansion_map_t& m, FormatContext& ctx)
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

// inline str

inline void NFA::_toMarkdown(const str_t& filename, const std::ios_base::openmode flag) noexcept
{
    using namespace fmt::literals;
    using ofs = std::ofstream;


    ofs fout { filename, flag };
    assert(fout.is_open());

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

        "{_state_transition_map}\n"

        "{_epsilon_transition_map}\n"

        "}}\n"
        "```\n",



        "RE"_a = _RE,
        "pre_process"_a = _pre_process,
        "postfix"_a = _postfix,
        "start"_a = _start_state,
        "end"_a = _final_state,
        "_state_transition_map"_a = _state_transition_map,
        "_epsilon_transition_map"_a = _epsilon_transition_map);
}

inline void
    NFA::_toDotFile(const str_t& filename, const std::ios_base::openmode flag) const noexcept
{
    // TODO :
}

inline void NFA::_toImage(const str_t& filename) const noexcept
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
    _state_info.erase(this->_final_state);
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
        auto [start, end] = st.top();

        _epsilon_transition_map[new_start].emplace(start);
        _epsilon_transition_map[new_start].emplace(new_end);

        _epsilon_transition_map[end].emplace(start);
        _epsilon_transition_map[end].emplace(new_end);

        st.top() = { new_start, new_end };
    };

    auto Concat = [this, &st]() {
        auto [start1, end1] = st.top();
        st.pop();
        auto [start2, end2] = st.top();
        // st.pop();

        _epsilon_transition_map[end2].emplace(start1);

        // st.push({ start2, end1 });
        st.top() = { start2, end1 };
    };

    auto Union = [this, &st]() {
        auto new_start = _newState(), new_end = _newState();
        auto [start1, end1] = st.top();
        st.pop();
        auto [start2, end2] = st.top();
        // st.pop();

        _epsilon_transition_map[new_start].emplace(start1);
        _epsilon_transition_map[new_start].emplace(start2);

        _epsilon_transition_map[end1].emplace(new_end);
        _epsilon_transition_map[end2].emplace(new_end);

        // st.push({ new_start, new_end });
        st.top() = { new_start, new_end };
    };

    auto Char = [this, &st](const Type::char_t ch) {
        auto start = _newState(), end = _newState();
        _state_transition_map[{ start, ch }] = end;
        st.push({ start, end });
    };

    auto OneOrMore = [this, &st]() {
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
        auto new_start = _newState(), new_end = _newState();
        auto [start, end] = st.top();
        // st.pop();

        _epsilon_transition_map[new_start].emplace(start);
        _epsilon_transition_map[new_start].emplace(new_end);

        _epsilon_transition_map[end].emplace(new_end);

        // st.push({ new_start, new_end });
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

inline void
    NFA::parse(NFA::str_t& RE, Type::str_auto_ref_c auto&& info, NFA::priority_t priority) noexcept
{
    _state_info[_final_state] = { priority, std::forward(info) };
    parse(RE);
}

inline NFA& NFA::operator+ (NFA& other) noexcept
{
    _state_transition_map.merge(other._state_transition_map);
    _epsilon_transition_map.merge(other._epsilon_transition_map);


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

inline std::optional<NFA::state_set_t> NFA::getReachedStates(const state_t state) const noexcept
{
    if (state < _state_count) {
        NFA::state_set_t reached_states {};
        getReachedStates(state, reached_states);
        return reached_states;
    }
    return std::nullopt;
}

inline std::optional<NFA::state_set_t>
    NFA::getReachedStates(const NFA::state_t state, Type::char_t ch) const noexcept
{
    auto it = _state_transition_map.find({ state, ch });
    if (it == _state_transition_map.end()) {
        return std::nullopt;
    }

    return getReachedStates(it->second);
}

inline void NFA::getReachedStates(
    const NFA::state_t state, Type::char_t ch, NFA::state_set_t& reached_states) const noexcept
{
    auto it = _state_transition_map.find({ state, ch });
    if (it == _state_transition_map.end()) {
        return;
    }

    getReachedStates(it->second, reached_states);
}

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

inline std::optional<NFA::state_set_t>
    NFA::getReachedStates(const NFA::state_set_t& state_set, Type::char_t ch) const noexcept
{
    NFA::state_set_t reached_states {};

    for (const auto state : state_set)
        getReachedStates(state, ch, reached_states);

    if (reached_states.empty())
        return std::nullopt;
    return reached_states;
}
