#include <Postfix.hpp>
#include <gtest/gtest.h>

int main(int argc, char** argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

// Original string:a+b*c, Postfix: a+bc*
const std::map<Postfix::str, Postfix::str> all_test {
    {"(a|b)",    "ab|"  },
    { "(ab)*",   "ab*"  },
    { "(a|b)*",  "ab|*" },
    { "ab(c|d)", "abcd|"},
    { "c*(a|b)", "c*ab|"},
};

// TEST :
TEST(Postfix, result) {
    Postfix p;
    for (auto& test : all_test) {
        EXPECT_EQ(p.getPostfix(test.first), test.second);
    }
}
