#include "TestHelper.h"
#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <iostream>
#include <set>
#include <sstream>

#include "psi/tools/Encryptor.h"
#include "psi/tools/Tools.h"

using namespace psi::test;
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

TEST(EncryptorTests, Tls13Handshake)
{
    ByteBuffer clientPrivateKey("49af42ba7f7994852d713ef2784bcbcaa7911de26adc5642cb634540e7ea5005", true);
    ByteBuffer clientPublicKey("99381de560e4bd43d23d8e435a7dbafeb3c06e51c13cae4d5413691e529aaf2c", true);

    ByteBuffer serverPrivateKey("b1580eeadf6dd589b8ef4f2d5652578cc810e9980191ec8d058308cea216a21e", true);
    ByteBuffer serverPublicKey("c9828876112095fe66762bdbf7c672e156d6cc253b833df1dd69b1b04e751f0f", true);

    ByteBuffer clientHello(
        "010000c00303cb34ecb1e78163ba1c38c6dacb196a6dffa21a8d9912ec18a2ef6283024dece7000006130113031302010000910000"
        "000b0009000006736572766572ff01000100000a00140012001d001700180019010001010102010301040023000000330026002400"
        "1d002099381de560e4bd43d23d8e435a7dbafeb3c06e51c13cae4d5413691e529aaf2c002b0003020304000d0020001e0403050306"
        "03020308040805080604010501060102010402050206020202002d00020101001c00024001",
        true);
    ByteBuffer serverHello(
        "020000560303a6af06a4121860dc5e6e60249cd34c95930c8ac5cb1434dac155772ed3e2692800130100002e00330024001d0020c9"
        "828876112095fe66762bdbf7c672e156d6cc253b833df1dd69b1b04e751f0f002b00020304",
        true);
    ByteBuffer serverExt("080000240022000a00140012001d00170018001901000101010201030104001c0002400100000000", true);
    ByteBuffer serverCert(
        "0b0001b9000001b50001b0308201ac30820115a003020102020102300d06092a864886f70d01010b0500300e310c300a06035504031303"
        "727361301e170d3136303733303031323335395a170d3236303733303031323335395a300e310c300a0603550403130372736130819f30"
        "0d06092a864886f70d010101050003818d0030818902818100b4bb498f8279303d980836399b36c6988c0c68de55e1bdb826d3901a2461"
        "eafd2de49a91d015abbc9a95137ace6c1af19eaa6af98c7ced43120998e187a80ee0ccb0524b1b018c3e0b63264d449a6d38e22a5fda43"
        "0846748030530ef0461c8ca9d9efbfae8ea6d1d03e2bd193eff0ab9a8002c47428a6d35a8d88d79f7f1e3f0203010001a31a3018300906"
        "03551d1304023000300b0603551d0f0404030205a0300d06092a864886f70d01010b05000381810085aad2a0e5b9276b908c65f73a7267"
        "170618a54c5f8a7b337d2df7a594365417f2eae8f8a58c8f8172f9319cf36b7fd6c55b80f21a03015156726096fd335e5e67f2dbf10270"
        "2e608ccae6bec1fc63a42a99be5c3eb7107c3c54e9b9eb2bd5203b1c3b84e0a8b2f759409ba3eac9d91d402dcc0cc8f8961229ac9187b4"
        "2b4de10000",
        true);
    ByteBuffer serverCertVerify(
        "0f000084080400805a747c5d88fa9bd2e55ab085a61015b7211f824cd484145ab3ff52f1fda8477b0b7abc90db78e2d33a5c141a078653"
        "fa6bef780c5ea248eeaaa785c4f394cab6d30bbe8d4859ee511f602957b15411ac027671459e46445c9ea58c181e818e95b8c3fb0bf327"
        "8409d3be152a3da5043e063dda65cdf5aea20d53dfacd42f74f3",
        true);
    ByteBuffer serverFinished("140000209b9b141d906337fbd2cbdce71df4deda4ab42c309572cb7fffee5454b78f0718", true);
    std::string clientFinishedStr = "14000020a8ec436d677634ae525ac1fcebe11a039ec17694fac6e98527b642f2edd5ce61";

    ByteBuffer sharedKey = Encryptor::x25519_scalarmult(clientPrivateKey, serverPublicKey);
    EXPECT_EQ(sharedKey.asHexString(), "8bd4054fb55b9d63fdfbacf9f04b9f0d35e6d63f537563efd46272900f89492d");

    auto client_server_hash = Encryptor::sha256(clientHello + serverHello);
    EXPECT_EQ(client_server_hash.asHexString(), "860c06edc07858ee8e78f0e7428c58edd6b43f2ca3e6e95f02ed063cf0e1cad8");
    auto handshake_hash =
        Encryptor::sha256(clientHello + serverHello + serverExt + serverCert + serverCertVerify + serverFinished);
    EXPECT_EQ(handshake_hash.asHexString(), "9608102a0f1ccc6db6250b7b7e417b1a000eaada3daae4777a7686c9ff83df13");

    auto earlySecret = Encryptor::hmac256(ByteBuffer(32), ByteBuffer(32));
    auto emptyHash = Encryptor::sha256({});
    auto derivedSecret = Encryptor::hkdf256ExpandLabel(earlySecret, "tls13 derived", emptyHash, 32u);
    auto handshake = Encryptor::hmac256(derivedSecret, sharedKey);
    auto clientHandshake = Encryptor::hkdf256ExpandLabel(handshake, "tls13 c hs traffic", client_server_hash, 32u);
    auto serverHandshake = Encryptor::hkdf256ExpandLabel(handshake, "tls13 s hs traffic", client_server_hash, 32u);
    auto masterDerived = Encryptor::hkdf256ExpandLabel(handshake, "tls13 derived", emptyHash, 32u);
    auto masterKey = Encryptor::hmac256(masterDerived, ByteBuffer(32));
    auto serverHandshakeKey = Encryptor::hkdf256ExpandLabel(serverHandshake, "tls13 key", {}, 16u);
    auto serverHandshakeIv = Encryptor::hkdf256ExpandLabel(serverHandshake, "tls13 iv", {}, 12u);
    auto clientHandshakeKey = Encryptor::hkdf256ExpandLabel(clientHandshake, "tls13 key", {}, 16u);
    auto clientHandshakeIv = Encryptor::hkdf256ExpandLabel(clientHandshake, "tls13 iv", {}, 12u);
    auto clientApp = Encryptor::hkdf256ExpandLabel(masterKey, "tls13 c ap traffic", handshake_hash, 32u);
    auto serverApp = Encryptor::hkdf256ExpandLabel(masterKey, "tls13 s ap traffic", handshake_hash, 32u);
    auto serverAppKey = Encryptor::hkdf256ExpandLabel(serverApp, "tls13 key", {}, 16u);
    auto serverAppIv = Encryptor::hkdf256ExpandLabel(serverApp, "tls13 iv", {}, 12u);
    auto serverFinishedKey = Encryptor::hkdf256ExpandLabel(serverHandshake, "tls13 finished", {}, 32u);
    auto serverFinishedVerify =
        Encryptor::hmac256(serverFinishedKey,
                           Encryptor::sha256(clientHello + serverHello + serverExt + serverCert + serverCertVerify));
    auto clientAppKey = Encryptor::hkdf256ExpandLabel(clientApp, "tls13 key", {}, 16u);
    auto clientAppIv = Encryptor::hkdf256ExpandLabel(clientApp, "tls13 iv", {}, 12u);
    auto clientFinishedKey = Encryptor::hkdf256ExpandLabel(clientHandshake, "tls13 finished", {}, 32u);
    auto clientFinishedVerify = Encryptor::hmac256(clientFinishedKey, handshake_hash);

    ByteBuffer acc(5u);
    acc.write(uint8_t(0x17));
    acc.write(uint8_t(0x03));
    acc.write(uint8_t(0x03));
    acc.write(uint8_t(0x00));
    acc.write(uint8_t(4u + 32u + 1u + 16u));
    ByteBuffer tag(16u);
    auto clientFinishedVerifyEncrypted =
        Encryptor::encryptAes128Gcm(ByteBuffer("14000020", true) + clientFinishedVerify + ByteBuffer("16", true),
                                    clientHandshakeKey,
                                    clientHandshakeIv,
                                    tag,
                                    acc);
    ByteBuffer clientHsRecord(acc.length() + clientFinishedVerifyEncrypted.length() + tag.length());
    clientHsRecord.writeArray(acc.data(), acc.length());
    clientHsRecord.writeArray(clientFinishedVerifyEncrypted.data(), clientFinishedVerifyEncrypted.length());
    clientHsRecord.writeArray(tag.data(), tag.length());

    EXPECT_EQ(earlySecret.asHexString(), "33ad0a1c607ec03b09e6cd9893680ce210adf300aa1f2660e1b22e10f170f92a");
    EXPECT_EQ(emptyHash.asHexString(), "e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855");
    EXPECT_EQ(derivedSecret.asHexString(), "6f2615a108c702c5678f54fc9dbab69716c076189c48250cebeac3576c3611ba");
    EXPECT_EQ(handshake.asHexString(), "1dc826e93606aa6fdc0aadc12f741b01046aa6b99f691ed221a9f0ca043fbeac");
    EXPECT_EQ(clientHandshake.asHexString(), "b3eddb126e067f35a780b3abf45e2d8f3b1a950738f52e9600746a0e27a55a21");
    EXPECT_EQ(serverHandshake.asHexString(), "b67b7d690cc16c4e75e54213cb2d37b4e9c912bcded9105d42befd59d391ad38");
    EXPECT_EQ(masterDerived.asHexString(), "43de77e0c77713859a944db9db2590b53190a65b3ee2e4f12dd7a0bb7ce254b4");
    EXPECT_EQ(masterKey.asHexString(), "18df06843d13a08bf2a449844c5f8a478001bc4d4c627984d5a41da8d0402919");
    EXPECT_EQ(serverHandshakeKey.asHexString(), "3fce516009c21727d0f2e4e86ee403bc");
    EXPECT_EQ(serverHandshakeIv.asHexString(), "5d313eb2671276ee13000b30");
    EXPECT_EQ(clientHandshakeKey.asHexString(), "dbfaa693d1762c5b666af5d950258d01");
    EXPECT_EQ(clientHandshakeIv.asHexString(), "5bd3c71b836e0b76bb73265f");
    EXPECT_EQ(clientApp.asHexString(), "9e40646ce79a7f9dc05af8889bce6552875afa0b06df0087f792ebb7c17504a5");
    EXPECT_EQ(serverApp.asHexString(), "a11af9f05531f856ad47116b45a950328204b4f44bfb6b3a4b4f1f3fcb631643");
    EXPECT_EQ(serverAppKey.asHexString(), "9f02283b6c9c07efc26bb9f2ac92e356");
    EXPECT_EQ(serverAppIv.asHexString(), "cf782b88dd83549aadf1e984");
    EXPECT_EQ(serverFinishedKey.asHexString(), "008d3b66f816ea559f96b537e885c31fc068bf492c652f01f288a1d8cdc19fc8");
    EXPECT_EQ(serverFinishedVerify.asHexString(), "9b9b141d906337fbd2cbdce71df4deda4ab42c309572cb7fffee5454b78f0718");
    EXPECT_EQ(clientAppKey.asHexString(), "17422dda596ed5d9acd890e3c63f5051");
    EXPECT_EQ(clientAppIv.asHexString(), "5b78923dee08579033e523d9");
    EXPECT_EQ(clientFinishedKey.asHexString(), "b80ad01015fb2f0bd65ff7d4da5d6bf83f84821d1f87fdc7d3c75b5a7b42d9c4");
    EXPECT_EQ(clientFinishedVerify.asHexString(), "a8ec436d677634ae525ac1fcebe11a039ec17694fac6e98527b642f2edd5ce61");
    EXPECT_EQ(clientHsRecord.asHexString(), "170303003575ec4dc238cce60b298044a71e219c56cc77b0517fe9b93c7a4bfc44d87f38f80338ac98fc46deb384bd1caeacab6867d726c40546");
}

