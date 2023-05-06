#pragma once
#include <iostream>

namespace Color {

using str = std::string;

constexpr auto End = "\033[0m";
constexpr auto Endl = "\033[0m\n";


#define __ATTR__(name)


#define __DEFINE__(name, code)              \
    constexpr auto name = "\033[" code "m"; \
    inline str name##_s(str& s)             \
    {                                       \
        return name + s + End;              \
    }

/// @brief attribute
__DEFINE__(HighLight, "1") // 高亮
__DEFINE__(HalfLight, "2") // 亮度减半
__DEFINE__(Italic, "3")    // 斜体
__DEFINE__(UnderLine, "4") // 下划线
__DEFINE__(Twinkle, "5")   // 闪烁
__DEFINE__(Flash, "6")     // 快闪

/// @brief foreground
__DEFINE__(Red, "31")
__DEFINE__(Green, "1;32")
__DEFINE__(Yellow, "33")
__DEFINE__(Blue, "34")
__DEFINE__(Purpor, "35")
__DEFINE__(Dgreen, "36")

/// @brief background
__DEFINE__(White, "37")
__DEFINE__(BLACK, "40")   // 黑色
__DEFINE__(RED, "41")     // 红色
__DEFINE__(GREEN, "42")   // 绿色
__DEFINE__(BROWN, "43")   // 棕色
__DEFINE__(BLUE, "44")    // 蓝色
__DEFINE__(SKYBLUE, "46") // 孔雀蓝
__DEFINE__(WHITE, "47")   // 白色


#define STR(n) #n

// 向上移动 n
#define UP(n) STR(\033[\
n##A)

// 向下移动n
#define DOWN(n) STR(\033[\
n##B)

// 向右移动n
#define RIGHT(n) STR(\033[\
n##C)

// 向左移动n
#define LEFT(n) STR(\033[\
n##D)

// 设定光标的位置
#define SET(y, x) STR(\x1b[\
y;x##H)

constexpr auto Save = "\033[s";    // 保存当前光标位置
constexpr auto Recover = "\033[u"; // 恢复之前保存的位置
constexpr auto Hide = "\033[?25l"; // 隐藏光标
constexpr auto Show = "\033[?25h"; // 显示光标
} // namespace Color
