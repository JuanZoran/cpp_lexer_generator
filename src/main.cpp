#include <DFA.hpp>
#include <Lexer.hpp>
#include <Util.hpp>
#include <fmt/format.h>
#include <vector>


using namespace std;
using namespace fmt;

#if 1
int main(int argc, char* argv[])
{
    NFA nfa;
    map<FSA::str_t, pair<NFA::priority_t, FSA::str_t>> test_cases {
        {"a",  { 1, "A" }},
        { "b", { 2, "B" }},
        { "c", { 3, "C" }},
    };

    for (auto& [key, value] : test_cases) {
        auto str1 = key;
        auto str2 = value.second;
        auto tmp = NFA(str1, str2, value.first);
        nfa = nfa + tmp;
    }
    DFA dfa(nfa);
    dfa.minimal();


    istringstream iss("abc");
    Lexer lexer(iss, dfa);
    // FIXME : 
    // Error state info and priority
    // Error exception
    while (true) {
        auto token = lexer.nextToken();
        if (!token)
            break;
        token->print();
    }

    return 0;
}

#else
int main(int argc, char* argv[])
{
    NFA nfa;
    map<NFA::str_t, pair<NFA::priority_t, NFA::state_info_t>> test {

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
