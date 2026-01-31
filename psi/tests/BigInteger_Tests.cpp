#include "psi/test/TestHelper.h"
#include "psi/test/psi_mock.h"

#include "psi/tools/BigInteger.h"

using namespace psi::tools;
using namespace psi::test;

struct BigInteger::BigInteger_Tests {
    static auto readValue(BigInteger &b)
    {
        return b.m_value;
    }
};

TEST(BigInteger_Tests, default_ctor)
{
    BigInteger data;
    ASSERT_EQ(BigInteger::BigInteger_Tests::readValue(data).size(), 1u);
    EXPECT_EQ(BigInteger::BigInteger_Tests::readValue(data)[0], 0u);
}

TEST(BigInteger_Tests, ctor)
{
    BigInteger data(20);
    ASSERT_EQ(BigInteger::BigInteger_Tests::readValue(data).size(), 1u);
    EXPECT_EQ(BigInteger::BigInteger_Tests::readValue(data)[0], 20u);
}

TEST(BigInteger_Tests, ctor_vector)
{
    {
        BigInteger data({20, 0, 20});
        ASSERT_EQ(BigInteger::BigInteger_Tests::readValue(data).size(), 3u);
        EXPECT_EQ(BigInteger::BigInteger_Tests::readValue(data)[0], 20u);
        EXPECT_EQ(BigInteger::BigInteger_Tests::readValue(data)[1], 0u);
        EXPECT_EQ(BigInteger::BigInteger_Tests::readValue(data)[2], 20u);
    }

    {
        BigInteger data({20, 0, 0});
        ASSERT_EQ(BigInteger::BigInteger_Tests::readValue(data).size(), 1u);
        EXPECT_EQ(BigInteger::BigInteger_Tests::readValue(data)[0], 20u);
    }
}

TEST(BigInteger_Tests, operator_plus_BigInteger)
{
    auto doTest = [](const auto &testCaseName, const auto &data1, const auto &data2, const std::vector<uint64_t> &expected) {
        // SCOPED_TRACE(testCaseName);

        BigInteger data3 = data1 + data2;
        ASSERT_EQ(BigInteger::BigInteger_Tests::readValue(data3).size(), expected.size());
        for (size_t i = 0; i < expected.size(); ++i) {
            EXPECT_EQ(BigInteger::BigInteger_Tests::readValue(data3)[i], expected[i]);
        }
    };

    doTest("case 1", BigInteger(), BigInteger(40), {40});
    doTest("case 2", BigInteger(20), BigInteger(), {20});
    doTest("case 3", BigInteger(200), BigInteger(4000), {4200});
    doTest("case 4", BigInteger(0xffffffffffffffff), BigInteger(1), {0, 1});
    doTest("case 5", BigInteger(0xffffffffffffffff), BigInteger(0xffffffffffffffff), {0xfffffffffffffffe, 1});
    doTest("case 6",
           BigInteger({0xffffffffffffffff, 0xffffffffffffffff}),
           BigInteger({0xffffffffffffffff, 0xffffffffffffffff}),
           {0xfffffffffffffffe, 0xffffffffffffffff, 1});
    doTest("case 7",
           BigInteger(),
           BigInteger({0xffffffffffffffff, 0xffffffffffffffff}),
           {0xffffffffffffffff, 0xffffffffffffffff});
    doTest("case 8",
           BigInteger({0xffffffffffffffff, 0xffffffffffffffff}),
           BigInteger(),
           {0xffffffffffffffff, 0xffffffffffffffff});
}

TEST(BigInteger_Tests, operator_plus_uint64_t)
{
    auto doTest = [](const auto &testCaseName, const auto &data1, const auto &data2, const std::vector<uint64_t> &expected) {
        // SCOPED_TRACE(testCaseName);

        BigInteger data3 = data1 + data2;
        ASSERT_EQ(BigInteger::BigInteger_Tests::readValue(data3).size(), expected.size());
        for (size_t i = 0; i < expected.size(); ++i) {
            EXPECT_EQ(BigInteger::BigInteger_Tests::readValue(data3)[i], expected[i]);
        }
    };

    doTest("case 1", BigInteger(), 40, {40});
    doTest("case 2", BigInteger(20), 0, {20});
    doTest("case 3", BigInteger(200), 4000, {4200});
    doTest("case 4", BigInteger(0xffffffffffffffff), 1, {0, 1});
    doTest("case 5", BigInteger(0xffffffffffffffff), 0xffffffffffffffff, {0xfffffffffffffffe, 1});
    doTest("case 6", BigInteger({0xffffffffffffffff, 0xffffffffffffffff}), 0, {0xffffffffffffffff, 0xffffffffffffffff});
}

