#pragma once
#include <Type.hpp>
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
    enum class OperatorType {
        None,
        LeftJoin,
        RightJoin,
        Binary
    };



    // left join unary operator: *, +, ), ?
    // right join unary operator: (
    // binary operator: |, ^
    auto getOperatorType = [](const char ch) {
        switch (ch) {
            case '*':
            case '+':
            case ')':
            case '?':
                return OperatorType::LeftJoin;
            case '|':
            case '^':
                return OperatorType::Binary;

            case '(':
                return OperatorType::RightJoin;

            default:
                return OperatorType::None;
        }
    };


    auto end = str.size() - 1;
    for (size_t i = 0; i < end; ++i) {
        auto ch = str[i];
        auto next_ch = str[i + 1];

        auto ch_type = getOperatorType(ch);
        auto next_ch_type = getOperatorType(next_ch);

        if ((ch_type == OperatorType::None || ch_type == OperatorType::LeftJoin)
            && (next_ch_type == OperatorType::None || next_ch_type == OperatorType::RightJoin)) {
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
inline void processRegex(str& infix, Type::set<char>& inputCharSet)
{
    static const Type::map<char, int> Priorities = {
        {'|',  2},
        { '^', 4},
        { '*', 8},
        { '+', 8},
        { '?', 8},
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
