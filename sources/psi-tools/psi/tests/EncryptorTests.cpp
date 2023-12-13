#include "TestHelper.h"
#include <gmock/gmock.h>
#include <gtest/gtest.h>


#include <iostream>
#include <set>
#include <sstream>

#include "psi/tools/Encryptor.h"
#include "psi/tools/Tools.h"

using namespace psi::tools;

TEST(EncryptorTests, Base64EncryptionDecryption_Success)
{
    auto doTest = [](const std::string &originMessage, const std::string &expectedEncryptedMessage) {
        ByteBuffer msg(originMessage.size());
        msg.writeString(originMessage);

        ByteBuffer encryptedMessage = Encryptor::encryptBase64(msg);
        EXPECT_EQ(encryptedMessage.asString(), expectedEncryptedMessage);

        ByteBuffer decryptedMessage = Encryptor::decryptBase64(encryptedMessage);
        EXPECT_EQ(decryptedMessage.asString(), originMessage);
    };

    // 0 padding
    doTest("Coding is my passion!", "Q29kaW5nIGlzIG15IHBhc3Npb24h");

    // 1 padding
    doTest("Hello world", "SGVsbG8gd29ybGQ=");

    // 2 padding
    doTest("Best base64 string!", "QmVzdCBiYXNlNjQgc3RyaW5nIQ==");

    // long test msg
    doTest("Kazhdiy god 31-go dekabrya mi s druz'yami hodim v banu, nu, moemsya mi tam (Russkiy yazik ne "
           "podderzhivaetsya)",
           "S2F6aGRpeSBnb2QgMzEtZ28gZGVrYWJyeWEgbWkgcyBkcnV6J3lhbWkgaG9kaW0gdiBiYW51LCBudSwgbW9lbXN5YSBtaSB0YW0gKFJ1c3N"
           "raXkgeWF6aWsgbmUgcG9kZGVyemhpdmFldHN5YSk=");
}

TEST(EncryptorTests, Base64Decryption_Failed)
{
    ByteBuffer encryptedMessage(0u);
    ByteBuffer decryptedMessage = Encryptor::decryptBase64(encryptedMessage);
    EXPECT_TRUE(decryptedMessage.size() == 0);
}

