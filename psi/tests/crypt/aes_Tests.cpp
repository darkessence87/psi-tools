#include "TestHelper.h"
#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <iostream>
#include <set>
#include <sstream>

#include "psi/tools/Tools.h"
#include "psi/tools/crypt/aes.h"

using namespace psi::tools;
using namespace psi::tools::crypt;

TEST(aes_Tests, subWord)
{
    auto doTest = [](uint8_t word[4], const uint8_t expectedWord[4]) {
        aes::subWord(word);

        for (uint8_t r = 0; r < 4; ++r) {
            EXPECT_EQ(expectedWord[r], word[r]);
        }
    };

    uint8_t word1[4u] = {0x00, 0x04, 0x08, 0x0c};
    const uint8_t expectedWord1[4u] = {0x63, 0xf2, 0x30, 0xfe};

    uint8_t word2[4u] = {0x10, 0x20, 0x30, 0x40};
    const uint8_t expectedWord2[4u] = {0xca, 0xb7, 0x04, 0x09};

    doTest(word1, expectedWord1);
    doTest(word2, expectedWord2);
}

void stringToBlock(const std::string &data, uint8_t (&block)[4][4])
{
    if (data.size() != 32u) {
        return;
    }

    size_t k = 0;
    for (uint8_t i = 0; i < data.size(); i += 2) {
        const std::string temp = data.substr(i, 2);
        uint8_t v = static_cast<uint8_t>(std::stoul(temp, nullptr, 16));
        const size_t r = k / 4;
        const size_t c = k % 4;
        block[c][r] = v;
        ++k;
    }
}

std::string blockToString(const uint8_t block[4][4])
{
    std::ostringstream os;

    for (size_t i = 0; i < 16u; ++i) {
        const size_t r = i / 4;
        const size_t c = i % 4;
        uint16_t n = block[c][r];
        os << psi::tools::to_hex_string(n);
    }

    return os.str();
}

TEST(aes_Tests, subBytes)
{
    aes::DataBlock16 block;
    stringToBlock("00102030405060708090a0b0c0d0e0f0", block);

    aes::subBytes(aes::m_sBox, block);

    EXPECT_EQ("63cab7040953d051cd60e0e7ba70e18c", blockToString(block));
}

TEST(aes_Tests, shiftRows)
{
    aes::DataBlock16 block;
    stringToBlock("63cab7040953d051cd60e0e7ba70e18c", block);

    aes::shiftRows(block);

    EXPECT_EQ("6353e08c0960e104cd70b751bacad0e7", blockToString(block));
}

TEST(aes_Tests, mixColumns)
{
    aes::DataBlock16 block;
    stringToBlock("d6f3d9dda6279bd1430d52a0e513f3fe", block);

    aes::mixColumns(block);

    EXPECT_EQ("bd86f0ea748fc4f4630f11c1e9331233", blockToString(block));
}

TEST(aes_Tests, applySubKey)
{
    uint8_t roundKey[16u] = {0xd0, 0x14, 0xf9, 0xa8, 0xc9, 0xee, 0x25, 0x89, 0xe1, 0x3f, 0x0c, 0xc8, 0xb6, 0x63, 0x0c, 0xa6};
    aes::DataBlock16 block;
    stringToBlock("e9317db5cb322c723d2e895faf090794", block);

    aes::applySubKey(roundKey, block);

    EXPECT_EQ("3925841d02dc09fbdc118597196a0b32", blockToString(block));
}

TEST(aes_Tests, writeBlock)
{
    uint8_t data[16] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f};
    aes::DataBlock16 block;
    aes::writeBlock(data, 16, block);

    EXPECT_EQ(block[0][0], data[0]);
    EXPECT_EQ(block[1][0], data[1]);
    EXPECT_EQ(block[2][0], data[2]);
    EXPECT_EQ(block[3][0], data[3]);
    EXPECT_EQ(block[0][1], data[4]);
    EXPECT_EQ(block[1][1], data[5]);
    EXPECT_EQ(block[2][1], data[6]);
    EXPECT_EQ(block[3][1], data[7]);
    EXPECT_EQ(block[0][2], data[8]);
    EXPECT_EQ(block[1][2], data[9]);
    EXPECT_EQ(block[2][2], data[10]);
    EXPECT_EQ(block[3][2], data[11]);
    EXPECT_EQ(block[0][3], data[12]);
    EXPECT_EQ(block[1][3], data[13]);
    EXPECT_EQ(block[2][3], data[14]);
    EXPECT_EQ(block[3][3], data[15]);
}

