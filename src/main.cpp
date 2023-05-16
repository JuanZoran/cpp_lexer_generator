#include <DFA.hpp>
#include <Util.hpp>
#include <fmt/format.h>
#include <vector>

using namespace std;
using namespace fmt;

#if 1
int main(int argc, char* argv[])
{
    NFA::str_t RE = "<((c|m|a|s)-)?(Tab|Cr|Leader|Left|Right|Down|Up|BS|Space|Esc)>";
    NFA nfa(RE);
    constexpr auto filename = "README.md";
    std::remove(filename);
    DFA dfa(nfa);
    dfa.minimal();

    Util::toDiagram(dfa, filename);
    NFA::str_t name = "test.cpp";
    dfa.saveTo(name);
    return 0;
}

#else
int main(int argc, char* argv[])
{
    NFA nfa;
    map<NFA::str_t, pair<NFA::priority_t, NFA::state_info_t> > test {

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
#endif
