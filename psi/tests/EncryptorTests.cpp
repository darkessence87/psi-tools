#include "TestHelper.h"
#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <iostream>
#include <set>
#include <sstream>

#include "psi/tools/Encryptor.h"
#include "psi/tools/Tools.h"

using namespace psi::tools;

TEST(EncryptorTests, generateSubKeys_impl_AES128)
{
    const ByteBuffer
        expectedEnhancedKey("2b7e151628aed2a6abf7158809cf4f3ca0fafe1788542cb123a339392a6c7605f2c295f27a96b9435935807a73"
                            "59f67f3d80477d4716fe3e1e237e446d7a883bef44a541a8525b7fb671253bdb0bad00d4d1c6f87c839d87caf2"
                            "b8bc11f915bc6d88a37a110b3efddbf98641ca0093fd4e54f70e5f5fc9f384a64fb24ea6dc4fead27321b58dba"
                            "d2312bf5607f8d292fac7766f319fadc2128d12941575c006ed014f9a8c9ee2589e13f0cc8b6630ca6",
                            true);

    const ByteBuffer key("2b7e151628aed2a6abf7158809cf4f3c", true);
    Encryptor::SubKeys128 subKeys = {};
    Encryptor::generateSubKeys_impl<4, 10>(key.data(), subKeys);
    ByteBuffer enhancedKey(16u * 11u);
    for (size_t i = 0; i < enhancedKey.size(); ++i) {
        enhancedKey.write(subKeys[i]);
    }
    EXPECT_EQ(expectedEnhancedKey.asHexStringFormatted(), enhancedKey.asHexStringFormatted());
}

TEST(EncryptorTests, generateSubKeys_impl_AES256)
{
    const ByteBuffer expectedEnhancedKey(
        "603deb1015ca71be2b73aef0857d77811f352c073b6108d72d9810a30914dff49ba354118e6925afa51a8b5f2067fcdea8b09c1a93d194"
        "cdbe49846eb75d5b9ad59aecb85bf3c917fee94248de8ebe96b5a9328a2678a647983122292f6c79b3812c81addadf48ba24360af2fab8"
        "b46498c5bfc9bebd198e268c3ba709e0421468007bacb2df331696e939e46c518d80c814e20476a9fb8a5025c02d59c58239de1369676c"
        "cc5a71fa2563959674ee155886ca5d2e2f31d77e0af1fa27cf73c3749c47ab18501ddae2757e4f7401905acafaaae3e4d59b349adf6ace"
        "bd10190dfe4890d1e6188d0b046df344706c631e",
        true);

    const ByteBuffer key("603deb1015ca71be2b73aef0857d77811f352c073b6108d72d9810a30914dff4", true);
    Encryptor::SubKeys256 subKeys = {};
    Encryptor::generateSubKeys_impl<8, 14>(key.data(), subKeys);
    ByteBuffer enhancedKey(16u * 15u);
    for (size_t i = 0; i < enhancedKey.size(); ++i) {
        enhancedKey.write(subKeys[i]);
    }
    EXPECT_EQ(expectedEnhancedKey.asHexStringFormatted(), enhancedKey.asHexStringFormatted());
}

TEST(EncryptorTests, encryptAes_impl_AES128)
{
    ByteBuffer key(16u);
    key.writeHexString("000102030405060708090a0b0c0d0e0f");

    {
        SCOPED_TRACE("// case 1. two full blocks + one non-full block");

        ByteBuffer data(47u);
        data.writeHexString(
            "00112233445566778899aabbccddeeff00112233445566778899aabbccddeeff00112233445566778899aabbccddee");

        const auto encodedData = Encryptor::encryptAes128(data, key);
        EXPECT_EQ(encodedData.asHexString(),
                  "69c4e0d86a7b0430d8cdb78070b4c55a69c4e0d86a7b0430d8cdb78070b4c55a7c99f42b6ee503309c6c1a67e97ac2420f");
    }

    {
        SCOPED_TRACE("// case 2. one non-full block");

        ByteBuffer data(15u);
        data.writeHexString("00112233445566778899aabbccddee");

        const auto encodedData = Encryptor::encryptAes128(data, key);
        EXPECT_EQ(encodedData.asHexString(), "7c99f42b6ee503309c6c1a67e97ac2420f");
    }

    {
        SCOPED_TRACE("// case 3. one full block");

        ByteBuffer data(16u);
        data.writeHexString("00112233445566778899aabbccddeeff");

        const auto encodedData = Encryptor::encryptAes128(data, key);
        EXPECT_EQ(encodedData.asHexString(), "69c4e0d86a7b0430d8cdb78070b4c55a");
    }
}

