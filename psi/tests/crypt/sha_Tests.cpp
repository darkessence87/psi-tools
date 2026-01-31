#include "psi/test/TestHelper.h"
#include "psi/test/psi_mock.h"

#include "psi/tools/crypt/sha.h"

using namespace psi::tools;
using namespace psi::tools::crypt;
using namespace psi::test;

TEST(sha_Tests, padMessage)
{
    auto doTest = [](const auto &testCase, const auto &msg, const auto &expected) {
        // SCOPED_TRACE(testCase);

        const ByteBuffer result = sha::padMessage(ByteBuffer(msg, true));
        EXPECT_EQ(result.asHexString(), expected);
    };

    doTest("// case 1", "", "80000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000");

    doTest("// case 2", "0a", "0a800000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000008");

    doTest("// case 3", "000102030405060708090a0b0c0d0e0f", "000102030405060708090a0b0c0d0e0f800000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000080");

    doTest("// case 4",
           "000102030405060708090a0b0c0d0e0f000102030405060708090a0b0c0d0e0f000102030405060708090a0b0c0d0e0f",
           "000102030405060708090a0b0c0d0e0f000102030405060708090a0b0c0d0e0f000102030405060708090a0b0c0d0e0f80000000000"
           "000000000000000000180");

    doTest("// case 5",
           "000102030405060708090a0b0c0d0e0f000102030405060708090a0b0c0d0e0f000102030405060708090a0b0c0d0e0f00010203040"
           "5060708090a0b0c0d0e0f",
           "000102030405060708090a0b0c0d0e0f000102030405060708090a0b0c0d0e0f000102030405060708090a0b0c0d0e0f00010203040"
           "5060708090a0b0c0d0e0f"
           "80000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
           "000000000000000000200");

    doTest("// case 6",
           "000102030405060708090a0b0c0d0e0f000102030405060708090a0b0c0d0e0f000102030405060708090a0b0c0d0e0f00010203040"
           "5060708090a0b0c0d0e0f"
           "00010203",
           "000102030405060708090a0b0c0d0e0f000102030405060708090a0b0c0d0e0f000102030405060708090a0b0c0d0e0f00010203040"
           "5060708090a0b0c0d0e0f"
           "00010203800000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
           "000000000000000000220");
}