TEST(BigInteger_Tests, operator_plus_equal_BigInteger)
{
    auto doTest = [](const auto &testCaseName, auto &&data1, const auto &data2, const std::vector<uint64_t> &expected) {
        // SCOPED_TRACE(testCaseName);

        data1 += data2;
        ASSERT_EQ(BigInteger::BigInteger_Tests::readValue(data1).size(), expected.size());
        for (size_t i = 0; i < expected.size(); ++i) {
            EXPECT_EQ(BigInteger::BigInteger_Tests::readValue(data1)[i], expected[i]);
        }
    };

    doTest("case 1", BigInteger(), BigInteger(40), {40});
    doTest("case 2", BigInteger(20), BigInteger(), {20});
    doTest("case 3", BigInteger(200), BigInteger(4000), {4200});
    doTest("case 4", BigInteger(0xffffffffffffffff), BigInteger(1), {0, 1});
    doTest("case 5", BigInteger(0xffffffffffffffff), BigInteger(0xffffffffffffffff), {0xfffffffffffffffe, 1});
    doTest("case 6",
           BigInteger({0xffffffffffffffff, 0xffffffffffffffff}),
           BigInteger({0xffffffffffffffff, 0xffffffffffffffff}),
           {0xfffffffffffffffe, 0xffffffffffffffff, 1});
    doTest("case 7",
           BigInteger(),
           BigInteger({0xffffffffffffffff, 0xffffffffffffffff}),
           {0xffffffffffffffff, 0xffffffffffffffff});
    doTest("case 8",
           BigInteger({0xffffffffffffffff, 0xffffffffffffffff}),
           BigInteger(),
           {0xffffffffffffffff, 0xffffffffffffffff});
}

TEST(BigInteger_Tests, operator_plus_equal_uint64_t)
{
    auto doTest = [](const auto &testCaseName, auto &&data1, const auto &data2, const std::vector<uint64_t> &expected) {
        // SCOPED_TRACE(testCaseName);

        data1 += data2;
        ASSERT_EQ(BigInteger::BigInteger_Tests::readValue(data1).size(), expected.size());
        for (size_t i = 0; i < expected.size(); ++i) {
            EXPECT_EQ(BigInteger::BigInteger_Tests::readValue(data1)[i], expected[i]);
        }
    };

    doTest("case 1", BigInteger(), 40, {40});
    doTest("case 2", BigInteger(20), 0, {20});
    doTest("case 3", BigInteger(200), 4000, {4200});
    doTest("case 4", BigInteger(0xffffffffffffffff), 1, {0, 1});
    doTest("case 5", BigInteger(0xffffffffffffffff), 0xffffffffffffffff, {0xfffffffffffffffe, 1});
    doTest("case 6", BigInteger({0xffffffffffffffff, 0xffffffffffffffff}), 0, {0xffffffffffffffff, 0xffffffffffffffff});
}

TEST(BigInteger_Tests, operator_minus_BigInteger)
{
    auto doTest = [](const auto &testCaseName, const auto &data1, const auto &data2, const std::vector<uint64_t> &expected) {
        // SCOPED_TRACE(testCaseName);

        BigInteger data3 = data1 - data2;
        ASSERT_EQ(BigInteger::BigInteger_Tests::readValue(data3).size(), expected.size());
        for (size_t i = 0; i < expected.size(); ++i) {
            EXPECT_EQ(BigInteger::BigInteger_Tests::readValue(data3)[i], expected[i]);
        }
    };

    doTest("case 1", BigInteger(), BigInteger(40), {0xffffffffffffffd8});
    doTest("case 2", BigInteger(20), BigInteger(20), {0});
    doTest("case 3", BigInteger(4400), BigInteger(200), {4200});
    doTest("case 4", BigInteger({0, 1}), BigInteger(1), {0xffffffffffffffff});
    doTest("case 5", BigInteger({0xfffffffffffffffe, 1}), BigInteger(0xffffffffffffffff), {0xffffffffffffffff});
    doTest("case 6",
           BigInteger({0xffffffffffffffff, 0xffffffffffffffff}),
           BigInteger({0, 0xffffffffffffffff}),
           {0xffffffffffffffff});
    doTest("case 7",
           BigInteger({0xffffffffffffffff, 0, 0xffffffffffffffff}),
           BigInteger({0, 0, 0xffffffffffffffff}),
           {0xffffffffffffffff});
    doTest("case 8", BigInteger({0, 0, 1}), BigInteger(1), {0xffffffffffffffff, 0xffffffffffffffff});
    doTest("case 9", BigInteger({0, 1, 0xffffffffffffffff}), BigInteger(1), {0xffffffffffffffff, 0, 0xffffffffffffffff});
    doTest("case 10",
           BigInteger({0xffffffffffffffff, 1, 0xffffffffffffffff}),
           BigInteger({0, 1}),
           {0xffffffffffffffff, 0, 0xffffffffffffffff});
    doTest("case 11",
           BigInteger({0xffffffffffffffff, 0, 0, 1}),
           BigInteger({0, 0, 1}),
           {0xffffffffffffffff, 0, 0xffffffffffffffff});
    doTest("case 12",
           BigInteger({0xffffffffffffffff, 0, 0, 0, 0, 0, 0xff}),
           BigInteger({0, 0, 0, 0, 0, 0, 0xff}),
           {0xffffffffffffffff});
}

