#include "psi/test/TestHelper.h"
#include "psi/test/psi_mock.h"

#include "psi/tools/BitSet.h"

using namespace psi::tools;
using namespace psi::test;

TEST(BitSetTests, default_ctor)
{
    BitSet data;
    ASSERT_EQ(data.size(), 0u);
}

TEST(BitSetTests, ctor)
{
    BitSet data(200);
    ASSERT_EQ(data.size(), 200u);
}

TEST(BitSetTests, ctor_stream)
{
    // 00000001 00000010 00000011 00000100 00000101 00000110 00000111 00001000
    // 00001001 00000000 11111111
    constexpr size_t N = 11;
    uint8_t stream[N] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 0, 0xff};
    BitSet data(stream, N);
    ASSERT_EQ(data.size(), N * 8);
    EXPECT_EQ(data.toString(), "1000000001000000110000000010000010100000011000001110000000010000100100000000000011111111");
}

TEST(BitSetTests, ctor_string)
{
    const std::string expected = "100101";
    BitSet data(expected);
    ASSERT_EQ(data.size(), expected.size());
    EXPECT_EQ(data.toString(), "100101");
}

TEST(BitSetTests, test)
{
    BitSet data(200);
    for (uint16_t i = 0; i < 200; ++i) {
        data.set(i);
        EXPECT_TRUE(data.test(i));
        data.set(i, false);
        EXPECT_FALSE(data.test(i));
    }

    data.set(201);
    EXPECT_FALSE(data.test(201));
}

TEST(BitSetTests, inverse)
{
    auto doTest = [](const auto &testCaseName, const auto &bits, const auto &expected) {
        // SCOPED_TRACE(testCaseName);

        BitSet data(bits);
        data.inverse();
        EXPECT_EQ(data.toString(), expected);
    };

    doTest("case 1. bits < 64", "100000001000000010000000", "011111110111111101111111");
    doTest("case 2. bits == 64",
           "1000000010000000100000001000000010000000100000001000000010000000",
           "0111111101111111011111110111111101111111011111110111111101111111");
    doTest("case 3. bits > 64",
           "100000001000000010000000100000001000000010000000100000001000000001",
           "011111110111111101111111011111110111111101111111011111110111111110");
    doTest("case 4. bits > 64 && bits % 64 == 0",
           "10000000100000001000000010000000100000001000000010000000100000001000000010000000100000001000000010000000100"
           "000001000000010000000",
           "01111111011111110111111101111111011111110111111101111111011111110111111101111111011111110111111101111111011"
           "111110111111101111111");
    doTest("case 5. [1,2,3,4,5,6,7,8,9,0,ff]",
           "1000000001000000110000000010000010100000011000001110000000010000100100000000000011111111",
           "0111111110111111001111111101111101011111100111110001111111101111011011111111111100000000");
}

TEST(BitSetTests, reverse)
{
    auto doTest = [](const auto &testCaseName, const auto &bits, const auto &expected) {
        // SCOPED_TRACE(testCaseName);

        BitSet data(bits);
        data.reverse();
        EXPECT_EQ(data.toString(), expected);
    };

    doTest("case 1. bits < 64", "100000001000000010000000", "000000010000000100000001");
    doTest("case 2. bits == 64",
           "1000000010000000100000001000000010000000100000001000000010000000",
           "0000000100000001000000010000000100000001000000010000000100000001");
    doTest("case 3. bits > 64",
           "100000001000000010000000100000001000000010000000100000001000000001",
           "100000000100000001000000010000000100000001000000010000000100000001");
    doTest("case 4. bits > 64 && bits % 64 == 0",
           "10000000100000001000000010000000100000001000000010000000100000001000000010000000100000001000000010000000100"
           "000001000000010000000",
           "00000001000000010000000100000001000000010000000100000001000000010000000100000001000000010000000100000001000"
           "000010000000100000001");
}

TEST(BitSetTests, toString)
{
    constexpr size_t N = 4;
    uint8_t stream[N] = {1, 2, 3, 4};
    BitSet data(stream, N);
    EXPECT_EQ(data.toString(), "10000000010000001100000000100000");
}