TEST(EncryptorTests, Aes256_GenerateRoundKeys)
{
    const uint8_t sampleKey1[32] = {
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    };
    const std::string expectedEnhancedKey1 = "[\
00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 \
62 63 63 63 62 63 63 63 62 63 63 63 62 63 63 63 aa fb fb fb aa fb fb fb aa fb fb fb aa fb fb fb \
6f 6c 6c cf 0d 0f 0f ac 6f 6c 6c cf 0d 0f 0f ac 7d 8d 8d 6a d7 76 76 91 7d 8d 8d 6a d7 76 76 91 \
53 54 ed c1 5e 5b e2 6d 31 37 8e a2 3c 38 81 0e 96 8a 81 c1 41 fc f7 50 3c 71 7a 3a eb 07 0c ab \
9e aa 8f 28 c0 f1 6d 45 f1 c6 e3 e7 cd fe 62 e9 2b 31 2b df 6a cd dc 8f 56 bc a6 b5 bd bb aa 1e \
64 06 fd 52 a4 f7 90 17 55 31 73 f0 98 cf 11 19 6d bb a9 0b 07 76 75 84 51 ca d3 31 ec 71 79 2f \
e7 b0 e8 9c 43 47 78 8b 16 76 0b 7b 8e b9 1a 62 74 ed 0b a1 73 9b 7e 25 22 51 ad 14 ce 20 d4 3b \
10 f8 0a 17 53 bf 72 9c 45 c9 79 e7 cb 70 63 85 \
]";

    const uint8_t sampleKey2[32] = {
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    };
    const std::string expectedEnhancedKey2 = "[\
ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff \
e8 e9 e9 e9 17 16 16 16 e8 e9 e9 e9 17 16 16 16 0f b8 b8 b8 f0 47 47 47 0f b8 b8 b8 f0 47 47 47 \
4a 49 49 65 5d 5f 5f 73 b5 b6 b6 9a a2 a0 a0 8c 35 58 58 dc c5 1f 1f 9b ca a7 a7 23 3a e0 e0 64 \
af a8 0a e5 f2 f7 55 96 47 41 e3 0c e5 e1 43 80 ec a0 42 11 29 bf 5d 8a e3 18 fa a9 d9 f8 1a cd \
e6 0a b7 d0 14 fd e2 46 53 bc 01 4a b6 5d 42 ca a2 ec 6e 65 8b 53 33 ef 68 4b c9 46 b1 b3 d3 8b \
9b 6c 8a 18 8f 91 68 5e dc 2d 69 14 6a 70 2b de a0 bd 9f 78 2b ee ac 97 43 a5 65 d1 f2 16 b6 5a \
fc 22 34 91 73 b3 5c cf af 9e 35 db c5 ee 1e 05 06 95 ed 13 2d 7b 41 84 6e de 24 55 9c c8 92 0f \
54 6d 42 4f 27 de 1e 80 88 40 2b 5b 4d ae 35 5e \
]";

    const uint8_t sampleKey3[32] = {
        0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
        0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f,
    };
    const std::string expectedEnhancedKey3 = "[\
00 01 02 03 04 05 06 07 08 09 0a 0b 0c 0d 0e 0f 10 11 12 13 14 15 16 17 18 19 1a 1b 1c 1d 1e 1f \
a5 73 c2 9f a1 76 c4 98 a9 7f ce 93 a5 72 c0 9c 16 51 a8 cd 02 44 be da 1a 5d a4 c1 06 40 ba de \
ae 87 df f0 0f f1 1b 68 a6 8e d5 fb 03 fc 15 67 6d e1 f1 48 6f a5 4f 92 75 f8 eb 53 73 b8 51 8d \
c6 56 82 7f c9 a7 99 17 6f 29 4c ec 6c d5 59 8b 3d e2 3a 75 52 47 75 e7 27 bf 9e b4 54 07 cf 39 \
0b dc 90 5f c2 7b 09 48 ad 52 45 a4 c1 87 1c 2f 45 f5 a6 60 17 b2 d3 87 30 0d 4d 33 64 0a 82 0a \
7c cf f7 1c be b4 fe 54 13 e6 bb f0 d2 61 a7 df f0 1a fa fe e7 a8 29 79 d7 a5 64 4a b3 af e6 40 \
25 41 fe 71 9b f5 00 25 88 13 bb d5 5a 72 1c 0a 4e 5a 66 99 a9 f2 4f e0 7e 57 2b aa cd f8 cd ea \
24 fc 79 cc bf 09 79 e9 37 1a c2 3c 6d 68 de 36 \
]";

    auto test = [](const uint8_t keySample[32u], const std::string &expectedEnhancedKey) {
        ByteBuffer key(32u);
        key.writeArray(keySample, 32u);

        Encryptor::SubKeys256 subKeys = {};
        Encryptor::generateSubKeys(key.data(), subKeys);

        ByteBuffer enhancedKey(16u * 15u);
        for (size_t i = 0; i < 15u; ++i) {
            for (size_t j = 0; j < 16u; ++j) {
                enhancedKey.write(subKeys[i][j]);
            }
        }
        EXPECT_EQ(expectedEnhancedKey, enhancedKey.asHexStringFormatted());
    };

    test(sampleKey1, expectedEnhancedKey1);
    test(sampleKey2, expectedEnhancedKey2);
    test(sampleKey3, expectedEnhancedKey3);
}