TEST(BigInteger_Tests, operator_minus_uint64_t)
{
    auto doTest = [](const auto &testCaseName, const auto &data1, const auto &data2, const std::vector<uint64_t> &expected) {
        // SCOPED_TRACE(testCaseName);

        BigInteger data3 = data1 - data2;
        ASSERT_EQ(BigInteger::BigInteger_Tests::readValue(data3).size(), expected.size());
        for (size_t i = 0; i < expected.size(); ++i) {
            EXPECT_EQ(BigInteger::BigInteger_Tests::readValue(data3)[i], expected[i]);
        }
    };

    doTest("case 1", BigInteger(), 40, {0xffffffffffffffd8});
    doTest("case 2", BigInteger(20), 20, {0});
    doTest("case 3", BigInteger(4400), 200, {4200});
    doTest("case 4", BigInteger({0, 1}), 1, {0xffffffffffffffff});
    doTest("case 5", BigInteger({0xfffffffffffffffe, 1}), 0xffffffffffffffff, {0xffffffffffffffff});
}

TEST(BigInteger_Tests, operator_minus_equal_BigInteger)
{
    auto doTest = [](const auto &testCaseName, auto &&data1, const auto &data2, const std::vector<uint64_t> &expected) {
        // SCOPED_TRACE(testCaseName);

        data1 -= data2;
        ASSERT_EQ(BigInteger::BigInteger_Tests::readValue(data1).size(), expected.size());
        for (size_t i = 0; i < expected.size(); ++i) {
            EXPECT_EQ(BigInteger::BigInteger_Tests::readValue(data1)[i], expected[i]);
        }
    };

    doTest("case 1", BigInteger(), BigInteger(40), {0xffffffffffffffd8});
    doTest("case 2", BigInteger(20), BigInteger(20), {0});
    doTest("case 3", BigInteger(4400), BigInteger(200), {4200});
    doTest("case 4", BigInteger({0, 1}), BigInteger(1), {0xffffffffffffffff});
    doTest("case 5", BigInteger({0xfffffffffffffffe, 1}), BigInteger(0xffffffffffffffff), {0xffffffffffffffff});
    doTest("case 6",
           BigInteger({0xffffffffffffffff, 0xffffffffffffffff}),
           BigInteger({0, 0xffffffffffffffff}),
           {0xffffffffffffffff});
    doTest("case 7",
           BigInteger({0xffffffffffffffff, 0, 0xffffffffffffffff}),
           BigInteger({0, 0, 0xffffffffffffffff}),
           {0xffffffffffffffff});
    doTest("case 8", BigInteger({0, 0, 1}), BigInteger(1), {0xffffffffffffffff, 0xffffffffffffffff});
    doTest("case 9", BigInteger({0, 1, 0xffffffffffffffff}), BigInteger(1), {0xffffffffffffffff, 0, 0xffffffffffffffff});
    doTest("case 10",
           BigInteger({0xffffffffffffffff, 1, 0xffffffffffffffff}),
           BigInteger({0, 1}),
           {0xffffffffffffffff, 0, 0xffffffffffffffff});
    doTest("case 11",
           BigInteger({0xffffffffffffffff, 0, 0, 1}),
           BigInteger({0, 0, 1}),
           {0xffffffffffffffff, 0, 0xffffffffffffffff});
    doTest("case 12",
           BigInteger({0xffffffffffffffff, 0, 0, 0, 0, 0, 0xff}),
           BigInteger({0, 0, 0, 0, 0, 0, 0xff}),
           {0xffffffffffffffff});
}

TEST(BigInteger_Tests, operator_minus_equal_uint64_t)
{
    auto doTest = [](const auto &testCaseName, auto &&data1, const auto &data2, const std::vector<uint64_t> &expected) {
        // SCOPED_TRACE(testCaseName);

        data1 -= data2;
        ASSERT_EQ(BigInteger::BigInteger_Tests::readValue(data1).size(), expected.size());
        for (size_t i = 0; i < expected.size(); ++i) {
            EXPECT_EQ(BigInteger::BigInteger_Tests::readValue(data1)[i], expected[i]);
        }
    };

    doTest("case 1", BigInteger(), 40, {0xffffffffffffffd8});
    doTest("case 2", BigInteger(20), 20, {0});
    doTest("case 3", BigInteger(4400), 200, {4200});
    doTest("case 4", BigInteger({0, 1}), 1, {0xffffffffffffffff});
    doTest("case 5", BigInteger({0xfffffffffffffffe, 1}), 0xffffffffffffffff, {0xffffffffffffffff});
}

