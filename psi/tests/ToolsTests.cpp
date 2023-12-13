
#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "psi/tools/Tools.h"

#include <Windows.h>

#include <codecvt>
#include <locale>

using namespace ::testing;
using namespace psi;

TEST(ToolsTests, utf8_to_wstring_to_utf8)
{
    std::u8string str1 = u8"\u0420\u0435\u0432\u0443\u0449\u0438\u0439 \u0444\u044c\u043e\u0440\u0434";
    std::wstring wstr1 = tools::utf8_to_wstring(std::string(str1.begin(), str1.end()));
    EXPECT_EQ(wstr1, std::wstring(L"\x420\x435\x432\x443\x449\x438\x439 \x444\x44C\x43E\x440\x434"));

    std::string str2 = tools::wstring_to_utf8(wstr1);
    EXPECT_TRUE(std::u8string(str2.begin(), str2.end()) == str1);
}
