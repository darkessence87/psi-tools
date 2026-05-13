#include "psi/test/TestHelper.h"
#include "psi/test/psi_mock.h"

#include "psi/tools/Tools.h"

#ifdef _WIN32
#include <Windows.h>
#endif

#include <codecvt>
#include <locale>

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
    EXPECT_EQ(std::u8string(str2.begin(), str2.end()), str1);
}

TEST(ToolsTests, to_hex_string_performance)
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

    TestHelper::timeFn("to_hex_string buffer small (64 bytes)", [&]() { tools::to_hex_string(buff64, 64); }, 100000);

    TestHelper::timeFn("to_hex_string buffer medium (2048 bytes)", [&]() { tools::to_hex_string(buff2048, 2048); }, 100000);

    TestHelper::timeFn("to_hex_string number", []() { tools::to_hex_string(123456789); }, 1'000'000);
}

TEST(ToolsTests, to_iso_8601_roundtrip)
{
    using namespace std::chrono;

    // Fixed point: 2026-05-12T10:30:00.000Z
    struct tm tmBuf {};
    tmBuf.tm_year = 2026 - 1900;
    tmBuf.tm_mon  = 5 - 1;
    tmBuf.tm_mday = 12;
    tmBuf.tm_hour = 10;
    tmBuf.tm_min  = 30;
    tmBuf.tm_sec  = 0;
#if defined(_WIN32)
    std::time_t t = _mkgmtime(&tmBuf);
#else
    std::time_t t = timegm(&tmBuf);
#endif
    auto tp = system_clock::from_time_t(t);

    std::string iso = tools::to_iso_8601(tp);
    EXPECT_EQ(iso, std::string("2026-05-12T10:30:00.000Z"));
}

TEST(ToolsTests, from_iso_8601_null_and_empty)
{
    // nullptr and empty string should return "now" without crashing
    auto tp1 = tools::from_iso_8601(nullptr);
    auto tp2 = tools::from_iso_8601("");
    // Just check they don't return epoch (i.e. they returned something close to now)
    auto epoch = std::chrono::system_clock::time_point{};
    EXPECT_EQ((tp1 == epoch), false);
    EXPECT_EQ((tp2 == epoch), false);
}

TEST(ToolsTests, optional_iso_8601_epoch_returns_empty)
{
    std::chrono::system_clock::time_point epoch{};
    EXPECT_EQ(tools::optional_iso_8601(epoch), std::string{});
}

TEST(ToolsTests, optional_iso_8601_nonepoch_returns_string)
{
    auto tp = std::chrono::system_clock::now();
    std::string result = tools::optional_iso_8601(tp);
    EXPECT_EQ(result.empty(), false);
}

TEST(ToolsTests, ptr_to_address)
{
    int x = 42;
    std::string addr = tools::ptr_to_address(&x);
    EXPECT_EQ(addr.substr(0, 2), std::string("0x"));
    EXPECT_EQ(addr.empty(), false);
}

TEST(ToolsTests, wstring_to_utf8_empty)
{
    std::wstring empty;
    EXPECT_EQ(tools::wstring_to_utf8(empty), std::string{});
}

TEST(ToolsTests, utf8_to_wstring_empty)
{
    std::string empty;
    EXPECT_EQ(tools::utf8_to_wstring(empty), std::wstring{});
}

TEST(ToolsTests, generateTimeStamp_not_empty)
{
    std::string ts = tools::generateTimeStamp();
    EXPECT_EQ(ts.empty(), false);
}