TEST(BigInteger_Tests, add)
{
    auto doTest =
        [](const auto &testCaseName, auto &&data, uint64_t value, uint64_t index, const std::vector<uint64_t> &expected) {
            // SCOPED_TRACE(testCaseName);

            data.add(value, index);
            ASSERT_EQ(BigInteger::BigInteger_Tests::readValue(data).size(), expected.size());
            for (size_t i = 0; i < expected.size(); ++i) {
                EXPECT_EQ(BigInteger::BigInteger_Tests::readValue(data)[i], expected[i]);
            }
        };

    doTest("case 1", BigInteger(), 40, 0, {40});
    doTest("case 2", BigInteger(20), 20, 0, {40});
    doTest("case 3", BigInteger(200), 4000, 0, {4200});
    doTest("case 4", BigInteger(0xffffffffffffffff), 1, 0, {0, 1});
    doTest("case 5", BigInteger(0xffffffffffffffff), 0xffffffffffffffff, 0, {0xfffffffffffffffe, 1});
    doTest("case 6", BigInteger(0xffffffffffffffff), 0xffffffffffffffff, 1, {0xffffffffffffffff, 0xffffffffffffffff});
    doTest("case 7", BigInteger(0xffffffffffffffff), 0xffffffffffffffff, 2, {0xffffffffffffffff, 0, 0xffffffffffffffff});
    doTest("case 8", BigInteger({0xffffffffffffffff, 0xffffffffffffffff}), 1, 0, {0, 0, 1});
    doTest("case 9", BigInteger({0xffffffffffffffff, 0, 0xffffffffffffffff}), 1, 0, {0, 1, 0xffffffffffffffff});
    doTest("case 10", BigInteger({0xffffffffffffffff, 0, 0xffffffffffffffff}), 1, 1, {0xffffffffffffffff, 1, 0xffffffffffffffff});
    doTest("case 11", BigInteger({0xffffffffffffffff, 0, 0xffffffffffffffff}), 1, 2, {0xffffffffffffffff, 0, 0, 1});
    doTest("case 12", BigInteger({0xffffffffffffffff, 0}), 0xff, 6, {0xffffffffffffffff, 0, 0, 0, 0, 0, 0xff});
}

TEST(BigInteger_Tests, substract)
{
    auto doTest =
        [](const auto &testCaseName, auto &&data, uint64_t value, uint64_t index, const std::vector<uint64_t> &expected) {
            // SCOPED_TRACE(testCaseName);

            data.substract(value, index);
            ASSERT_EQ(BigInteger::BigInteger_Tests::readValue(data).size(), expected.size());
            for (size_t i = 0; i < expected.size(); ++i) {
                EXPECT_EQ(BigInteger::BigInteger_Tests::readValue(data)[i], expected[i]);
            }
        };

    doTest("case 1", BigInteger(), 40, 0, {0xffffffffffffffd8});
    doTest("case 2", BigInteger(20), 20, 0, {0});
    doTest("case 3", BigInteger(4400), 200, 0, {4200});
    doTest("case 4", BigInteger({0, 1}), 1, 0, {0xffffffffffffffff});
    doTest("case 5", BigInteger({0xfffffffffffffffe, 1}), 0xffffffffffffffff, 0, {0xffffffffffffffff});
    doTest("case 6", BigInteger({0xffffffffffffffff, 0xffffffffffffffff}), 0xffffffffffffffff, 1, {0xffffffffffffffff});
    doTest("case 7", BigInteger({0xffffffffffffffff, 0, 0xffffffffffffffff}), 0xffffffffffffffff, 2, {0xffffffffffffffff});
    doTest("case 8", BigInteger({0, 0, 1}), 1, 0, {0xffffffffffffffff, 0xffffffffffffffff});
    doTest("case 9", BigInteger({0, 1, 0xffffffffffffffff}), 1, 0, {0xffffffffffffffff, 0, 0xffffffffffffffff});
    doTest("case 10", BigInteger({0xffffffffffffffff, 1, 0xffffffffffffffff}), 1, 1, {0xffffffffffffffff, 0, 0xffffffffffffffff});
    doTest("case 11", BigInteger({0xffffffffffffffff, 0, 0, 1}), 1, 2, {0xffffffffffffffff, 0, 0xffffffffffffffff});
    doTest("case 12", BigInteger({0xffffffffffffffff, 0, 0, 0, 0, 0, 0xff}), 0xff, 6, {0xffffffffffffffff});
}

TEST(BigInteger_Tests, DISABLED_performance)
{
    // TestHelper::timeFn("1. ")
}
