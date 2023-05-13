#include <NFA.hpp>
#include <gtest/gtest.h>

NFA::str_t test_str = "a+b";

NFA::set_t<std::pair<NFA::state_t, NFA::state_set_t>> test_case {
    {0,  { 0 }         },
    { 1, { 0, 1, 3, 4 }},
    { 2, { 0, 2 }      },
    { 3, { 3, 4 }      },
    { 4, { 4 }         },
    { 5, { 5 }         },
};

NFA::map_t<std::pair<NFA::state_set_t, NFA::char_t>, NFA::state_set_t> transition_test_case {
  // {{ { 0 }, 'a' },           { 0, 1, 3, 4 }},
  // { { { 0, 1, 3, 4 }, 'a' }, { 0, 1, 3, 4 }},
    {{ { 0, 1, 3, 4 }, 'b' }, { 5 }},
};

class NFATest: public ::testing::Test
{
protected:
    NFA nfa;

    NFATest():
        nfa(test_str)
    {
    }
};

TEST_F(NFATest, getReachedStates)
{
    NFA::state_set_t reachedStates;
    for (auto test : test_case) {
        reachedStates = *nfa.getReachedStates(test.first);
        EXPECT_EQ(reachedStates, test.second);
    }
}

// FIXME:
// TEST_F(NFATest, getReachedStatesWithStateSetChar)
// {
//     NFA::state_set_t reachedStates;
//     for (auto [test, expected] : transition_test_case) {
//         reachedStates = *nfa.getReachedStates(test.first, test.second);
//         EXPECT_EQ(reachedStates, expected);
//     }
// }

// TEST_F(NFATest, toPostfix)
// {
//     for (auto test : all_test) {
//         std::set<char> inputCharSet;
//         NFA::str input = test[0];
//         NFA::str right = test[2];

//         addConcatOperator(input);
//         processRegex(input, inputCharSet);
//         EXPECT_EQ(right, input);
//         EXPECT_EQ(inputCharSet, std::set<char>(test[3].begin(), test[3].end()));
//     }
// }

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
