#pragma once
#include <map>
#include <set>
#include <stack>
#include <string>

namespace Util {
using str = std::string;

/**
 * @brief Add concat operator to regex string
 *
 * @param str Regex string
 */
inline void addConcatOperator(str& str)
{
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

/*
 * @brief Convert infix expression to postfix expression and extract the input character set
 *
 * @param infix expression
 * @param inputCharSet reference to a set of characters to store the input character set
 */
inline void processRegex(str& infix, std::set<char>& inputCharSet)
{
    static const std::map<char, int> Priorities = {
        {'|',  2},
        { '^', 4},
        { '*', 8},
        { '(', 0},
        { ')', 0}
    };
    using stack = std::stack<char>;
    auto isOperator = [](const char ch) {
        return Priorities.find(ch) != Priorities.end();
    };

    str postfix {};
    stack st;

    auto processOperators = [&st, &postfix](const char ch) {
        if (ch == '(') {
            st.push(ch);
        }
        else if (ch == ')') {
            // WARNING : if there is no '(' in the stack, it will cause an error
            while (st.top() != '(') {
                postfix.push_back(st.top());
                st.pop();
            }

            st.pop();
        }

        else {
            while (!st.empty() && Priorities.at(st.top()) >= Priorities.at(ch)) {
                postfix.push_back(st.top());
                st.pop();
            }

            st.push(ch);
        }
    };

    auto processOperands = [&postfix, &inputCharSet](const char ch) {
        postfix.push_back(ch);
        inputCharSet.insert(ch);
    };

    for (const auto ch : infix)
        isOperator(ch) ? processOperators(ch) : processOperands(ch);

    while (!st.empty()) {
        postfix.push_back(st.top());
        st.pop();
    }

    infix = std::move(postfix);
}
} // namespace Util
