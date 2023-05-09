#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <Util.hpp>
#include <array>
#include <doctest/doctest.h>
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

TEST_CASE("Util")
{
    SUBCASE("addConcatOperator")
    {
        for (auto test : all_test) {
            str input = test[0];
            str right = test[1];

            addConcatOperator(input);
            CHECK_EQ(right, input);
        }
    }

    SUBCASE("toPostfix")
    {
        for (auto test : all_test) {
            std::set<char> inputCharSet;
            str input = test[0];
            str right = test[2];

            addConcatOperator(input);
            getPostfixAndChatSet(input, inputCharSet);
            CHECK_EQ(right, input);
            CHECK_EQ(inputCharSet, std::set<char>(test[3].begin(), test[3].end()));
        }
    }
}

#if 0
#include <Util.hpp>
#include <array>
#include <gtest/gtest.h>
#include <vector>


    int
    main(int argc, char** argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

// TEST :
// TEST(Unit Name, Test Name) {
//     TODO : Your Test
// }


const vector<array<Util::str, 3>> all_tests {
 {"ab",         "a^b"        },
  { "a|b",       "a|b"        },
  { "a*b",       "a*^b"       },
  { "(a)b",      "(a)^b"      },
  { "a(b)",      "a^(b)"      },
  { "a(b)",      "a^(b)"      },
  { "a|b*c",     "a|b*^c"     },
  { "a*b|c",     "a*^b|c"     },
  { "a*(b|c)",   "a*^(b|c)"   },
  { "(a|b)c*",   "(a|b)^c*"   },
  { "(a*b)|c",   "(a*^b)|c"   },
  { "a*(b*c)",   "a*^(b*^c)"  },
  { "a|b|c",     "a|b|c"      },
  { "a*b*c",     "a*^b*^c"    },
  { "a|b*c|d",   "a|b*^c|d"   },
  { "a*(b|c)*d", "a*^(b|c)*^d"},
  { "(a*b*)*",   "(a*^b*)*"   },
  { "a|b|(cd)*", "a|b|(c^d)*" },
};

TEST(Util, addConcatOperator)
{
    const vector<pair<Util::str, Util::str>> all_tests {
        {"ab",         "a^b"        },
        { "a|b",       "a|b"        },
        { "a*b",       "a*^b"       },
        { "(a)b",      "(a)^b"      },
        { "a(b)",      "a^(b)"      },
        { "a(b)",      "a^(b)"      },
        { "a|b*c",     "a|b*^c"     },
        { "a*b|c",     "a*^b|c"     },
        { "a*(b|c)",   "a*^(b|c)"   },
        { "(a|b)c*",   "(a|b)^c*"   },
        { "(a*b)|c",   "(a*^b)|c"   },
        { "a*(b*c)",   "a*^(b*^c)"  },
        { "a|b|c",     "a|b|c"      },
        { "a*b*c",     "a*^b*^c"    },
        { "a|b*c|d",   "a|b*^c|d"   },
        { "a*(b|c)*d", "a*^(b|c)*^d"},
        { "(a*b*)*",   "(a*^b*)*"   },

        { "a|b|(cd)*", "a|b|(c^d)*" },
    };


    for (auto& [input, right] : all_tests) {
        auto left = std::move(input);
        addConcatOperator(left);
        EXPECT_EQ(right, left);
    }
}

TEST(Util, toPostfix)
{
    const std::vector<pair<Util::str, Util::str>> all_test {
        {"(a|b)",    "ab|"  },
        { "(ab)*",   "ab*"  },
        { "(a|b)*",  "ab|*" },
        { "ab(c|d)", "abcd|"},
        { "c*(a|b)", "c*ab|"},
    };

    for (auto& [input, expected] : all_test) {
        EXPECT_EQ(expected, toPostfix(input));
    }
}

#endif
