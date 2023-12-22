#include "TestHelper.h"
#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "psi/tools/crypt/aes_gcm.h"

using namespace psi::tools;
using namespace psi::tools::crypt;

TEST(aes_gcm_Tests, gfMultBlock)
{
    auto doTest = [](const auto &testCase, const auto &x, const auto &y, const auto &expected) {
        SCOPED_TRACE(testCase);

        ByteBuffer xBuffer(x, true);
        ByteBuffer yBuffer(y, true);
        aes_gcm::DataBlock16 z {};
        aes_gcm::gfMultBlock(xBuffer.data(), yBuffer.data(), z);
        ByteBuffer zBuffer(16);
        zBuffer.write(z);
        EXPECT_EQ(zBuffer.asHexString(), expected);
    };

    doTest("// case 1",
           "0388dace60b6a392f328c2b971b2fe78",
           "66e94bd4ef8a2c3b884cfa59ca342b2e",
           "5e2ec746917062882c85b0685353deb7");

    doTest("// case 2",
           "5e2ec746917062882c85b0685353de37",
           "66e94bd4ef8a2c3b884cfa59ca342b2e",
           "f38cbb1ad69223dcc3457ae5b6b0f885");

    doTest("// case 3",
           "ba471e049da20e40495e28e58ca8c555",
           "b83b533708bf535d0aa6e52980d53b78",
           "b714c9048389afd9f9bc5c1d4378e052");

    doTest("// case 4",
           "acbef20579b4b8ebce889bac8732dad7",
           "ed95f8e164bf3213febc740f0bd9c4af",
           "4db870d37cb75fcb46097c36230d1612");
}

