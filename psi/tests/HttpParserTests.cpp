#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "psi/tools/HttpParser.h"

using namespace psi::tools;

TEST(HttpParserTests, parseHttpChunkedMessage)
{
    const std::string expectedHeader = "HTTP/1.1 200 OK";
    const std::map<std::string, std::string> expectedMeta {
        {"DATE", "Mon, 08 Jan 2024 00:25:41 GMT"},
        {"CONTENT-TYPE", "application/json"},
        {"TRANSFER-ENCODING", "chunked"},
        {"CONNECTION", "keep-alive"},
    };
    auto doTest = [&](const auto &testCaseName,
                      const auto &msg,
                      const auto &expectedData,
                      const auto &expectedRemainingSz,
                      const auto &expectedEndData) {
        SCOPED_TRACE(testCaseName);

        const ByteBuffer request(msg);

        std::string header;
        std::map<std::string, std::string> meta;
        std::ostringstream data;
        size_t remainingSz;
        EXPECT_EQ(expectedEndData, HttpParser::parseHttpChunkedMessage(request, header, meta, remainingSz, data));
        EXPECT_EQ(expectedHeader, header);
        EXPECT_EQ(expectedMeta, meta);
        EXPECT_EQ(expectedData, data.str());
        EXPECT_EQ(expectedRemainingSz, remainingSz);
    };

    doTest("case 1. data block + end block",
           "HTTP/1.1 200 OK\r\nDate: Mon, 08 Jan 2024 00:25:41 GMT\r\nContent-Type: "
           "application/json\r\nTransfer-Encoding: chunked\r\nConnection: "
           "keep-alive\r\n\r\n9\r\ntest_data\r\n0\r\n\x17",
           "test_data",
           0,
           true);
    doTest("case 2. 2 data blocks + end block",
           "HTTP/1.1 200 OK\r\nDate: Mon, 08 Jan 2024 00:25:41 GMT\r\nContent-Type: "
           "application/json\r\nTransfer-Encoding: chunked\r\nConnection: "
           "keep-alive\r\n\r\n9\r\ntest_data\r\na\r\ntest_data2\r\n0\r\n\x17",
           "test_datatest_data2",
           0,
           true);
    doTest("case 3. data block + part of block",
           "HTTP/1.1 200 OK\r\nDate: Mon, 08 Jan 2024 00:25:41 GMT\r\nContent-Type: "
           "application/json\r\nTransfer-Encoding: chunked\r\nConnection: "
           "keep-alive\r\n\r\n9\r\ntest_data\r\na\r\ntest_dat\r\n\x17",
           "test_datatest_dat",
           2,
           false);
    doTest("case 4. data block",
           "HTTP/1.1 200 OK\r\nDate: Mon, 08 Jan 2024 00:25:41 GMT\r\nContent-Type: "
           "application/json\r\nTransfer-Encoding: chunked\r\nConnection: "
           "keep-alive\r\n\r\n9\r\ntest_data\r\n\x17",
           "test_data",
           HttpParser::MAX_MSG_LENGTH,
           false);
}

TEST(HttpParserTests, parseHttpFragment)
{
    auto doTest = [](const auto &testCaseName,
                     const auto &fragmentData,
                     const auto &originRemainingSz,
                     const auto &expectedData,
                     const auto &expectedRemainingSz,
                     const auto &expectedEndData) {
        SCOPED_TRACE(testCaseName);

        const ByteBuffer fragment(fragmentData);
        size_t remainingSz = originRemainingSz;

        std::ostringstream data;
        EXPECT_EQ(expectedEndData, HttpParser::parseHttpFragment(fragment, remainingSz, data));
        EXPECT_EQ(expectedData, data.str());
        EXPECT_EQ(expectedRemainingSz, remainingSz);
    };

    doTest("case 1. data block + end block", "9\r\ntest_data\r\n0\r\n\x17", HttpParser::MAX_MSG_LENGTH, "test_data", 0, true);
    doTest("case 2. 2 data blocks + end block",
           "9\r\ntest_data\r\na\r\ntest_data2\r\n0\r\n\x17",
           HttpParser::MAX_MSG_LENGTH,
           "test_datatest_data2",
           0,
           true);
    doTest("case 3. data block + part of block",
           "9\r\ntest_data\r\na\r\ntest_dat\r\n\x17",
           HttpParser::MAX_MSG_LENGTH,
           "test_datatest_dat",
           2,
           false);
    doTest("case 4. data block",
           "9\r\ntest_data\r\n\x17",
           HttpParser::MAX_MSG_LENGTH,
           "test_data",
           HttpParser::MAX_MSG_LENGTH,
           false);
    doTest("case 5. part of block", "part_test_data\r\n\x17", 100, "part_test_data", 100 - 14, false);
    doTest("case 6. end of part of block", "part_test_data\r\n\x17", 14, "part_test_data", HttpParser::MAX_MSG_LENGTH, false);
    doTest("case 7. end of part of block + data block",
           "part_test_data\r\n9\r\ntest_data\x17",
           14,
           "part_test_datatest_data",
           HttpParser::MAX_MSG_LENGTH,
           false);
    doTest("case 8. end of part of block + part of data block",
           "part_test_data\r\na\r\ntest_data\x17",
           14,
           "part_test_datatest_data",
           1,
           false);
    doTest("case 9. end of part of block + data block + part of data block",
           "part_test_data\r\na\r\ntest_data0\r\na\r\ntest_data\x17",
           14,
           "part_test_datatest_data0test_data",
           1,
           false);
    doTest("case 10. end of part of block + data block + end block",
           "part_test_data\r\na\r\ntest_data0\r\n0\r\n\x17",
           14,
           "part_test_datatest_data0",
           0,
           true);
}

TEST(HttpParserTests, skipToHttpData)
{
    auto doTest = [&](const auto &testCaseName, const auto &msg, const auto &expectedMsg) {
        SCOPED_TRACE(testCaseName);

        const ByteBuffer request(msg);
        const ByteBuffer expected(expectedMsg);

        HttpParser::skipToHttpData(request);
        EXPECT_EQ(request.readToByteBuffer(request.remainingLength()).asString(), expected.asString());
    };

    doTest("case 1. header + 2 data blocks",
           "HTTP/1.1 200 OK\r\nDate: Mon, 08 Jan 2024 00:25:41 GMT\r\nContent-Type: "
           "application/json\r\nTransfer-Encoding: chunked\r\nConnection: "
           "keep-alive\r\n\r\n9\r\ntest_data\r\n0\r\n\x17",
           "9\r\ntest_data\r\n0\r\n\x17");
    doTest("case 2. header + 3 data blocks",
           "HTTP/1.1 200 OK\r\nDate: Mon, 08 Jan 2024 00:25:41 GMT\r\nContent-Type: "
           "application/json\r\nTransfer-Encoding: chunked\r\nConnection: "
           "keep-alive\r\n\r\n9\r\ntest_data\r\na\r\ntest_data0\r\n0\r\n\x17",
           "9\r\ntest_data\r\na\r\ntest_data0\r\n0\r\n\x17");
}
