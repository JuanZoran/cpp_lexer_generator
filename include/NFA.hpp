#pragma once
#include <cassert>
#include <map>
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

public:
    explicit NFA(const char ch);
    explicit NFA(const std::string& postfix);
    NFA(NFA&&) = default;
    NFA(const NFA&) = default;
    NFA& operator= (NFA&&) = default;
    NFA& operator= (const NFA&) = default;
    ~NFA() = default;

    // TODO : add a method to print the NFA
    // TODO : add a method to validate a string

    void operator| (const NFA& other) {
        // TODO :
    }

    void operator^ (const NFA& other) {
        // TODO :
        // return NFA;
    }

    void operator* () {
        auto new_start = new_state();
        auto new_end = new_state();

        epsilon_transition[new_start] = new_end;
        epsilon_transition[new_start] = start;
        epsilon_transition[end] = new_end;
        epsilon_transition[end] = start;


        start = new_start;
        end = new_end;
    }


private:
    state new_state() {
        return size++;
    }

private:
    std::map<std::pair<state, char>, state> transition {};
    std::map<state, state> epsilon_transition {};
    state start {};
    state end {};
    size_t size {};
};

/*
 *
 * INFO :
 * Thompson algorithm
 * meta characters : ( ) | * + ?
 */
inline NFA::NFA(const std::string& postfix) {
    using stack = std::stack<NFA>;
    stack st {};

    // FIXME :
    for (const auto ch : postfix) {
        switch (ch) {
            case '*': {
                *st.top();
                break;
            }
            case '^': {
                auto rhs = st.top();
                st.pop();
                st.top() ^ rhs;
                break;
            }

            case '|': {
                auto rhs = st.top();
                st.pop();
                st.top() | rhs;
                break;
            }

            default:
                break;
        }
    }

    assert(st.size() == 1);
}

inline NFA::NFA(const char ch) {
    start = new_state();
    end = new_state();


    epsilon_transition[start] = end;
}
