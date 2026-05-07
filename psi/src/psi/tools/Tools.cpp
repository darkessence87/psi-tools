#ifdef _WIN32
#include <Windows.h>
#endif
#include <ctime>

#include "psi/tools/Tools.h"

#include <chrono>

namespace psi::tools {

std::string wstring_to_utf8(const std::wstring &str) noexcept
{
    if (str.empty()) {
        return {};
    }
#ifdef _WIN32
    const int size = WideCharToMultiByte(CP_UTF8, 0, str.c_str(), static_cast<int>(str.size()), nullptr, 0, nullptr, nullptr);
    if (size <= 0) {
        return {};
    }
    std::string result(static_cast<size_t>(size), '\0');
    WideCharToMultiByte(CP_UTF8, 0, str.c_str(), static_cast<int>(str.size()), result.data(), size, nullptr, nullptr);
    return result;
#else
    std::string result;
    for (wchar_t wc : str) {
        const auto cp = static_cast<uint32_t>(wc);
        if (cp < 0x80u) {
            result += static_cast<char>(cp);
        } else if (cp < 0x800u) {
            result += static_cast<char>(0xC0u | (cp >> 6));
            result += static_cast<char>(0x80u | (cp & 0x3Fu));
        } else if (cp < 0x10000u) {
            result += static_cast<char>(0xE0u | (cp >> 12));
            result += static_cast<char>(0x80u | ((cp >> 6) & 0x3Fu));
            result += static_cast<char>(0x80u | (cp & 0x3Fu));
        } else {
            result += static_cast<char>(0xF0u | (cp >> 18));
            result += static_cast<char>(0x80u | ((cp >> 12) & 0x3Fu));
            result += static_cast<char>(0x80u | ((cp >> 6) & 0x3Fu));
            result += static_cast<char>(0x80u | (cp & 0x3Fu));
        }
    }
    return result;
#endif
}

std::wstring utf8_to_wstring(const std::string &str) noexcept
{
    if (str.empty()) {
        return {};
    }
#ifdef _WIN32
    const int size = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), static_cast<int>(str.size()), nullptr, 0);
    if (size <= 0) {
        return {};
    }
    std::wstring result(static_cast<size_t>(size), L'\0');
    MultiByteToWideChar(CP_UTF8, 0, str.c_str(), static_cast<int>(str.size()), result.data(), size);
    return result;
#else
    std::wstring result;
    const auto *s = reinterpret_cast<const uint8_t *>(str.data());
    const auto *end = s + str.size();
    while (s < end) {
        uint32_t cp = 0;
        if (*s < 0x80u) {
            cp = *s++;
        } else if ((*s & 0xE0u) == 0xC0u && (s + 1) < end) {
            cp = static_cast<uint32_t>(*s++ & 0x1Fu) << 6;
            cp |= static_cast<uint32_t>(*s++ & 0x3Fu);
        } else if ((*s & 0xF0u) == 0xE0u && (s + 2) < end) {
            cp = static_cast<uint32_t>(*s++ & 0x0Fu) << 12;
            cp |= static_cast<uint32_t>(*s++ & 0x3Fu) << 6;
            cp |= static_cast<uint32_t>(*s++ & 0x3Fu);
        } else if ((*s & 0xF8u) == 0xF0u && (s + 3) < end) {
            cp = static_cast<uint32_t>(*s++ & 0x07u) << 18;
            cp |= static_cast<uint32_t>(*s++ & 0x3Fu) << 12;
            cp |= static_cast<uint32_t>(*s++ & 0x3Fu) << 6;
            cp |= static_cast<uint32_t>(*s++ & 0x3Fu);
        } else {
            ++s; // skip invalid byte
        }
        result += static_cast<wchar_t>(cp);
    }
    return result;
#endif
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
