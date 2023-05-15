#include <DFA.hpp>
#include <Util.hpp>
#include <fmt/format.h>
#include <vector>

using namespace std;
using namespace fmt;

int main(int argc, char* argv[])
{
    vector<FSA::str_t> tests {
        "a+b",
        "a+",
        "a?",
        "a|b",
        "(a|b)?",
        "a|b*",
        "a|b+",
        "a|b?",
        "a|b|c",
    };
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
        dfa.minimal();
        Util::toDiagram(dfa, filename);
    }

    return EXIT_SUCCESS;
}