TEST(EncryptorTests, performance)
{
    auto tlsHandshake = []() {
        ByteBuffer clientPrivateKey("49af42ba7f7994852d713ef2784bcbcaa7911de26adc5642cb634540e7ea5005", true);
        ByteBuffer clientPublicKey("99381de560e4bd43d23d8e435a7dbafeb3c06e51c13cae4d5413691e529aaf2c", true);

        ByteBuffer serverPrivateKey("b1580eeadf6dd589b8ef4f2d5652578cc810e9980191ec8d058308cea216a21e", true);
        ByteBuffer serverPublicKey("c9828876112095fe66762bdbf7c672e156d6cc253b833df1dd69b1b04e751f0f", true);

        ByteBuffer clientHello(
            "010000c00303cb34ecb1e78163ba1c38c6dacb196a6dffa21a8d9912ec18a2ef6283024dece7000006130113031302010000910000"
            "000b0009000006736572766572ff01000100000a00140012001d001700180019010001010102010301040023000000330026002400"
            "1d002099381de560e4bd43d23d8e435a7dbafeb3c06e51c13cae4d5413691e529aaf2c002b0003020304000d0020001e0403050306"
            "03020308040805080604010501060102010402050206020202002d00020101001c00024001",
            true);
        ByteBuffer serverHello(
            "020000560303a6af06a4121860dc5e6e60249cd34c95930c8ac5cb1434dac155772ed3e2692800130100002e00330024001d0020c9"
            "828876112095fe66762bdbf7c672e156d6cc253b833df1dd69b1b04e751f0f002b00020304",
            true);
        ByteBuffer serverExt("080000240022000a00140012001d00170018001901000101010201030104001c0002400100000000", true);
        ByteBuffer serverCert("0b0001b9000001b50001b0308201ac30820115a003020102020102300d06092a864886f70d01010b0500300e"
                              "310c300a06035504031303"
                              "727361301e170d3136303733303031323335395a170d3236303733303031323335395a300e310c300a060355"
                              "0403130372736130819f30"
                              "0d06092a864886f70d010101050003818d0030818902818100b4bb498f8279303d980836399b36c6988c0c68"
                              "de55e1bdb826d3901a2461"
                              "eafd2de49a91d015abbc9a95137ace6c1af19eaa6af98c7ced43120998e187a80ee0ccb0524b1b018c3e0b63"
                              "264d449a6d38e22a5fda43"
                              "0846748030530ef0461c8ca9d9efbfae8ea6d1d03e2bd193eff0ab9a8002c47428a6d35a8d88d79f7f1e3f02"
                              "03010001a31a3018300906"
                              "03551d1304023000300b0603551d0f0404030205a0300d06092a864886f70d01010b05000381810085aad2a0"
                              "e5b9276b908c65f73a7267"
                              "170618a54c5f8a7b337d2df7a594365417f2eae8f8a58c8f8172f9319cf36b7fd6c55b80f21a030151567260"
                              "96fd335e5e67f2dbf10270"
                              "2e608ccae6bec1fc63a42a99be5c3eb7107c3c54e9b9eb2bd5203b1c3b84e0a8b2f759409ba3eac9d91d402d"
                              "cc0cc8f8961229ac9187b4"
                              "2b4de10000",
                              true);
        ByteBuffer serverCertVerify("0f000084080400805a747c5d88fa9bd2e55ab085a61015b7211f824cd484145ab3ff52f1fda8477b0b"
                                    "7abc90db78e2d33a5c141a078653"
                                    "fa6bef780c5ea248eeaaa785c4f394cab6d30bbe8d4859ee511f602957b15411ac027671459e46445c"
                                    "9ea58c181e818e95b8c3fb0bf327"
                                    "8409d3be152a3da5043e063dda65cdf5aea20d53dfacd42f74f3",
                                    true);
        ByteBuffer serverFinished("140000209b9b141d906337fbd2cbdce71df4deda4ab42c309572cb7fffee5454b78f0718", true);

        ByteBuffer sharedKey = Encryptor::x25519_scalarmult(clientPrivateKey, serverPublicKey);
        auto client_server_hash = Encryptor::sha256(clientHello + serverHello);
        auto handshake_hash =
            Encryptor::sha256(clientHello + serverHello + serverExt + serverCert + serverCertVerify + serverFinished);
        EXPECT_EQ(handshake_hash.asHexString(), "9608102a0f1ccc6db6250b7b7e417b1a000eaada3daae4777a7686c9ff83df13");

        auto earlySecret = Encryptor::hmac256(ByteBuffer(32), ByteBuffer(32));
        auto emptyHash = Encryptor::sha256({});
        auto derivedSecret = Encryptor::hkdf256ExpandLabel(earlySecret, "tls13 derived", emptyHash, 32u);
        auto handshake = Encryptor::hmac256(derivedSecret, sharedKey);
        auto clientHandshake = Encryptor::hkdf256ExpandLabel(handshake, "tls13 c hs traffic", client_server_hash, 32u);
        auto serverHandshake = Encryptor::hkdf256ExpandLabel(handshake, "tls13 s hs traffic", client_server_hash, 32u);
        auto masterDerived = Encryptor::hkdf256ExpandLabel(handshake, "tls13 derived", emptyHash, 32u);
        auto masterKey = Encryptor::hmac256(masterDerived, ByteBuffer(32));
        auto serverHandshakeKey = Encryptor::hkdf256ExpandLabel(serverHandshake, "tls13 key", {}, 16u);
        auto serverHandshakeIv = Encryptor::hkdf256ExpandLabel(serverHandshake, "tls13 iv", {}, 12u);
        auto clientHandshakeKey = Encryptor::hkdf256ExpandLabel(clientHandshake, "tls13 key", {}, 16u);
        auto clientHandshakeIv = Encryptor::hkdf256ExpandLabel(clientHandshake, "tls13 iv", {}, 12u);
        auto clientApp = Encryptor::hkdf256ExpandLabel(masterKey, "tls13 c ap traffic", handshake_hash, 32u);
        auto serverApp = Encryptor::hkdf256ExpandLabel(masterKey, "tls13 s ap traffic", handshake_hash, 32u);
        auto serverAppKey = Encryptor::hkdf256ExpandLabel(serverApp, "tls13 key", {}, 16u);
        auto serverAppIv = Encryptor::hkdf256ExpandLabel(serverApp, "tls13 iv", {}, 12u);
        auto serverFinishedKey = Encryptor::hkdf256ExpandLabel(serverHandshake, "tls13 finished", {}, 32u);
        auto serverFinishedVerify =
            Encryptor::hmac256(serverFinishedKey,
                               Encryptor::sha256(clientHello + serverHello + serverExt + serverCert + serverCertVerify));
        auto clientAppKey = Encryptor::hkdf256ExpandLabel(clientApp, "tls13 key", {}, 16u);
        auto clientAppIv = Encryptor::hkdf256ExpandLabel(clientApp, "tls13 iv", {}, 12u);
        auto clientFinishedKey = Encryptor::hkdf256ExpandLabel(clientHandshake, "tls13 finished", {}, 32u);
        auto clientFinishedVerify = Encryptor::hmac256(clientFinishedKey, handshake_hash);

        ByteBuffer acc(5u);
        acc.write(uint8_t(0x17));
        acc.write(uint8_t(0x03));
        acc.write(uint8_t(0x03));
        acc.write(uint8_t(0x00));
        acc.write(uint8_t(4u + 32u + 1u + 16u));
        ByteBuffer tag(16u);
        auto clientFinishedVerifyEncrypted =
            Encryptor::encryptAes128Gcm(ByteBuffer("14000020", true) + clientFinishedVerify + ByteBuffer("16", true),
                                        clientHandshakeKey,
                                        clientHandshakeIv,
                                        tag,
                                        acc);
        ByteBuffer clientHsRecord(acc.length() + clientFinishedVerifyEncrypted.length() + tag.length());
        clientHsRecord.writeArray(acc.data(), acc.length());
        clientHsRecord.writeArray(clientFinishedVerifyEncrypted.data(), clientFinishedVerifyEncrypted.length());
        clientHsRecord.writeArray(tag.data(), tag.length());
    };
    TestHelper::timeFn("1. Tls13Handshake", tlsHandshake, 1000);
}