TEST(aes_Tests, readBlock)
{
    aes::DataBlock16 block {{0x00, 0x04, 0x08, 0x0c},
                            {0x01, 0x05, 0x09, 0x0d},
                            {0x02, 0x06, 0x0a, 0x0e},
                            {0x03, 0x07, 0x0b, 0x0f}};
    uint8_t data[16] = {'\0'};
    aes::readBlock(block, data, 16);

    EXPECT_EQ(block[0][0], data[0]);
    EXPECT_EQ(block[1][0], data[1]);
    EXPECT_EQ(block[2][0], data[2]);
    EXPECT_EQ(block[3][0], data[3]);
    EXPECT_EQ(block[0][1], data[4]);
    EXPECT_EQ(block[1][1], data[5]);
    EXPECT_EQ(block[2][1], data[6]);
    EXPECT_EQ(block[3][1], data[7]);
    EXPECT_EQ(block[0][2], data[8]);
    EXPECT_EQ(block[1][2], data[9]);
    EXPECT_EQ(block[2][2], data[10]);
    EXPECT_EQ(block[3][2], data[11]);
    EXPECT_EQ(block[0][3], data[12]);
    EXPECT_EQ(block[1][3], data[13]);
    EXPECT_EQ(block[2][3], data[14]);
    EXPECT_EQ(block[3][3], data[15]);
}

TEST(aes_Tests, rotWord)
{
    uint8_t block[4] = {0x01, 0x02, 0x03, 0x04};
    aes::rotWord(block);
    ASSERT_EQ(block[0], 0x02);
    ASSERT_EQ(block[1], 0x03);
    ASSERT_EQ(block[2], 0x04);
    ASSERT_EQ(block[3], 0x01);

    // psi::test::TestHelper::timeFn("rotWord_old", [&]() { aes::rotWord(block); }, 100'000'000);
    // psi::test::TestHelper::timeFn("rotWord_new", [&]() { aes::rotWord2(block); }, 100'000'000);
}

TEST(aes_Tests, generateSubKeys_impl_AES128)
{
    const ByteBuffer
        expectedEnhancedKey("2b7e151628aed2a6abf7158809cf4f3ca0fafe1788542cb123a339392a6c7605f2c295f27a96b9435935807a73"
                            "59f67f3d80477d4716fe3e1e237e446d7a883bef44a541a8525b7fb671253bdb0bad00d4d1c6f87c839d87caf2"
                            "b8bc11f915bc6d88a37a110b3efddbf98641ca0093fd4e54f70e5f5fc9f384a64fb24ea6dc4fead27321b58dba"
                            "d2312bf5607f8d292fac7766f319fadc2128d12941575c006ed014f9a8c9ee2589e13f0cc8b6630ca6",
                            true);

    const ByteBuffer key("2b7e151628aed2a6abf7158809cf4f3c", true);
    aes::SubKey subKeys[11u] = {};
    aes::generateSubKeys_impl<4, 10>(key.data(), subKeys);
    ByteBuffer enhancedKey(16u * 11u);
    for (size_t i = 0; i < enhancedKey.size(); ++i) {
        enhancedKey.write(subKeys[i]);
    }
    EXPECT_EQ(expectedEnhancedKey.asHexStringFormatted(), enhancedKey.asHexStringFormatted());
}

TEST(aes_Tests, encryptAes_impl_AES128_rawData)
{
    ByteBuffer key(16u);
    key.writeHexString("000102030405060708090a0b0c0d0e0f");

    uint8_t rawData[] = {0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff};
    const auto encodedData = aes::encryptAes_impl<4, 10>(rawData, 16u, key);
    EXPECT_EQ(encodedData.asHexString(), "69c4e0d86a7b0430d8cdb78070b4c55a");
}