TEST(EncryptorTests, encryptAes_impl_AES256)
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
    // const size_t N = 10;
    for (size_t i = 0; i < N; ++i) {
        const auto key = Encryptor::generateSessionKey();
        keyStrs.emplace_back(key.asHexString());
        auto itr = keys.emplace(key.asHash());
        // if insertion failed that means that session key is NOT unique
        if (i != 0) {
            ASSERT_TRUE(itr.second);

            if (i == 1) {
                std::cout << key.asHexStringFormatted() << std::endl;
            }
        }
    }
}

TEST(Encryptor_x25519Tests, scalarmult_base)
{
    {
        SCOPED_TRACE("// case 1. sender keys");
        uint8_t sender_pk[32];
        uint8_t sender_sk[32] = {0x77, 0x07, 0x6d, 0x0a, 0x73, 0x18, 0xa5, 0x7d, 0x3c, 0x16, 0xc1,
                                 0x72, 0x51, 0xb2, 0x66, 0x45, 0xdf, 0x4c, 0x2f, 0x87, 0xeb, 0xc0,
                                 0x99, 0x2a, 0xb1, 0x77, 0xfb, 0xa5, 0x1d, 0xb9, 0x2c, 0x2a};

        Encryptor_x25519::scalarmult_base(sender_pk, sender_sk);

        ByteBuffer pkBuffer(32);
        pkBuffer.write(sender_pk);

        uint8_t expectedPk[32] = {0x85, 0x20, 0xf0, 0x09, 0x89, 0x30, 0xa7, 0x54, 0x74, 0x8b, 0x7d,
                                  0xdc, 0xb4, 0x3e, 0xf7, 0x5a, 0x0d, 0xbf, 0x3a, 0x0d, 0x26, 0x38,
                                  0x1a, 0xf4, 0xeb, 0xa4, 0xa9, 0x8e, 0xaa, 0x9b, 0x4e, 0x6a};
        ByteBuffer expected(32);
        expected.write(expectedPk);
        EXPECT_EQ(pkBuffer.asHexString(), expected.asHexString());
    }

    {
        SCOPED_TRACE("// case 2. receiver keys");
        uint8_t receiver_pk[32];
        uint8_t receiver_sk[32] = {0x5d, 0xab, 0x08, 0x7e, 0x62, 0x4a, 0x8a, 0x4b, 0x79, 0xe1, 0x7f,
                                   0x8b, 0x83, 0x80, 0x0e, 0xe6, 0x6f, 0x3b, 0xb1, 0x29, 0x26, 0x18,
                                   0xb6, 0xfd, 0x1c, 0x2f, 0x8b, 0x27, 0xff, 0x88, 0xe0, 0xeb};

        Encryptor_x25519::scalarmult_base(receiver_pk, receiver_sk);

        ByteBuffer pkBuffer(32);
        pkBuffer.write(receiver_pk);

        uint8_t expectedPk[32] = {0xde, 0x9e, 0xdb, 0x7d, 0x7b, 0x7d, 0xc1, 0xb4, 0xd3, 0x5b, 0x61,
                                  0xc2, 0xec, 0xe4, 0x35, 0x37, 0x3f, 0x83, 0x43, 0xc8, 0x5b, 0x78,
                                  0x67, 0x4d, 0xad, 0xfc, 0x7e, 0x14, 0x6f, 0x88, 0x2b, 0x4f};
        ByteBuffer expected(32);
        expected.write(expectedPk);
        EXPECT_EQ(pkBuffer.asHexString(), expected.asHexString());
    }
}

