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
    const vector<pair<Type::str, Type::str>> all_tests {
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
