#include "TestHelper.h"
#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <iostream>
#include <set>
#include <sstream>

#include "psi/tools/Encryptor.h"
#include "psi/tools/Tools.h"

using namespace psi::tools;

TEST(EncryptorTests, encryptAes128)
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

TEST(EncryptorTests, EncryptionDecryption_AES_128)
{
    auto doTest = [](const std::string &hexMessage, const std::string &hexKey) {
        ByteBuffer message(16u);
        message.writeHexString(hexMessage);

        ByteBuffer key(16u);
        key.writeHexString(hexKey);

        auto encryptedMessage = Encryptor::encryptAes128(message, key);
        ASSERT_TRUE(encryptedMessage.size() > 0);

        auto decryptedMessage = Encryptor::decryptAes128(encryptedMessage, key);
        ASSERT_TRUE(decryptedMessage.size() > 0);
        EXPECT_EQ(decryptedMessage.asHexString(), hexMessage);
    };

    doTest("00112233445566778899aabbccddeeff", "000102030405060708090a0b0c0d0e0f");
    doTest("f69f2445df4f9b17ad2b417be66c3710", "603deb1015ca71be2b73aef0857d7781");
    doTest("ae2d8a571e03ac9c9eb76fac45af8e51", "603deb1015ca71be2b73aef0857d7781");
}

TEST(EncryptorTests, EncryptionDecryption_AES_256)
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

TEST(EncryptorTests, BigDataEncryptionDecryption_AES_256)
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

TEST(EncryptorTests, NonStringDataEncryptionDecryption_AES_256)
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
