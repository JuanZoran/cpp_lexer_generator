#include <DFA.hpp>
#include <Util.hpp>
#include <fmt/format.h>
#include <vector>

using namespace std;
using namespace fmt;

int main(int argc, char* argv[])
{
    NFA nfa;
    map<NFA::str_t, pair<NFA::priority_t, NFA::state_info_t>> test {
        {"a?",  { 1, "test1" }},
        { "a+", { 2, "test2" }},
        { "a*", { 3, "test3" }},
    };

    for (auto& [key, value] : test) {
        auto str1 = key;
        auto str2 = value.second;
        auto tmp = NFA(str1, str2, value.first);
        nfa = nfa + tmp;
    }

    constexpr auto filename = "README.md";
    std::remove(filename);

    Util::toDiagram(nfa, filename);
    // nfa.printStateInfo();

    DFA dfa(nfa);
    // dfa.printStateInfo();
    Util::toDiagram(dfa, filename);

    dfa.minimal();
    dfa.printStateInfo();
    Util::toDiagram(dfa, filename);
    return 0;
}
