#include <Util.hpp>
#include <gtest/gtest.h>
#include <vector>

using namespace Util;
using namespace std;

int main(int argc, char** argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

// TEST :
// TEST(Unit Name, Test Name) {
//     TODO : Your Test
// }


TEST(Util, addConcatOperator) {
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
    };


    for (auto& [input, right] : all_tests) {
        auto left = std::move(input);
        addConcatOperator(left);
        EXPECT_EQ(right, left);
    }
}

TEST(Util, toPostfix) {
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
