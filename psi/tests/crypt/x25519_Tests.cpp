#include "psi/test/TestHelper.h"
#include "psi/test/psi_mock.h"

#include "psi/tools/ByteBuffer.h"
#include "psi/tools/crypt/x25519.h"

using namespace psi::tools;
using namespace psi::tools::crypt;
using namespace psi::test;

TEST(x25519_Tests, scalarmult_base)
{
    auto doTest = [](const auto &testCaseName, const auto &privateKey, const auto &expectedKey) {
        // SCOPED_TRACE(testCaseName);

        ByteBuffer privateKeyBuffer(privateKey, true);
        ByteBuffer publicKeyBuffer(32u);
        x25519::scalarmult_base(publicKeyBuffer.data(), privateKeyBuffer.data());

        EXPECT_EQ(publicKeyBuffer.asHexString(), expectedKey);
    };

    doTest("case 1. sender keys",
           "77076d0a7318a57d3c16c17251b26645df4c2f87ebc0992ab177fba51db92c2a",
           "8520f0098930a754748b7ddcb43ef75a0dbf3a0d26381af4eba4a98eaa9b4e6a");
    doTest("case 2. receiver keys",
           "5dab087e624a8a4b79e17f8b83800ee66f3bb1292618b6fd1c2f8b27ff88e0eb",
           "de9edb7d7b7dc1b4d35b61c2ece435373f8343c85b78674dadfc7e146f882b4f");
    doTest("case 3. client keys",
           "202122232425262728292a2b2c2d2e2f303132333435363738393a3b3c3d3e3f",
           "358072d6365880d1aeea329adf9121383851ed21a28e3b75e965d0d2cd166254");
    doTest("case 4. server keys",
           "909192939495969798999a9b9c9d9e9fa0a1a2a3a4a5a6a7a8a9aaabacadaeaf",
           "9fd7ad6dcff4298dd3f96d5b1b2af910a0535b1488d7f8fabb349a982880b615");
}

TEST(x25519_Tests, scalarmult)
{
    auto doTest = [](const auto &testCaseName, const auto &privateKey, const auto &publicKey, const auto &expectedKey) {
        // SCOPED_TRACE(testCaseName);

        ByteBuffer privateKeyBuffer(privateKey, true);
        ByteBuffer publicKeyBuffer(publicKey, true);
        ByteBuffer sharedKeyBuffer(32u);
        x25519::scalarmult(sharedKeyBuffer.data(), privateKeyBuffer.data(), publicKeyBuffer.data());

        EXPECT_EQ(sharedKeyBuffer.asHexString(), expectedKey);
    };

    doTest("case 1. shared key sender side",
           "77076d0a7318a57d3c16c17251b26645df4c2f87ebc0992ab177fba51db92c2a",
           "de9edb7d7b7dc1b4d35b61c2ece435373f8343c85b78674dadfc7e146f882b4f",
           "4a5d9d5ba4ce2de1728e3bf480350f25e07e21c947d19e3376f09b3c1e161742");
    doTest("case 2. shared key receiver side",
           "5dab087e624a8a4b79e17f8b83800ee66f3bb1292618b6fd1c2f8b27ff88e0eb",
           "8520f0098930a754748b7ddcb43ef75a0dbf3a0d26381af4eba4a98eaa9b4e6a",
           "4a5d9d5ba4ce2de1728e3bf480350f25e07e21c947d19e3376f09b3c1e161742");
    doTest("case 3. shared key client side",
           "202122232425262728292a2b2c2d2e2f303132333435363738393a3b3c3d3e3f",
           "9fd7ad6dcff4298dd3f96d5b1b2af910a0535b1488d7f8fabb349a982880b615",
           "df4a291baa1eb7cfa6934b29b474baad2697e29f1f920dcc77c8a0a088447624");
    doTest("case 4. shared key client side",
           "909192939495969798999a9b9c9d9e9fa0a1a2a3a4a5a6a7a8a9aaabacadaeaf",
           "358072d6365880d1aeea329adf9121383851ed21a28e3b75e965d0d2cd166254",
           "df4a291baa1eb7cfa6934b29b474baad2697e29f1f920dcc77c8a0a088447624");
}
