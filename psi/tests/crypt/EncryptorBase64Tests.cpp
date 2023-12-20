#include "TestHelper.h"
#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <iostream>
#include <set>
#include <sstream>

#include "psi/tools/Tools.h"
#include "psi/tools/crypt/EncryptorBase64.h"

using namespace psi::tools;
using namespace psi::tools::crypt;

TEST(EncryptorBase64Tests, EncryptionDecryption_Base64_Success)
{
    auto doTest = [](const std::string &originMessage, const std::string &expectedEncryptedMessage) {
        ByteBuffer msg(originMessage.size());
        msg.writeString(originMessage);

        ByteBuffer encryptedMessage = EncryptorBase64::encryptBase64(msg);
        EXPECT_EQ(encryptedMessage.asString(), expectedEncryptedMessage);

        ByteBuffer decryptedMessage = EncryptorBase64::decryptBase64(encryptedMessage);
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

TEST(EncryptorBase64Tests, EncryptionDecryption_Base64_Fail)
{
    ByteBuffer encryptedMessage(0u);
    ByteBuffer decryptedMessage = EncryptorBase64::decryptBase64(encryptedMessage);
    EXPECT_TRUE(decryptedMessage.size() == 0);
}
