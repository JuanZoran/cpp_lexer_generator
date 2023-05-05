#pragma once
#include <map>
#include <ranges>
#include <stack>
#include <string>

namespace Util {
using str = std::string;

/**
 * @brief Add concat operator to regex string
 *
 * @param str Regex string
 */
// inline void addConcatOperator(const Type::str_c auto& str) {
inline void addConcatOperator(str& str) {
    constexpr auto concatOperator = '^';

    // left join unary operator: *, )
    // right join unary operator: (
    // binary operator: |, ^
    auto is_operator = [](const char ch) {
        switch (ch) {
            case '*':
            case ')':
            case '(':
            case '|':
            case '^':
                return true;
            default:
                return false;
        }
    };

    auto end = str.size() - 1;
    for (size_t i = 0; i < end; ++i) {
        auto ch = str[i];
        auto next_ch = str[i + 1];

        if ((!is_operator(ch) || ch == '*' || ch == ')')
            && (!is_operator(next_ch) || next_ch == '(')) {
            str.insert(++i, 1, concatOperator);
            ++end;
        }
    }
}


/**
 * @brief Convert infix expression to postfix expression
 *
 * @param infix expression
 * @return postfix expression
 */
inline str toPostfix(const str& infix) {
    static const std::map<char, int> Priorities = {
        {'|',  2},
        { '^', 2},
        { '*', 4},
        { '(', 0},
        { ')', 0}
    };
    using stack = std::stack<char>;
    auto isOperator = [](const char ch) {
        return Priorities.find(ch) != Priorities.end();
    };

    str postfix {};

    postfix.clear();
    stack st;

    auto processOperators = [&st, &postfix](const char ch) {
        if (ch == ')') {
            // WARNING : if there is no '(' in the stack, it will cause an error
            while (st.top() != '(') {
                postfix.push_back(st.top());
                st.pop();
            }

            st.pop();
        }
        else if (st.empty()) {
            st.push(ch);
        }
        else {
            while (!st.empty() && Priorities.at(st.top()) >= Priorities.at(ch)) {
                postfix.push_back(st.top());
                st.pop();
            }

            st.push(ch);
        }
    };

    auto processOperands = [&postfix](const char ch) {
        postfix.push_back(ch);
    };

    for (const auto ch : infix)
        if (isOperator(ch))
            processOperators(ch);
        else
            processOperands(ch);

    while (!st.empty()) {
        postfix.push_back(st.top());
        st.pop();
    }

    return postfix;
}
} // namespace Util