TEST(aes_gcm_Tests, encrypt)
{
    {
        SCOPED_TRACE("// case 1.");

        const ByteBuffer key("00000000000000000000000000000000", true);
        const ByteBuffer data("");
        ByteBuffer iv(16);
        iv.writeHexString("000000000000000000000000");

        aes_gcm::Tag tag;
        ByteBuffer encodedData = aes_gcm::encrypt(data, key, iv, tag);
        ByteBuffer tagBuffer(16);
        tagBuffer.write(tag);
        EXPECT_EQ(encodedData.asHexString(), "");
        EXPECT_EQ(tagBuffer.asHexString(), "58e2fccefa7e3061367f1d57a4e7455a");
    }

    {
        SCOPED_TRACE("// case 2.");

        const ByteBuffer key("00000000000000000000000000000000", true);
        const ByteBuffer data("00000000000000000000000000000000", true);
        ByteBuffer iv(16);
        iv.writeHexString("000000000000000000000000");

        aes_gcm::Tag tag;
        ByteBuffer encodedData = aes_gcm::encrypt(data, key, iv, tag);
        ByteBuffer tagBuffer(16);
        tagBuffer.write(tag);
        EXPECT_EQ(encodedData.asHexString(), "0388dace60b6a392f328c2b971b2fe78");
        EXPECT_EQ(tagBuffer.asHexString(), "ab6e47d42cec13bdf53a67b21257bddf");
    }

    {
        SCOPED_TRACE("// case 3.");

        const ByteBuffer key("feffe9928665731c6d6a8f9467308308", true);
        const ByteBuffer data("d9313225f88406e5a55909c5aff5269a86a7a9531534f7da2e4c303d8a318a721c3c0c95956809532fcf0e24"
                              "49a6b525b16aedf5aa0de657ba637b391aafd255",
                              true);
        ByteBuffer iv(16);
        iv.writeHexString("cafebabefacedbaddecaf888");

        aes_gcm::Tag tag;
        ByteBuffer encodedData = aes_gcm::encrypt(data, key, iv, tag);
        ByteBuffer tagBuffer(16);
        tagBuffer.write(tag);
        EXPECT_EQ(encodedData.asHexString(), "42831ec2217774244b7221b784d0d49ce3aa212f2c02a4e035c17e2329aca12e21d514b25466931c7d8f6a5aac84aa051ba30b396a0aac973d58e091473f5985");
        EXPECT_EQ(tagBuffer.asHexString(), "4d5c2af327cd64a62cf35abd2ba6fab4");
    }

    {
        SCOPED_TRACE("// case 4.");

        const ByteBuffer key("feffe9928665731c6d6a8f9467308308", true);
        const ByteBuffer data("d9313225f88406e5a55909c5aff5269a86a7a9531534f7da2e4c303d8a318a721c3c0c95956809532fcf0e24"
                              "49a6b525b16aedf5aa0de657ba637b39",
                              true);
        const ByteBuffer acc("feedfacedeadbeeffeedfacedeadbeefabaddad2", true);
        ByteBuffer iv(16);
        iv.writeHexString("cafebabefacedbaddecaf888");

        aes_gcm::Tag tag;
        ByteBuffer encodedData = aes_gcm::encrypt(data, key, iv, tag, acc);
        ByteBuffer tagBuffer(16);
        tagBuffer.write(tag);
        EXPECT_EQ(encodedData.asHexString(), "42831ec2217774244b7221b784d0d49ce3aa212f2c02a4e035c17e2329aca12e21d514b25466931c7d8f6a5aac84aa051ba30b396a0aac973d58e091");
        EXPECT_EQ(tagBuffer.asHexString(), "5bc94fbc3221a5db94fae95ae7121a47");
    }

    {
        SCOPED_TRACE("// case 5.");

        const ByteBuffer key("feffe9928665731c6d6a8f9467308308", true);
        const ByteBuffer data("d9313225f88406e5a55909c5aff5269a86a7a9531534f7da2e4c303d8a318a721c3c0c95956809532fcf0e24"
                              "49a6b525b16aedf5aa0de657ba637b39",
                              true);
        const ByteBuffer acc("feedfacedeadbeeffeedfacedeadbeefabaddad2", true);
        ByteBuffer iv(16);
        iv.writeHexString("cafebabefacedbad");

        aes_gcm::Tag tag;
        ByteBuffer encodedData = aes_gcm::encrypt(data, key, iv, tag, acc);
        ByteBuffer tagBuffer(16);
        tagBuffer.write(tag);
        EXPECT_EQ(encodedData.asHexString(), "61353b4c2806934a777ff51fa22a4755699b2a714fcdc6f83766e5f97b6c742373806900e49f24b22b097544d4896b424989b5e1ebac0f07c23f4598");
        EXPECT_EQ(tagBuffer.asHexString(), "3612d2e79e3b0785561be14aaca2fccb");
    }

    {
        SCOPED_TRACE("// case 6.");

        const ByteBuffer key("feffe9928665731c6d6a8f9467308308", true);
        const ByteBuffer data("d9313225f88406e5a55909c5aff5269a86a7a9531534f7da2e4c303d8a318a721c3c0c95956809532fcf0e24"
                              "49a6b525b16aedf5aa0de657ba637b39",
                              true);
        const ByteBuffer acc("feedfacedeadbeeffeedfacedeadbeefabaddad2", true);
        ByteBuffer iv("9313225df88406e555909c5aff5269aa6a7a9538534f7da1e4c303d2a318a728c3c0c95156809539fcf0e2429a6b5254"
                      "16aedbf5a0de6a57a637b39b",
                      true);

        aes_gcm::Tag tag;
        ByteBuffer encodedData = aes_gcm::encrypt(data, key, iv, tag, acc);
        ByteBuffer tagBuffer(16);
        tagBuffer.write(tag);
        EXPECT_EQ(encodedData.asHexString(), "8ce24998625615b603a033aca13fb894be9112a5c3a211a8ba262a3cca7e2ca701e4a9a4fba43c90ccdcb281d48c7c6fd62875d2aca417034c34aee5");
        EXPECT_EQ(tagBuffer.asHexString(), "619cc5aefffe0bfa462af43c1699d050");
    }
}
