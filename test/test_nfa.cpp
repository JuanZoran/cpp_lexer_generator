#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <NFA.hpp>
#include <doctest/doctest.h>

NFA::str_t test_str = "a+b";
using namespace Type;

set_t<std::pair<NFA::state_t, state_set_t>> test_case {
    {0,  { 0 }         },
    { 1, { 0, 1, 3, 4 }},
    { 2, { 0, 2 }      },
    { 3, { 3, 4 }      },
    { 4, { 4 }         },
    { 5, { 5 }         },
};


map_t<std::pair<state_set_t, char_t>, state_set_t> transition_test_case {
    // {{ { 0 }, 'a' },           { 0, 1, 3, 4 }},
    // { { { 0, 1, 3, 4 }, 'a' }, { 0, 1, 3, 4 }},
    { { { 0, 1, 3, 4 }, 'b' }, { 5 }         },
};

TEST_CASE("NFA")
{
    NFA nfa(test_str);
    SUBCASE("getReachedStates")
    {
        SUBCASE("Param: state")
        {
            state_set_t reachedStates;
            for (auto test : test_case) {
                reachedStates = *nfa.getReachedStates(test.first);
                CHECK_EQ(reachedStates, test.second);
            }
        }
        // FIXME :
        // SUBCASE("Param: state_set char")
        // {
        //     state_set_t reachedStates;
        //     for (auto [test, expected] : transition_test_case) {
        //         reachedStates = *nfa.getReachedStates(test.first, test.second);
        //         CHECK_EQ(reachedStates, expected);
        //     }
        // }
    }

    //     SUBCASE("toPostfix")
    //     {
    //         for (auto test : all_test) {
    //             std::set<char> inputCharSet;
    //             str input = test[0];
    //             str right = test[2];

    //             addConcatOperator(input);
    //             processRegex(input, inputCharSet);
    //             CHECK_EQ(right, input);
    //             CHECK_EQ(inputCharSet, std::set<char>(test[3].begin(), test[3].end()));
    //         }
    //     }
}

// TEST :
// TEST(Unit Name, Test Name) {
//     TODO : Your Test
// }
