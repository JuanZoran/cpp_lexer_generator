#include <DFA.hpp>
#include <Util.hpp>
#include <fmt/format.h>
#include <vector>

using namespace std;
using namespace fmt;


#if 1
int main(int argc, char* argv[])
{
    vector<FSA::str_t> tests { "1*0(0|1)*" };
    // vector<Type::str_t> tests {
    //     "a+b",
    //     // "a",
    // };
    constexpr auto filename = "README.md";
    remove(filename);


    auto nfa = NFA();
    for (auto& RE : tests) {
        print("Current RE: {}\n", Color::Green_s(RE));
        nfa.parse(RE);
        Util::toDiagram(nfa, filename);
        auto dfa = DFA(nfa);
        Util::toDiagram(dfa, filename);
    }

    return EXIT_SUCCESS;
}


#else
/**
 * @brief Test doxygen
 *
 * @param argc arg number
 * @param argv arg value
 * @return result code
 */
int main(int argc, char* argv[])
{
    vector<std::pair<std::string, std::string> > tests {
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

        Util::toDiagram(nfa, filename);
        print("Current RE: {}\n", Color::Green_s(test.first));
    }
    return EXIT_SUCCESS;
}
#endif