TEST(sha_Tests, rightRotate)
{
    auto doTest = [](const auto &testCase, uint32_t v, uint8_t n, uint32_t expected) {
        // SCOPED_TRACE(testCase);

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

TEST(sha_Tests, encode256)
{
    auto doTest = [](const auto &testCase, const auto &msg, const auto &expected) {
        // SCOPED_TRACE(testCase);

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

TEST(sha_Tests, hmac256)
{
    auto doTest = [](const auto &testCase, const auto &key, const auto &msg, const auto &expected) {
        // SCOPED_TRACE(testCase);

        const ByteBuffer result = sha::hmac256(ByteBuffer(key, true), ByteBuffer(msg, true));
        EXPECT_EQ(result.asHexString(), expected);
    };

    doTest("// case 1",
           "0b0b0b0b0b0b0b0b0b0b0b0b0b0b0b0b0b0b0b0b",
           "4869205468657265",
           "b0344c61d8db38535ca8afceaf0bf12b881dc200c9833da726e9376c2e32cff7");
    doTest("// case 2",
           "4a656665",
           "7768617420646f2079612077616e7420666f72206e6f7468696e673f",
           "5bdcc146bf60754e6a042426089575c75a003f089d2739839dec58b964ec3843");
    doTest("// case 3",
           "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa",
           "dddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddd",
           "773ea91e36800e46854db8ebd09181a72959098b3ef8c122d9635514ced565fe");
    doTest("// case 4",
           "0102030405060708090a0b0c0d0e0f10111213141516171819",
           "cdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcd",
           "82558a389a443c0ea4cc819899f2083a85f0faa3e578f8077a2e3ff46729665b");
    doTest(
        "// case 5",
        "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
        "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
        "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa",
        "54657374205573696e67204c6172676572205468616e20426c6f636b2d53697a65204b6579202d2048617368204b6579204669727374",
        "60e431591ee0b67f0d8a26aacbf5b77f8e0bc6213728c5140546040f0ee37f54");
    doTest(
        "// case 6",
        "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
        "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
        "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa",
        "5468697320697320612074657374207573696e672061206c6172676572207468616e20626c6f636b2d73697a65206b657920616e642061"
        "206c6172676572207468616e20626c6f636b2d73697a6520646174612e20546865206b6579206e6565647320746f206265206861736865"
        "64206265666f7265206265696e6720757365642062792074686520484d414320616c676f726974686d2e",
        "9b09ffa71b942fcb27635fbcd5b0e944bfdc63644f0713938a7f51535c3a35e2");
}

TEST(sha_Tests, hkdf256Extract)
{
    auto doTest = [](const auto &testCase, const auto &kMat, const auto &seed, const auto &expected) {
        // SCOPED_TRACE(testCase);

        const auto result = sha::hkdf256Extract(ByteBuffer(kMat, true), ByteBuffer(seed, true));
        EXPECT_EQ(result.asHexString(), expected);
    };

    doTest("// case 1",
           "4869205468657265",
           "0b0b0b0b0b0b0b0b0b0b0b0b0b0b0b0b0b0b0b0b",
           "b0344c61d8db38535ca8afceaf0bf12b881dc200c9833da726e9376c2e32cff7");
    doTest("// case 2",
           "0b0b0b0b0b0b0b0b0b0b0b0b0b0b0b0b0b0b0b0b0b0b",
           "000102030405060708090a0b0c",
           "077709362c2e32df0ddc3f0dc47bba6390b6c73bb50f9c3122ec844ad7c2b3e5");
    doTest("// case 3",
           "000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f202122232425262728292a2b2c2d2e2f30313233343"
           "5363738393a3b3c3d3e3f404142434445464748494a4b4c4d4e4f",
           "606162636465666768696a6b6c6d6e6f707172737475767778797a7b7c7d7e7f808182838485868788898a8b8c8d8e8f90919293949"
           "5969798999a9b9c9d9e9fa0a1a2a3a4a5a6a7a8a9aaabacadaeaf",
           "06a6b88c5853361a06104c9ceb35b45cef760014904671014a193f40c15fc244");
    doTest("// case 4",
           "0b0b0b0b0b0b0b0b0b0b0b0b0b0b0b0b0b0b0b0b0b0b",
           "",
           "19ef24a32c717b167f33a91d6f648bdf96596776afdb6377ac434c1c293ccb04");
}

TEST(sha_Tests, hkdf256Expand)
{
    auto doTest = [](const auto &testCase, const auto &prk, const auto &info, size_t len, const auto &expected) {
        // SCOPED_TRACE(testCase);

        const auto result = sha::hkdf256Expand(ByteBuffer(prk, true), ByteBuffer(info, true), len);
        EXPECT_EQ(result.asHexString(), expected);
    };

    doTest("// case 1",
           "077709362c2e32df0ddc3f0dc47bba6390b6c73bb50f9c3122ec844ad7c2b3e5",
           "f0f1f2f3f4f5f6f7f8f9",
           42,
           "3cb25f25faacd57a90434f64d0362f2a2d2d0a90cf1a5a4c5db02d56ecc4c5bf34007208d5b887185865");
    doTest("// case 2",
           "06a6b88c5853361a06104c9ceb35b45cef760014904671014a193f40c15fc244",
           "b0b1b2b3b4b5b6b7b8b9babbbcbdbebfc0c1c2c3c4c5c6c7c8c9cacbcccdcecfd0d1d2d3d4d5d6d7d8d9dadbdcdddedfe0e1e2e3e4e"
           "5e6e7e8e9eaebecedeeeff0f1f2f3f4f5f6f7f8f9fafbfcfdfeff",
           82,
           "b11e398dc80327a1c8e7f78c596a49344f012eda2d4efad8a050cc4c19afa97c59045a99cac7827271cb41c65e590e09da3275600c2"
           "f09b8367793a9aca3db71cc30c58179ec3e87c14c01d5c1f3434f1d87");
    doTest("// case 3",
           "19ef24a32c717b167f33a91d6f648bdf96596776afdb6377ac434c1c293ccb04",
           "",
           42,
           "8da4e775a563c18f715f802a063c5a31b8a11f5c5ee1879ec3454e5f3c738d2d9d201395faa4b61a96c8");
}

TEST(sha_Tests, hkdf256)
{
    auto doTest =
        [](const auto &testCase, const auto &key, const auto &seed, const auto &info, size_t len, const auto &expected) {
            // SCOPED_TRACE(testCase);

            const auto result = sha::hkdf256(ByteBuffer(key, true), ByteBuffer(seed, true), ByteBuffer(info, true), len);
            EXPECT_EQ(result.asHexString(), expected);
        };

    doTest("// case 1",
           "0b0b0b0b0b0b0b0b0b0b0b0b0b0b0b0b0b0b0b0b0b0b",
           "000102030405060708090a0b0c",
           "f0f1f2f3f4f5f6f7f8f9",
           42,
           "3cb25f25faacd57a90434f64d0362f2a2d2d0a90cf1a5a4c5db02d56ecc4c5bf34007208d5b887185865");
    doTest("// case 2",
           "000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f202122232425262728292a2b2c2d2e2f30313233343"
           "5363738393a3b3c3d3e3f404142434445464748494a4b4c4d4e4f",
           "606162636465666768696a6b6c6d6e6f707172737475767778797a7b7c7d7e7f808182838485868788898a8b8c8d8e8f90919293949"
           "5969798999a9b9c9d9e9fa0a1a2a3a4a5a6a7a8a9aaabacadaeaf",
           "b0b1b2b3b4b5b6b7b8b9babbbcbdbebfc0c1c2c3c4c5c6c7c8c9cacbcccdcecfd0d1d2d3d4d5d6d7d8d9dadbdcdddedfe0e1e2e3e4e"
           "5e6e7e8e9eaebecedeeeff0f1f2f3f4f5f6f7f8f9fafbfcfdfeff",
           82,
           "b11e398dc80327a1c8e7f78c596a49344f012eda2d4efad8a050cc4c19afa97c59045a99cac7827271cb41c65e590e09da3275600c2"
           "f09b8367793a9aca3db71cc30c58179ec3e87c14c01d5c1f3434f1d87");
    doTest("// case 3",
           "0b0b0b0b0b0b0b0b0b0b0b0b0b0b0b0b0b0b0b0b0b0b",
           "",
           "",
           42,
           "8da4e775a563c18f715f802a063c5a31b8a11f5c5ee1879ec3454e5f3c738d2d9d201395faa4b61a96c8");
}

TEST(sha_Tests, performance)
{
    TestHelper::timeFn(
        "hashInit",
        []() {
            uint32_t h[8] = {};
            sha::hashInit(h);
        },
        100000);

    TestHelper::timeFn(
        "prepareMessageSchedule",
        []() {
            uint8_t dataBlock[64] = {};
            uint32_t w[64] = {};
            sha::prepareMessageSchedule(dataBlock, w);
        },
        100000);

    TestHelper::timeFn(
        "hmac256",
        []() {
            ByteBuffer key(32);
            ByteBuffer data(32);
            sha::hmac256(key, data);
        },
        100000);

    TestHelper::timeFn(
        "encode256 small (32 bytes)",
        []() {
            ByteBuffer data(32);
            sha::encode256(data);
        },
        100000);

    TestHelper::timeFn(
        "encode256 medium (2048 bytes)",
        []() {
            ByteBuffer data(2048);
            sha::encode256(data);
        },
        100000);
}
