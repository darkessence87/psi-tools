#include <codecvt>
#include <ctime>

#include "psi/tools/Tools.h"

#include <chrono>

namespace psi::tools {

std::string wstring_to_utf8(const std::wstring &str) noexcept
{
    using convert_type = std::codecvt_utf8<wchar_t>;
    std::wstring_convert<convert_type, wchar_t> converter;
    return converter.to_bytes(str);
}

std::wstring utf8_to_wstring(const std::string &str) noexcept
{
    using convert_type = std::codecvt_utf8<wchar_t>;
    std::wstring_convert<convert_type, wchar_t> converter;
    return converter.from_bytes(str);
}

std::string generateTimeStamp() noexcept
{
    using namespace std::chrono;

    std::stringstream ss;
#ifdef __linux__
    const auto curTime = system_clock::now();
    const auto time = system_clock::to_time_t(curTime);
    const auto localTime = std::localtime(&time);
    ss << std::put_time(localTime, "%Y.%m.%d_%H.%M.%S");
#elif _WIN32
    const auto curTime = system_clock::now();
    struct tm buf;
    const auto time = system_clock::to_time_t(curTime);
    localtime_s(&buf, &time);
    ss << std::put_time(&buf, "%Y.%m.%d_%H.%M.%S");
#endif
    return ss.str();
}

} // namespace psi::tools
