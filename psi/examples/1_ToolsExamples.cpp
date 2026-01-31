#include "psi/tools/Tools.h"

int main()
{
    const auto str = psi::tools::wstring_to_utf8(L"Hello Вьорлд");
    std::cout << str << std::endl;

    const auto wstr = psi::tools::utf8_to_wstring(str);
    std::wcout << wstr << std::endl;
}
