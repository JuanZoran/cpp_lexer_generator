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
inline void addConcatOperator(const Type::str_c auto& str) {
    constexpr auto concatOperator = "^";

    auto isOperator = [](const char ch) {
        switch (ch) {
            case '|':
            case '^':
            case '*':
                // case '(':
                // case ')':
                return true;
                break;

            default:
                return false;
                break;
        }
    };

    auto end = str.size() - 1;
    for (auto i = 0; i < end; ++i) {
        if (!isOperator(str[i])) {
            if (!isOperator(str[i + 1])) {
                str.insert(++i, concatOperator);
                ++end;
            }
        }
    }
}
} // namespace Util