TEST(aes_Tests, encryptAes_impl_AES128_ByteBuffer)
{
    const ByteBuffer key_("000102030405060708090a0b0c0d0e0f", true);

    {
        SCOPED_TRACE("// case 1. two full blocks + one non-full block");

        ByteBuffer data(47u);
        data.writeHexString(
            "00112233445566778899aabbccddeeff00112233445566778899aabbccddeeff00112233445566778899aabbccddee");

        const auto encodedData = aes::encryptAes_impl<4, 10>(data, key_);
        EXPECT_EQ(encodedData.asHexString(),
                  "69c4e0d86a7b0430d8cdb78070b4c55a69c4e0d86a7b0430d8cdb78070b4c55a7c99f42b6ee503309c6c1a67e97ac2420f");
    }

    {
        SCOPED_TRACE("// case 2. one non-full block");

        ByteBuffer data(15u);
        data.writeHexString("00112233445566778899aabbccddee");

        const auto encodedData = aes::encryptAes_impl<4, 10>(data, key_);
        EXPECT_EQ(encodedData.asHexString(), "7c99f42b6ee503309c6c1a67e97ac2420f");
    }

    {
        SCOPED_TRACE("// case 3. full blocks");

        {
            ByteBuffer key("000102030405060708090a0b0c0d0e0f", true);
            ByteBuffer data("00112233445566778899aabbccddeeff", true);
            const auto encodedData = aes::encryptAes_impl<4, 10>(data, key);
            EXPECT_EQ(encodedData.asHexString(), "69c4e0d86a7b0430d8cdb78070b4c55a");
        }

        {
            ByteBuffer key("2b7e151628aed2a6abf7158809cf4f3c", true);
            ByteBuffer data("3243f6a8885a308d313198a2e0370734", true);
            const auto encodedData = aes::encryptAes_impl<4, 10>(data, key);
            EXPECT_EQ(encodedData.asHexString(), "3925841d02dc09fbdc118597196a0b32");
        }

        {
            ByteBuffer key("5468617473206d79204b756e67204675", true);
            ByteBuffer data("5468617473206d79204b756e67204675", true);
            const auto encodedData = aes::encryptAes_impl<4, 10>(data, key);
            EXPECT_EQ(encodedData.asHexString(), "fe4d306a177d577d2a68d16fa2aacf9d");
        }

        {
            ByteBuffer key("00000000000000000000000000000000", true);
            ByteBuffer data("00000000000000000000000000000000", true);
            const auto encodedData = aes::encryptAes_impl<4, 10>(data, key);
            EXPECT_EQ(encodedData.asHexString(), "66e94bd4ef8a2c3b884cfa59ca342b2e");
        }

        {
            ByteBuffer key("00000000000000000000000000000000", true);
            ByteBuffer data("80000000000000000000000000000000", true);
            const auto encodedData = aes::encryptAes_impl<4, 10>(data, key);
            EXPECT_EQ(encodedData.asHexString(), "3ad78e726c1ec02b7ebfe92b23d9ec34");
        }
    }
}

TEST(aes_Tests, decryptAes_impl_AES128)
{
    ByteBuffer key(16u);
    key.writeHexString("000102030405060708090a0b0c0d0e0f");

    {
        SCOPED_TRACE("// case 1. two full blocks + one non-full block");

        ByteBuffer data(49u);
        data.writeHexString(
            "69c4e0d86a7b0430d8cdb78070b4c55a69c4e0d86a7b0430d8cdb78070b4c55a7c99f42b6ee503309c6c1a67e97ac2420f");

        const auto decodedData = aes::decryptAes_impl<4, 10>(data, key);
        EXPECT_EQ(decodedData.asHexString(),
                  "00112233445566778899aabbccddeeff00112233445566778899aabbccddeeff00112233445566778899aabbccddee");
    }

    {
        SCOPED_TRACE("// case 2. one non-full block");

        ByteBuffer data(17u);
        data.writeHexString("7c99f42b6ee503309c6c1a67e97ac2420f");

        const auto decodedData = aes::decryptAes_impl<4, 10>(data, key);
        EXPECT_EQ(decodedData.asHexString(), "00112233445566778899aabbccddee");
    }

    {
        SCOPED_TRACE("// case 3. one full block");

        ByteBuffer data(16u);
        data.writeHexString("69c4e0d86a7b0430d8cdb78070b4c55a");

        const auto decodedData = aes::decryptAes_impl<4, 10>(data, key);
        EXPECT_EQ(decodedData.asHexString(), "00112233445566778899aabbccddeeff");
    }
}