TEST(EncryptorTests, subWord)
{
    auto doTest = [](uint8_t word[4], const uint8_t expectedWord[4]) {
        Encryptor::subWord(word);

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

TEST(EncryptorTests, scheduleKey)
{
    auto doTest = [](uint8_t word[4], const uint8_t expectedWord[4]) {
        Encryptor::scheduleKey(word, 0);

        for (uint8_t r = 0; r < 4; ++r) {
            EXPECT_EQ(expectedWord[r], word[r]);
        }
    };

    uint8_t word1[4u] = {0x00, 0x04, 0x08, 0x0c};
    const uint8_t expectedWord1[4u] = {0xf3, 0x30, 0xfe, 0x63};

    uint8_t word2[4u] = {0x10, 0x20, 0x30, 0x40};
    const uint8_t expectedWord2[4u] = {0xb6, 0x04, 0x09, 0xca};

    doTest(word1, expectedWord1);
    doTest(word2, expectedWord2);
}

TEST(EncryptorTests, Aes256EncryptionDecryption)
{
    auto doTest = [](const std::string &hexMessage, const std::string &hexKey, const std::string &expectedHexCipher) {
        ByteBuffer message(16u);
        message.writeHexString(hexMessage);

        ByteBuffer key(32u);
        key.writeHexString(hexKey);

        auto encryptedMessage = Encryptor::encryptAes256(message, key);
        ASSERT_TRUE(encryptedMessage.size() > 0);
        EXPECT_EQ(encryptedMessage.asHexString(), expectedHexCipher);

        auto decryptedMessage = Encryptor::decryptAes256(encryptedMessage, key);
        ASSERT_TRUE(decryptedMessage.size() > 0);
        EXPECT_EQ(decryptedMessage.asHexString(), hexMessage);
    };

    doTest("00112233445566778899aabbccddeeff",
           "000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f",
           "8ea2b7ca516745bfeafc49904b496089");
    doTest("f69f2445df4f9b17ad2b417be66c3710",
           "603deb1015ca71be2b73aef0857d77811f352c073b6108d72d9810a30914dff4",
           "23304b7a39f9f3ff067d8d8f9e24ecc7");
    doTest("ae2d8a571e03ac9c9eb76fac45af8e51",
           "603deb1015ca71be2b73aef0857d77811f352c073b6108d72d9810a30914dff4",
           "591ccb10d410ed26dc5ba74a31362870");
}

TEST(EncryptorTests, Aes256BigDataEncryptionDecryption)
{
    auto doTest = [](const std::string &hexMessage, const std::string &hexKey, const std::string &expectedHexCipher) {
        ASSERT_EQ(hexMessage.size() % 2, 0u);

        ByteBuffer message(hexMessage.size() / 2);
        message.writeHexString(hexMessage);

        ByteBuffer key(32u);
        key.writeHexString(hexKey);

        auto encryptedMessage = Encryptor::encryptAes256(message, key);
        ASSERT_TRUE(encryptedMessage.size() > 0);
        EXPECT_EQ(encryptedMessage.asHexString(), expectedHexCipher);

        auto decryptedMessage = Encryptor::decryptAes256(encryptedMessage, key);
        ASSERT_TRUE(decryptedMessage.size() > 0);
        EXPECT_EQ(decryptedMessage.asHexString(), hexMessage);
    };

    doTest("0123456789",
           "000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f",
           "f86c87eaab99b58893f54438918fc1a005");
    doTest("00112233445566778899aabbccddeeff00112233445566778899aabbccddeeff",
           "000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f",
           "8ea2b7ca516745bfeafc49904b4960898ea2b7ca516745bfeafc49904b496089");
    doTest("0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef",
           "000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f",
           "b7e7f4e4da5004021090a21cdf555652b7e7f4e4da5004021090a21cdf555652");
    doTest("0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdeff0",
           "000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f",
           "b7e7f4e4da5004021090a21cdf555652b7e7f4e4da5004021090a21cdf555652569194267f16d6fef58c71f9dcbd8d5001");
}

TEST(EncryptorTests, Aes256NonStringDataEncryptionDecryption)
{
    auto doTest = [](const char *msg, const size_t msgSz, const std::string &hexKey) {
        ByteBuffer message(msgSz);
        message.writeArray(msg, msgSz);

        ByteBuffer key(32u);
        key.writeHexString(hexKey);

        auto encryptedMessage = Encryptor::encryptAes256(message, key);
        ASSERT_TRUE(encryptedMessage.size() > 0);

        auto decryptedMessage = Encryptor::decryptAes256(encryptedMessage, key);
        ASSERT_TRUE(decryptedMessage.size() > 0);

        EXPECT_EQ(std::string(msg), decryptedMessage.asString());
    };

    const char *key = "000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f";
    doTest("0123456789", 10, key);
    doTest("0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef", 64, key);
    doTest("0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef77", 66, key);

    std::ostringstream bigData;
    for (size_t i = 0; i < 32769; ++i) {
        bigData << 'a';
    }
    doTest(bigData.str().c_str(), bigData.str().size(), key);
}

TEST(EncryptorTests, DISABLED_performanceCompare)
{
    ByteBuffer data;
    data.writeString("performanceCompare");
    const auto &key = Encryptor::generateSessionKey();

    const auto &encoded_1 = Encryptor::encryptAes256(data, key);
    const auto &encoded_2 = Encryptor::encryptAes256(data, key);
    ASSERT_EQ(encoded_1.asHexString(), encoded_2.asHexString());

    const auto &decoded = Encryptor::decryptAes256(encoded_2, key);
    ASSERT_EQ(decoded.asHexString(), data.asHexString());

    // TestHelper::timeFn(
    //     "encryptAes256", [&]() { Encryptor::encryptAes256(data, key); }, 100000);
    // TestHelper::timeFn(
    //     "decryptAes256", [&]() { Encryptor::decryptAes256(encoded_2, key); }, 100000);
}

void stringToSubKey(const std::string &data, uint8_t (&key)[16])
{
    if (data.size() != 32u) {
        return;
    }

    size_t k = 0;
    for (uint8_t i = 0; i < data.size(); i += 2) {
        const std::string temp = data.substr(i, 2);
        uint8_t v = static_cast<uint8_t>(std::stoul(temp, nullptr, 16));
        key[k] = v;
        ++k;
    }
}

void stringToKey(const std::string &data, uint8_t (&key)[32])
{
    if (data.size() != 64u) {
        return;
    }

    size_t k = 0;
    for (uint8_t i = 0; i < data.size(); i += 2) {
        const std::string temp = data.substr(i, 2);
        uint8_t v = static_cast<uint8_t>(std::stoul(temp, nullptr, 16));
        key[k] = v;
        ++k;
    }
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

TEST(EncryptorTests, encryptAes256)
{
    ByteBuffer key(32u);
    key.writeHexString("000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f");

    {
        SCOPED_TRACE("// case 1. two full blocks + one non-full block");

        ByteBuffer data(47u);
        data.writeHexString(
            "00112233445566778899aabbccddeeff00112233445566778899aabbccddeeff00112233445566778899aabbccddee");

        const auto encodedData = Encryptor::encryptAes256(data, key);
        EXPECT_EQ(encodedData.asHexString(),
                  "8ea2b7ca516745bfeafc49904b4960898ea2b7ca516745bfeafc49904b49608994501d9b894874a1f7feae67d905c45b0f");
    }

    {
        SCOPED_TRACE("// case 2. one non-full block");

        ByteBuffer data(15u);
        data.writeHexString("00112233445566778899aabbccddee");

        const auto encodedData = Encryptor::encryptAes256(data, key);
        EXPECT_EQ(encodedData.asHexString(), "94501d9b894874a1f7feae67d905c45b0f");
    }

    {
        SCOPED_TRACE("// case 3. one full block");

        ByteBuffer data(16u);
        data.writeHexString("00112233445566778899aabbccddeeff");

        const auto encodedData = Encryptor::encryptAes256(data, key);
        EXPECT_EQ(encodedData.asHexString(), "8ea2b7ca516745bfeafc49904b496089");
    }
}

TEST(EncryptorTests, decryptAes256)
{
    ByteBuffer key(32u);
    key.writeHexString("000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f");

    {
        SCOPED_TRACE("// case 1. two full blocks + one non-full block");

        ByteBuffer data(49u);
        data.writeHexString(
            "8ea2b7ca516745bfeafc49904b4960898ea2b7ca516745bfeafc49904b49608994501d9b894874a1f7feae67d905c45b0f");

        const auto decodedData = Encryptor::decryptAes256(data, key);
        EXPECT_EQ(decodedData.asHexString(),
                  "00112233445566778899aabbccddeeff00112233445566778899aabbccddeeff00112233445566778899aabbccddee");
    }

    {
        SCOPED_TRACE("// case 2. one non-full block");

        ByteBuffer data(17u);
        data.writeHexString("94501d9b894874a1f7feae67d905c45b0f");

        const auto decodedData = Encryptor::decryptAes256(data, key);
        EXPECT_EQ(decodedData.asHexString(), "00112233445566778899aabbccddee");
    }

    {
        SCOPED_TRACE("// case 3. one full block");

        ByteBuffer data(16u);
        data.writeHexString("8ea2b7ca516745bfeafc49904b496089");

        const auto decodedData = Encryptor::decryptAes256(data, key);
        EXPECT_EQ(decodedData.asHexString(), "00112233445566778899aabbccddeeff");
    }
}

TEST(EncryptorTests, subBytes)
{
    Encryptor::DataBlock16 block;
    stringToBlock("00102030405060708090a0b0c0d0e0f0", block);

    Encryptor::subBytes(Encryptor::m_sBox, block);

    EXPECT_EQ("63cab7040953d051cd60e0e7ba70e18c", blockToString(block));
}

TEST(EncryptorTests, shiftRows)
{
    Encryptor::DataBlock16 block;
    stringToBlock("63cab7040953d051cd60e0e7ba70e18c", block);

    Encryptor::shiftRows(block);

    EXPECT_EQ("6353e08c0960e104cd70b751bacad0e7", blockToString(block));
}

TEST(EncryptorTests, mixColumns)
{
    Encryptor::DataBlock16 block;
    stringToBlock("d6f3d9dda6279bd1430d52a0e513f3fe", block);

    Encryptor::mixColumns(block);

    EXPECT_EQ("bd86f0ea748fc4f4630f11c1e9331233", blockToString(block));
}

TEST(EncryptorTests, applySubKey)
{
    uint8_t roundKey[16u] = {0xd0, 0x14, 0xf9, 0xa8, 0xc9, 0xee, 0x25, 0x89, 0xe1, 0x3f, 0x0c, 0xc8, 0xb6, 0x63, 0x0c, 0xa6};
    Encryptor::DataBlock16 block;
    stringToBlock("e9317db5cb322c723d2e895faf090794", block);

    Encryptor::applySubKey(roundKey, block);

    EXPECT_EQ("3925841d02dc09fbdc118597196a0b32", blockToString(block));
}

TEST(EncryptorTests, writeBlock)
{
    uint8_t data[16] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f};
    Encryptor::DataBlock16 block;
    Encryptor::writeBlock(data, 16, block);

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

TEST(EncryptorTests, readBlock)
{
    Encryptor::DataBlock16 block {{0x00, 0x04, 0x08, 0x0c},
                                  {0x01, 0x05, 0x09, 0x0d},
                                  {0x02, 0x06, 0x0a, 0x0e},
                                  {0x03, 0x07, 0x0b, 0x0f}};
    uint8_t data[16] = {'\0'};
    Encryptor::readBlock(block, data, 16);

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

TEST(EncryptorTests, rotWord)
{
    uint8_t block[4] = {0x01, 0x02, 0x03, 0x04};
    Encryptor::rotWord(block);
    ASSERT_EQ(block[0], 0x02);
    ASSERT_EQ(block[1], 0x03);
    ASSERT_EQ(block[2], 0x04);
    ASSERT_EQ(block[3], 0x01);

    // psi::test::TestHelper::timeFn("rotWord_old", [&]() { Encryptor::rotWord(block); }, 100'000'000);
    // psi::test::TestHelper::timeFn("rotWord_new", [&]() { Encryptor::rotWord2(block); }, 100'000'000);
}

TEST(EncryptorTests, DISABLED_generateSessionKey)
{
    std::vector<std::string> keyStrs;
    auto comparator = [](const std::vector<uint64_t> &a, const std::vector<uint64_t> &b) {
        if (a.size() != b.size()) {
            return a.size() < b.size();
        }
        for (size_t i = 0; i < a.size(); ++i) {
            if (a[i] != b[i]) {
                return a[i] < b[i];
            }
        }
        return true; 
    };
    std::set<std::vector<uint64_t>, decltype(comparator)> keys;

    // generate N keys
    const size_t N = 1'000'000;
    for (size_t i = 0; i < N; ++i) {
        const auto key = Encryptor::generateSessionKey();
        keyStrs.emplace_back(key.asHexString());
        auto itr = keys.emplace(key.asHash());
        // if insertion failed that means that session key is NOT unique
        if (i != 0) {
            ASSERT_TRUE(itr.second);
        }
    }
}