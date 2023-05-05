#pragma once
#include <Type.hpp>
#include <ranges>
#include <string>

namespace Util {
/**
 * @brief Add concat operator to regex string
 *
 * @param str Regex string
 */
// inline void addConcatOperator(const Type::str_c auto& str) {
inline void addConcatOperator(Type::str_c auto& str) {
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

        if ((!is_operator(ch) || ch == '*' || ch == ')') && (!is_operator(next_ch) || next_ch == '(')) {
            str.insert(++i, 1, concatOperator);
            ++end;
        }
    }
}
} // namespace Util