TEST(Encryptor_x25519Tests, scalarmult)
{
    {
        SCOPED_TRACE("// case 1. shared key sender side");
        uint8_t shared_pk[32];
        uint8_t sender_sk[32] = {0x77, 0x07, 0x6d, 0x0a, 0x73, 0x18, 0xa5, 0x7d, 0x3c, 0x16, 0xc1,
                                 0x72, 0x51, 0xb2, 0x66, 0x45, 0xdf, 0x4c, 0x2f, 0x87, 0xeb, 0xc0,
                                 0x99, 0x2a, 0xb1, 0x77, 0xfb, 0xa5, 0x1d, 0xb9, 0x2c, 0x2a};
        uint8_t receiver_pk[32] = {0xde, 0x9e, 0xdb, 0x7d, 0x7b, 0x7d, 0xc1, 0xb4, 0xd3, 0x5b, 0x61,
                                   0xc2, 0xec, 0xe4, 0x35, 0x37, 0x3f, 0x83, 0x43, 0xc8, 0x5b, 0x78,
                                   0x67, 0x4d, 0xad, 0xfc, 0x7e, 0x14, 0x6f, 0x88, 0x2b, 0x4f};

        Encryptor_x25519::scalarmult(shared_pk, sender_sk, receiver_pk);

        ByteBuffer shared_pkBuffer(32);
        shared_pkBuffer.write(shared_pk);
        uint8_t expectedSharedKey[32] = {0x4a, 0x5d, 0x9d, 0x5b, 0xa4, 0xce, 0x2d, 0xe1, 0x72, 0x8e, 0x3b,
                                         0xf4, 0x80, 0x35, 0x0f, 0x25, 0xe0, 0x7e, 0x21, 0xc9, 0x47, 0xd1,
                                         0x9e, 0x33, 0x76, 0xf0, 0x9b, 0x3c, 0x1e, 0x16, 0x17, 0x42};
        ByteBuffer expected(32);
        expected.write(expectedSharedKey);
        EXPECT_EQ(shared_pkBuffer.asHexString(), expected.asHexString());
    }
    
    {
        SCOPED_TRACE("// case 2. shared key receiver side");
        uint8_t shared_pk[32];
        uint8_t receiver_sk[32] = {0x5d, 0xab, 0x08, 0x7e, 0x62, 0x4a, 0x8a, 0x4b, 0x79, 0xe1, 0x7f,
                                   0x8b, 0x83, 0x80, 0x0e, 0xe6, 0x6f, 0x3b, 0xb1, 0x29, 0x26, 0x18,
                                   0xb6, 0xfd, 0x1c, 0x2f, 0x8b, 0x27, 0xff, 0x88, 0xe0, 0xeb};
        uint8_t sender_pk[32] = {0x85, 0x20, 0xf0, 0x09, 0x89, 0x30, 0xa7, 0x54, 0x74, 0x8b, 0x7d,
                                 0xdc, 0xb4, 0x3e, 0xf7, 0x5a, 0x0d, 0xbf, 0x3a, 0x0d, 0x26, 0x38,
                                 0x1a, 0xf4, 0xeb, 0xa4, 0xa9, 0x8e, 0xaa, 0x9b, 0x4e, 0x6a};

        Encryptor_x25519::scalarmult(shared_pk, receiver_sk, sender_pk);

        ByteBuffer shared_pkBuffer(32);
        shared_pkBuffer.write(shared_pk);
        uint8_t expectedSharedKey[32] = {0x4a, 0x5d, 0x9d, 0x5b, 0xa4, 0xce, 0x2d, 0xe1, 0x72, 0x8e, 0x3b,
                                         0xf4, 0x80, 0x35, 0x0f, 0x25, 0xe0, 0x7e, 0x21, 0xc9, 0x47, 0xd1,
                                         0x9e, 0x33, 0x76, 0xf0, 0x9b, 0x3c, 0x1e, 0x16, 0x17, 0x42};
        ByteBuffer expected(32);
        expected.write(expectedSharedKey);
        EXPECT_EQ(shared_pkBuffer.asHexString(), expected.asHexString());
    }
}