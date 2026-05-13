#include "psi/test/TestHelper.h"
#include "psi/test/psi_mock.h"

#include <iostream>
#include <set>
#include <sstream>

#include "psi/tools/Tools.h"
#include "psi/tools/crypt/base64.h"

using namespace psi::tools;
using namespace psi::tools::crypt;
using namespace psi::test;

TEST(base64_Tests, EncryptionDecryption_Base64_Success)
{
    auto doTest = [](const std::string &originMessage, const std::string &expectedEncryptedMessage) {
        ByteBuffer msg(originMessage.size());
        msg.writeString(originMessage);

        ByteBuffer encryptedMessage = base64::encryptBase64(msg);
        EXPECT_EQ(encryptedMessage.asString(), expectedEncryptedMessage);

        ByteBuffer decryptedMessage = base64::decryptBase64(encryptedMessage);
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

TEST(base64_Tests, EncryptionDecryption_Base64_Fail)
{
    ByteBuffer encryptedMessage(0u);
    ByteBuffer decryptedMessage = base64::decryptBase64(encryptedMessage);
    EXPECT_EQ(decryptedMessage.size(), size_t {0});
}

TEST(base64_Tests, DecryptBase64_InvalidLength)
{
    // Length not divisible by 4 — must return empty
    ByteBuffer buf(3u);
    buf.writeString("abc");
    ByteBuffer result = base64::decryptBase64(buf);
    EXPECT_EQ(result.size(), size_t {0});
}

TEST(base64_Tests, DecryptBase64_InvalidCharacter)
{
    // '!' is not a valid base64 character
    ByteBuffer buf(4u);
    buf.writeString("A!AA");
    ByteBuffer result = base64::decryptBase64(buf);
    EXPECT_EQ(result.size(), size_t {0});
}

TEST(base64_Tests, DecryptBase64_InvalidPaddingPosition)
{
    // '=' at position 1 triggers the default: branch in padding switch
    // inputLen=4, when '=' is read at idx=1: inputLen-idx=3 → default
    ByteBuffer buf(4u);
    buf.writeString("A===");
    ByteBuffer result = base64::decryptBase64(buf);
    EXPECT_EQ(result.size(), size_t {0});
}

TEST(base64_Tests, EncryptBase64_EmptyInput)
{
    ByteBuffer empty(0u);
    ByteBuffer result = base64::encryptBase64(empty);
    EXPECT_EQ(result.size(), size_t {0});
}

TEST(base64_Tests, EncryptDecrypt_BinaryData)
{
    // Binary data including boundary bytes: 0x00, 0xFF, and values mapping to +, /
    ByteBuffer buf(9u);
    buf.write(uint8_t(0x00));
    buf.write(uint8_t(0xFF));
    buf.write(uint8_t(0xFB)); // produces '+' in base64
    buf.write(uint8_t(0xFF));
    buf.write(uint8_t(0x00));
    buf.write(uint8_t(0x01));
    buf.write(uint8_t(0x7F));
    buf.write(uint8_t(0x80));
    buf.write(uint8_t(0xFE));

    ByteBuffer encrypted = base64::encryptBase64(buf);
    EXPECT_EQ(encrypted.size(), size_t {12}); // 9 bytes → 12 base64 chars (no padding)

    ByteBuffer decrypted = base64::decryptBase64(encrypted);
    EXPECT_EQ(decrypted.size(), buf.size());

    buf.resetRead();
    decrypted.resetRead();
    uint8_t a, b;
    while (buf.read(a) && decrypted.read(b)) {
        EXPECT_EQ(a, b);
    }
}
