#include "TestHelper.h"
#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "psi/tools/Tools.h"

#include <Windows.h>

#include <codecvt>
#include <locale>

using namespace ::testing;
using namespace psi;
using namespace psi::test;

TEST(ToolsTests, to_hex_string_buffer)
{
    uint8_t buff[] = {'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', '0', '1', '2', '3', '4'};
    EXPECT_EQ("61626364656667683031323334", tools::to_hex_string(buff, sizeof(buff)));
}

TEST(ToolsTests, to_hex_string_number)
{
    {
        uint64_t n = 123456789;
        EXPECT_EQ("075bcd15", tools::to_hex_string(n));
    }
    {
        uint64_t n = 4328719365;
        EXPECT_EQ("0102030405", tools::to_hex_string(n));
    }
    {
        uint64_t n = 6;
        EXPECT_EQ("06", tools::to_hex_string(n));
    }
    {
        uint64_t n = 15;
        EXPECT_EQ("0f", tools::to_hex_string(n));
    }
    {
        uint64_t n = 16;
        EXPECT_EQ("10", tools::to_hex_string(n));
    }
    {
        uint64_t n = 16 * 16;
        EXPECT_EQ("0100", tools::to_hex_string(n));
    }
    {
        uint64_t n = 16 * 16 * 16;
        EXPECT_EQ("1000", tools::to_hex_string(n));
    }
}

TEST(ToolsTests, utf8_to_wstring_to_utf8)
{
    std::u8string str1 = u8"\u0420\u0435\u0432\u0443\u0449\u0438\u0439 \u0444\u044c\u043e\u0440\u0434";
    std::wstring wstr1 = tools::utf8_to_wstring(std::string(str1.begin(), str1.end()));
    EXPECT_EQ(wstr1, std::wstring(L"\x420\x435\x432\x443\x449\x438\x439 \x444\x44C\x43E\x440\x434"));

    std::string str2 = tools::wstring_to_utf8(wstr1);
    EXPECT_TRUE(std::u8string(str2.begin(), str2.end()) == str1);
}

TEST(ToolsTests, performance)
{
    uint8_t buff64[64] = {
        'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd',
        'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd',
        'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd',
    };
    uint8_t buff2048[2048] = {
        'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd',
        'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd',
        'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd',
        'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd',
        'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd',
        'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd',
        'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd',
        'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd',
        'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd',
        'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd',
        'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd',
        'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd',
        'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd',
        'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd',
        'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd',
        'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd',
        'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd',
        'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd',
        'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd',
        'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd',
        'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd',
        'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd',
        'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd',
        'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd',
        'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd',
        'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd',
        'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd',
        'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd',
        'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd',
        'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd',
        'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd',
        'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd',
        'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd',
        'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd',
        'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd',
        'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd',
        'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd',
        'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd',
        'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd',
        'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd',
        'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd',
        'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd',
        'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd',
        'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd',
        'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd',
        'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd',
        'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd',
        'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd',
        'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd',
        'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd',
        'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd',
        'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd',
        'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd',
        'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd',
        'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd',
        'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd',
        'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd',
        'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd',
        'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd',
        'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd',
        'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd',
        'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd',
        'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd',
        'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd',
        'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd',
        'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd',
        'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd',
        'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd',
        'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd',
        'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd',
        'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd',
        'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd',
        'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd',
        'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd',
        'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd',
        'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd',
        'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd',
        'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd',
        'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd',
        'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd',
        'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd',
        'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd',
        'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd',
        'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd',
        'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd',
        'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd',
        'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd',
        'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd',
        'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd',
        'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd',
        'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd',
        'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd',
        'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd', 'c', 'd',
        'c', 'd',
    };

    TestHelper::timeFn(
        "to_hex_string buffer small (64 bytes)", [&]() { tools::to_hex_string(buff64, 64); }, 100000);

    TestHelper::timeFn(
        "to_hex_string buffer medium (2048 bytes)", [&]() { tools::to_hex_string(buff2048, 2048); }, 100000);

    TestHelper::timeFn(
        "to_hex_string number", []() { tools::to_hex_string(123456789); }, 1'000'000);
}
