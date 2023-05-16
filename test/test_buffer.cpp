#include <Buffer.hpp>
#include <fstream>
#include <cassert>
#include <gtest/gtest.h>

int main(int argc, char** argv)
{
    constexpr auto filename = "../test.txt";
    std::ifstream ifs(filename);
    assert(ifs.is_open());

    Buffer buffer(ifs);
    buffer.printLines();

    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

// TEST :
// TEST(Unit Name, Test Name) {
//     TODO : Your Test
// }
