#include <NFA.hpp>
#include <Util.hpp>
#include <cstdio>
#include <fmt/format.h>
#include <vector>

using namespace std;
using namespace fmt;

/**
 * @brief Test doxygen
 *
 * @param argc arg number
 * @param argv arg value
 * @return result code
 */
int main(int argc, char* argv[])
{
    vector<std::pair<std::string, std::string>> tests {
        {"a+b",     "测试一"},
        { "a+",     "测试二"},


        { "a?",     "测试三"},
        { "(a|b)?", "测试四"}
    };

    constexpr auto filename = "README.md";
    remove(filename);

    NFA nfa;
    for (auto& test : tests) {
        nfa.parse(test.first, test.second);
        nfa.toDiagram(filename);

        print("Current RE: {}\n", Color::Green_s(test.first));
    }
    return EXIT_SUCCESS;
}
