#include <NFA.hpp>
#include <Util.hpp>
#include <cstdio>
#include <vector>

using namespace std;

/**
 * @brief Test doxygen
 *
 * @param argc arg number
 * @param argv arg value
 * @return result code
 */
int main(int argc, char* argv[])
{
    vector<std::string> tests {
        "a|b", "a*", "ab", "(a|b)*", "a|b|(cd)*",
    };


    constexpr auto filename = "README.md";
    remove(filename);

    NFA nfa;
    for (auto& test : tests) {
        cout << "Current: " << test << endl;
        nfa.parse(test);
        nfa.toDiagram(filename);
    }
    return 0;
}