TEST(aes_Tests, generateSubKeys_impl_AES256)
{
    const ByteBuffer expectedEnhancedKey(
        "603deb1015ca71be2b73aef0857d77811f352c073b6108d72d9810a30914dff49ba354118e6925afa51a8b5f2067fcdea8b09c1a93d194"
        "cdbe49846eb75d5b9ad59aecb85bf3c917fee94248de8ebe96b5a9328a2678a647983122292f6c79b3812c81addadf48ba24360af2fab8"
        "b46498c5bfc9bebd198e268c3ba709e0421468007bacb2df331696e939e46c518d80c814e20476a9fb8a5025c02d59c58239de1369676c"
        "cc5a71fa2563959674ee155886ca5d2e2f31d77e0af1fa27cf73c3749c47ab18501ddae2757e4f7401905acafaaae3e4d59b349adf6ace"
        "bd10190dfe4890d1e6188d0b046df344706c631e",
        true);

    const ByteBuffer key("603deb1015ca71be2b73aef0857d77811f352c073b6108d72d9810a30914dff4", true);
    aes::SubKey subKeys[15u] = {};
    aes::generateSubKeys_impl<8, 14>(key.data(), subKeys);
    ByteBuffer enhancedKey(16u * 15u);
    for (size_t i = 0; i < enhancedKey.size(); ++i) {
        enhancedKey.write(subKeys[i]);
    }
    EXPECT_EQ(expectedEnhancedKey.asHexStringFormatted(), enhancedKey.asHexStringFormatted());
}

TEST(aes_Tests, encryptAes_impl_AES256)
{
    ByteBuffer key(32u);
    key.writeHexString("000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f");

    {
        SCOPED_TRACE("// case 1. two full blocks + one non-full block");

        ByteBuffer data(47u);
        data.writeHexString(
            "00112233445566778899aabbccddeeff00112233445566778899aabbccddeeff00112233445566778899aabbccddee");

        const auto encodedData = aes::encryptAes_impl<8, 14>(data, key);
        EXPECT_EQ(encodedData.asHexString(),
                  "8ea2b7ca516745bfeafc49904b4960898ea2b7ca516745bfeafc49904b49608994501d9b894874a1f7feae67d905c45b0f");
    }

    {
        SCOPED_TRACE("// case 2. one non-full block");

        ByteBuffer data(15u);
        data.writeHexString("00112233445566778899aabbccddee");

        const auto encodedData = aes::encryptAes_impl<8, 14>(data, key);
        EXPECT_EQ(encodedData.asHexString(), "94501d9b894874a1f7feae67d905c45b0f");
    }

    {
        SCOPED_TRACE("// case 3. one full block");

        ByteBuffer data(16u);
        data.writeHexString("00112233445566778899aabbccddeeff");

        const auto encodedData = aes::encryptAes_impl<8, 14>(data, key);
        EXPECT_EQ(encodedData.asHexString(), "8ea2b7ca516745bfeafc49904b496089");
    }
}

TEST(aes_Tests, decryptAes_impl_AES256)
{
    ByteBuffer key(32u);
    key.writeHexString("000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f");

    {
        SCOPED_TRACE("// case 1. two full blocks + one non-full block");

        ByteBuffer data(49u);
        data.writeHexString(
            "8ea2b7ca516745bfeafc49904b4960898ea2b7ca516745bfeafc49904b49608994501d9b894874a1f7feae67d905c45b0f");

        const auto decodedData = aes::decryptAes_impl<8, 14>(data, key);
        EXPECT_EQ(decodedData.asHexString(),
                  "00112233445566778899aabbccddeeff00112233445566778899aabbccddeeff00112233445566778899aabbccddee");
    }

    {
        SCOPED_TRACE("// case 2. one non-full block");

        ByteBuffer data(17u);
        data.writeHexString("94501d9b894874a1f7feae67d905c45b0f");

        const auto decodedData = aes::decryptAes_impl<8, 14>(data, key);
        EXPECT_EQ(decodedData.asHexString(), "00112233445566778899aabbccddee");
    }

    {
        SCOPED_TRACE("// case 3. one full block");

        ByteBuffer data(16u);
        data.writeHexString("8ea2b7ca516745bfeafc49904b496089");

        const auto decodedData = aes::decryptAes_impl<8, 14>(data, key);
        EXPECT_EQ(decodedData.asHexString(), "00112233445566778899aabbccddeeff");
    }
}
