#include <DFA.hpp>
#include <Lexer.hpp>
#include <Util.hpp>
#include <fmt/format.h>
#include <vector>


using namespace std;
using namespace fmt;
using namespace Color;


#if 1
int main(int argc, char* argv[])
{
    NFA nfa;
    map<FSA::str_t, pair<NFA::priority_t, FSA::str_t>> test_cases {
        {"<(Leader|Tab)>",  { 4, "Key" } },
        { "b",  { 2, "B" } },
        { "c",  { 3, "C" } },
        { ">",  { 5, ">" } },
    };

    cout << Green << "======================" << Endl;
    for (auto& [key, value] : test_cases) {
        cout << Yellow << "Regex : " << key << " | "
             << "Priority :" << value.first << " | "
             << "Type : " << value.second << Endl;
        auto str1 = key;
        auto str2 = value.second;
        auto tmp = NFA(str1, str2, value.first);


        nfa = nfa + tmp;
    }

    cout << Green << "======================" << Endl;

    DFA dfa(nfa);
    // FIXME :
    // Error state info and priority
    // Error exception

    // BUG : minimal dfa cause state info error
    // dfa.minimal();


    constexpr auto str = "bbc<b<Leader><Tab>cc";
    cout << Color::Green << "Test Str:" << str << Color::Endl;

    istringstream iss(str);
    Lexer lexer(iss, dfa);
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
