#include "TestHelper.h"
#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "psi/tools/crypt/sha.h"

using namespace psi::tools;
using namespace psi::tools::crypt;

TEST(sha_Tests, padMessage)
{
    auto doTest = [](const auto &testCase, const auto &msg, const auto &expected) {
        SCOPED_TRACE(testCase);

        const ByteBuffer result = sha::padMessage(ByteBuffer(msg, true));
        EXPECT_EQ(result.asHexString(), expected);
    };

    doTest("// case 1",
           "",
           "80000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000");

    doTest("// case 2",
           "0a",
           "0a800000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000008");

    doTest("// case 3",
           "000102030405060708090a0b0c0d0e0f",
           "000102030405060708090a0b0c0d0e0f800000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000080");

    doTest("// case 4",
           "000102030405060708090a0b0c0d0e0f000102030405060708090a0b0c0d0e0f000102030405060708090a0b0c0d0e0f",
           "000102030405060708090a0b0c0d0e0f000102030405060708090a0b0c0d0e0f000102030405060708090a0b0c0d0e0f80000000000000000000000000000180");

    doTest("// case 5",
           "000102030405060708090a0b0c0d0e0f000102030405060708090a0b0c0d0e0f000102030405060708090a0b0c0d0e0f000102030405060708090a0b0c0d0e0f",
           "000102030405060708090a0b0c0d0e0f000102030405060708090a0b0c0d0e0f000102030405060708090a0b0c0d0e0f000102030405060708090a0b0c0d0e0f"
           "80000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000200");

    doTest("// case 6",
           "000102030405060708090a0b0c0d0e0f000102030405060708090a0b0c0d0e0f000102030405060708090a0b0c0d0e0f000102030405060708090a0b0c0d0e0f"
           "00010203",
           "000102030405060708090a0b0c0d0e0f000102030405060708090a0b0c0d0e0f000102030405060708090a0b0c0d0e0f000102030405060708090a0b0c0d0e0f"
           "00010203800000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000220");
}

TEST(sha_Tests, rightRotate)
{
    auto doTest = [](const auto &testCase, uint32_t v, uint8_t n, uint32_t expected) {
        SCOPED_TRACE(testCase);

        const uint32_t result = sha::rightRotate(v, n);
        EXPECT_EQ(result, expected);
    };

    // shift 1 bit
    doTest("// case 1", uint32_t(0), uint8_t(1u), uint32_t(0));
    doTest("// case 1.2", uint32_t(1), uint8_t(1u), uint32_t(-2147483648));
    doTest("// case 1.3", uint32_t(5), uint8_t(1u), uint32_t(-2147483646));
    doTest("// case 1.4", uint32_t(128), uint8_t(1u), uint32_t(64));
    doTest("// case 1.5", uint32_t(2048), uint8_t(1u), uint32_t(1024));
    doTest("// case 1.6", uint32_t(-2147483648), uint8_t(1u), uint32_t(1073741824));
    
    // shift n bits
    doTest("// case 2", uint32_t(140), uint8_t(0u), uint32_t(140));
    doTest("// case 2.1", uint32_t(140), uint8_t(1u), uint32_t(70));
    doTest("// case 2.2", uint32_t(140), uint8_t(2u), uint32_t(35));
    doTest("// case 2.3", uint32_t(140), uint8_t(3u), uint32_t(-2147483631));
    doTest("// case 2.4", uint32_t(140), uint8_t(13u), uint32_t(73400320));

    // shift n >= 32 bits
    doTest("// case 3", uint32_t(140), uint8_t(32u), uint32_t(140));
    doTest("// case 3.1", uint32_t(140), uint8_t(33u), uint32_t(70));
    doTest("// case 3.2", uint32_t(140), uint8_t(250u), uint32_t(8960));
    doTest("// case 3.3", uint32_t(-1), uint8_t(250u), uint32_t(-1));
}

TEST(sha_Tests, encode)
{
    auto doTest = [](const auto &testCase, const auto &msg, const auto &expected) {
        SCOPED_TRACE(testCase);

        const ByteBuffer result = sha::encode256(ByteBuffer(msg, true));
        EXPECT_EQ(result.asHexString(), expected);
    };

    doTest("// case 1", "", "e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855");
    doTest("// case 2", "ab0c", "15f27c6b8828e62cd3d09e8b4de5230844facfc88f1b9c34129cab7706f2f561");
    doTest("// case 3", "616263", "ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad");
    doTest("// case 4",
           "ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff",
           "af9613760f72635fbdb44a5a0a63c39f12af30f950a6ee5c971be188e89c4051");
    doTest("// case 5",
           "fffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff"
           "fffffffffffffffffffff",
           "8667e718294e9e0df1d30600ba3eeb201f764aad2dad72748643e4a285e1d1f7");
}
