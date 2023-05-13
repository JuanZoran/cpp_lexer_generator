#include <Util.hpp>
#include <array>
#include <gtest/gtest.h>
#include <vector>

using namespace Util;
using namespace std;

// TEST : Origin -> addConcatOperator -> toPostfix
// metadata:
// * -> 0 or more
// | -> or
// ^ -> concat

// origin str | addConcatOperator | toPostfix | charSet
const std::vector<std::array<Util::str, 4>> all_test {
    {"ab",          "a^b",         "ab^",       "ab"  },
    { "a|b",        "a|b",         "ab|",       "ab"  },
    { "a*b",        "a*^b",        "a*b^",      "ab"  },
    { "(a)b",       "(a)^b",       "ab^",       "ab"  },
    { "a(b)",       "a^(b)",       "ab^",       "ab"  },
    { "a|b*c",      "a|b*^c",      "ab*c^|",    "abc" },
    { "a*b|c",      "a*^b|c",      "a*b^c|",    "abc" },
    { "a*(b|c)",    "a*^(b|c)",    "a*bc|^",    "abc" },
    { "(a|b)c*",    "(a|b)^c*",    "ab|c*^",    "abc" },
    { "(a*b)|c",    "(a*^b)|c",    "a*b^c|",    "abc" },
    { "a*(b*c)",    "a*^(b*^c)",   "a*b*c^^",   "abc" },
    { "a|b|c",      "a|b|c",       "ab|c|",     "abc" },
    { "a*b*c",      "a*^b*^c",     "a*b*^c^",   "abc" },
    { "a|b*c|d",    "a|b*^c|d",    "ab*c^|d|",  "abcd"},
    { "a*(b|c)*d",  "a*^(b|c)*^d", "a*bc|*^d^", "abcd"},
    { "(a*b*)*",    "(a*^b*)*",    "a*b*^*",    "ab"  },
    { "a|b|(cd)*",  "a|b|(c^d)*",  "ab|cd^*|",  "abcd"},

 // test "+"
    { "a+",         "a+",          "a+",        "a"   },
    { "ab+",        "a^b+",        "ab+^",      "ab"  },
    { "a+b",        "a+^b",        "a+b^",      "ab"  },
    { "a+|b",       "a+|b",        "a+b|",      "ab"  },
    { "a|b+",       "a|b+",        "ab+|",      "ab"  },
    { "(a+)b",      "(a+)^b",      "a+b^",      "ab"  },
    { "a+(b)",      "a+^(b)",      "a+b^",      "ab"  },
    { "a+|b*c",     "a+|b*^c",     "a+b*c^|",   "abc" },
    { "a*b+|c",     "a*^b+|c",     "a*b+^c|",   "abc" },
    { "(a+|b)c*",   "(a+|b)^c*",   "a+b|c*^",   "abc" },
    { "(a*b+)|c",   "(a*^b+)|c",   "a*b+^c|",   "abc" },
    { "a+(b*c)",    "a+^(b*^c)",   "a+b*c^^",   "abc" },
    { "a+|b+|c",    "a+|b+|c",     "a+b+|c|",   "abc" },
    { "a*b*c+",     "a*^b*^c+",    "a*b*^c+^",  "abc" },
    { "a+|b*c|d",   "a+|b*^c|d",   "a+b*c^|d|", "abcd"},
    { "(a*b*)+",    "(a*^b*)+",    "a*b*^+",    "ab"  },
    { "a+|b|(cd)*", "a+|b|(c^d)*", "a+b|cd^*|", "abcd"},

    { "a+(b|c)*d",  "a+^(b|c)*^d", "a+bc|*^d^", "abcd"}, // a+b*c|*^d^

  // GPT4 error :
  // { "a+(b|c)*d",  "a+^(b|c)*^d", "a+b*c|*^d^", "abcd"}, // a+b*c|*^d^

  // test "?"
    { "a?",         "a?",          "a?",        "a"   },
    { "ab?",        "a^b?",        "ab?^",      "ab"  },
    { "a?b",        "a?^b",        "a?b^",      "ab"  },
    { "a?|b",       "a?|b",        "a?b|",      "ab"  },
    { "a|b?",       "a|b?",        "ab?|",      "ab"  },
    { "(a?)b",      "(a?)^b",      "a?b^",      "ab"  },
    { "a?(b)",      "a?^(b)",      "a?b^",      "ab"  },
    { "a?|b*c",     "a?|b*^c",     "a?b*c^|",   "abc" },
    { "a*b?|c",     "a*^b?|c",     "a*b?^c|",   "abc" },
    { "(a?|b)c*",   "(a?|b)^c*",   "a?b|c*^",   "abc" },
    { "(a*b?)|c",   "(a*^b?)|c",   "a*b?^c|",   "abc" },
    { "a?(b*c)",    "a?^(b*^c)",   "a?b*c^^",   "abc" },
    { "a?|b?|c",    "a?|b?|c",     "a?b?|c|",   "abc" },
    { "a*b*c?",     "a*^b*^c?",    "a*b*^c?^",  "abc" },
    { "a?|b*c|d",   "a?|b*^c|d",   "a?b*c^|d|", "abcd"},
    { "(a*b*)?",    "(a*^b*)?",    "a*b*^?",    "ab"  },
    { "a?|b|(cd)*", "a?|b|(c^d)*", "a?b|cd^*|", "abcd"},

    { "a?(b|c)*d",  "a?^(b|c)*^d", "a?bc|*^d^", "abcd"}, // a?b*c|*^d^
  // GPT4 error :
  // { "a?(b|c)*d",  "a?^(b|c)*^d", "a?b*c|*^d^", "abcd"},
};

TEST(UtilTest, addConcatOperatorTest)
{
    for (auto test : all_test) {
        str input = test[0];
        str right = test[1];

        addConcatOperator(input);
        EXPECT_EQ(right, input);
    }
}

TEST(UtilTest, toPostfixTest)
{
    for (auto test : all_test) {
        std::set<char> inputCharSet;
        str input = test[0];
        str right = test[2];

        addConcatOperator(input);
        getPostfixAndChatSet(input, inputCharSet);
        EXPECT_EQ(right, input);
        EXPECT_EQ(inputCharSet, std::set<char>(test[3].begin(), test[3].end()));
    }
}

int main(int argc, char** argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
