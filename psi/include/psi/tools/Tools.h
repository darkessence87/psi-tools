#pragma once

#include <algorithm>
#include <chrono>
#include <cstdio>
#include <ctime>
#include <functional>
#include <future>
#include <iomanip>
#include <iostream>
#include <locale>
#include <span>
#include <sstream>
#include <string>
#include <vector>

namespace psi::tools {

static constexpr char g_hex_lookup_table[17] = "0123456789abcdef";

/**
 * @brief Converts input byte buffer to string in hex format.
 * 
 * @param buffer input buffer
 * @param sz length of input buffer
 * @return std::string hex formatted input data
 */
inline std::string to_hex_string(uint8_t *buffer, size_t sz)
{
    std::string result;
    result.resize(sz * 2);

    char *dst = result.data();
    size_t index = 0;
    for (size_t i = 0; i < sz; ++i) {
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunsafe-buffer-usage"
        const uint8_t c = buffer[i];
        dst[index++] = g_hex_lookup_table[c >> 4];
        dst[index++] = g_hex_lookup_table[c & 0xf];
#pragma clang diagnostic pop
    }

    return result;
}

/// Converts a system_clock time_point to an ISO-8601 string with millisecond
/// precision, e.g. "2026-05-06T00:32:26.347Z".
inline std::string to_iso_8601(std::chrono::system_clock::time_point tp)
{
    std::time_t t = std::chrono::system_clock::to_time_t(tp);
    struct tm tmBuf {};
#if defined(_WIN32)
    gmtime_s(&tmBuf, &t);
#else
    gmtime_r(&t, &tmBuf);
#endif
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(tp.time_since_epoch()) % 1000;
    char buf[32];
    std::strftime(buf, sizeof(buf), "%Y-%m-%dT%H:%M:%S", &tmBuf);
    char out[40];
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunsafe-buffer-usage-in-libc-call"
    std::snprintf(out, sizeof(out), "%s.%03dZ", buf, static_cast<int>(ms.count()));
#pragma clang diagnostic pop
    return out;
}

/// Parses a basic ISO-8601 UTC string (e.g. "2026-05-06T00:32:26Z") back to
/// a system_clock time_point.  Returns now() on parse failure.
inline std::chrono::system_clock::time_point from_iso_8601(const char *s)
{
    if (!s || s[0] == '\0') {
        return std::chrono::system_clock::now();
    }
    struct tm tmBuf {};
#if defined(_WIN32)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunsafe-buffer-usage-in-libc-call"
    sscanf_s(s, "%d-%d-%dT%d:%d:%dZ",
             &tmBuf.tm_year, &tmBuf.tm_mon, &tmBuf.tm_mday,
             &tmBuf.tm_hour, &tmBuf.tm_min, &tmBuf.tm_sec);
#pragma clang diagnostic pop
#else
    sscanf(s, "%d-%d-%dT%d:%d:%dZ",
           &tmBuf.tm_year, &tmBuf.tm_mon, &tmBuf.tm_mday,
           &tmBuf.tm_hour, &tmBuf.tm_min, &tmBuf.tm_sec);
#endif
    tmBuf.tm_year -= 1900;
    tmBuf.tm_mon  -= 1;
    std::time_t t =
#if defined(_WIN32)
        _mkgmtime(&tmBuf);
#else
        timegm(&tmBuf);
#endif
    return std::chrono::system_clock::from_time_t(t);
}

/// Returns toIso8601(tp), or an empty string when tp is the default epoch value.
inline std::string optional_iso_8601(std::chrono::system_clock::time_point tp)
{
    if (tp == std::chrono::system_clock::time_point{})
        return {};
    return to_iso_8601(tp);
}

/**
 * @brief Converts input 8-bytes integer to string in hex format.
 * 
 * @param val input 8-bytes integer
 * @return std::string hex formatted input data
 */
inline std::string to_hex_string(uint64_t val)
{
    std::string result;
    result.reserve(16);

    bool is_leading_zero = true;
    for (int i = 7; i >= 0; --i) {
        const uint8_t c = uint8_t(val >> (i * 8));
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunsafe-buffer-usage"
        const char v1 = g_hex_lookup_table[c >> 4];
        const char v2 = g_hex_lookup_table[c & 0xf];
#pragma clang diagnostic pop
        if (is_leading_zero && v1 == '0' && v2 == '0') {
            continue;
        }

        is_leading_zero = false;
        result.push_back(v1);
        result.push_back(v2);
    }

    return result;
}

/**
 * @brief Converts pointer to address in hex format.
 * 
 * @tparam T type of object
 * @param ptr pointer to object
 * @return std::string std::string hex formatted input data
 */
template <typename T>
inline std::string ptr_to_address(T *ptr)
{
    return "0x" + to_hex_string(reinterpret_cast<size_t>(ptr));
}

/**
 * @brief Parses input string to list of [key,value] pairs using provided delimiter.
 * Each pair is delimited by 'new line'
 * Each key and value are delimited by provided delimiter.
 * 
 * @param s input string
 * @param delimiter delimiter
 * @return std::vector<std::pair<std::string, std::string>> list of [key,value] pairs
 */
inline std::vector<std::pair<std::string, std::string>> parse_to_map(const std::string &s, char delimiter) noexcept
{
    std::vector<std::pair<std::string, std::string>> tokens;
    std::string tokenLine;

    std::istringstream tokenStream(s);
    while (std::getline(tokenStream, tokenLine)) {
        const auto pos = tokenLine.find(delimiter);
        if (pos != std::string::npos) {
            const auto tokenName = tokenLine.substr(0, pos);
            const auto tokenValue = tokenLine.substr(pos + 1, tokenLine.size());
            tokens.emplace_back(std::pair<std::string, std::string> {tokenName, tokenValue});
        }
    }
    return tokens;
}

/**
 * @brief Parses input string to list of tokens using provided delimiter.
 * 
 * @param s input string
 * @param delimiter delimiter
 * @return std::vector<std::string> 
 */
inline std::vector<std::string> parse_to_vector(const std::string &s, char delimiter) noexcept
{
    std::vector<std::string> tokens;
    std::string tokenLine;

    std::istringstream tokenStream(s);
    while (std::getline(tokenStream, tokenLine, delimiter)) {
        tokens.emplace_back(tokenLine);
    }
    return tokens;
}

/**
 * @brief Removes special symbols from the beginning of provided string.
 * 
 * @param str input string
 * @param chars list of symbols to be removed
 * @return std::string& modified input string
 */
inline std::string &ltrim(std::string &str, const std::string &chars = "\t\n\v\f\r ") noexcept
{
    const auto pos = str.find_first_not_of(chars);
    if (pos != std::string::npos) {
        str.erase(0, pos);
    } else {
        str.clear();
    }
    return str;
}

/**
 * @brief Removes special symbols from the end of provided string.
 * 
 * @param str input string
 * @param chars list of symbols to be removed
 * @return std::string& modified input string
 */
inline std::string &rtrim(std::string &str, const std::string &chars = "\t\n\v\f\r ") noexcept
{
    const auto pos = str.find_last_not_of(chars);
    if (pos != std::string::npos) {
        str.erase(pos + 1);
    } else {
        str.clear();
    }
    return str;
}

/**
 * @brief Removes special symbols from the beginning and the end of provided string.
 * 
 * @param str input string
 * @param chars list of symbols to be removed
 * @return std::string& modified input string
 */
inline std::string &trim(std::string &str, const std::string &chars = "\t\n\v\f\r ") noexcept
{
    return ltrim(rtrim(str, chars), chars);
}

/**
 * @brief Converts each symbol of provided string to upper case.
 * 
 * @param str input string
 * @return std::string new string with upper case
 */
inline std::string to_upper(const std::string &str) noexcept
{
    std::string data;
    data.resize(str.size());
    for (size_t index = 0; index < str.size(); ++index) {
        data[index] = static_cast<char>(std::toupper(static_cast<uint8_t>(str[index])));
    }
    return data;
}

/**
 * @brief Converts a UTF-8 encoded string to a wide string.
 * Uses the platform-native wide encoding (UTF-16 on Windows, UTF-32 on Linux/macOS).
 * 
 * @param str UTF-8 encoded input string
 * @return std::wstring wide string representation
 */
std::wstring utf8_to_wstring(const std::string &str) noexcept;

/**
 * @brief Converts a wide string to a UTF-8 encoded string.
 * Uses the platform-native wide encoding (UTF-16 on Windows, UTF-32 on Linux/macOS).
 * 
 * @param str wide string input
 * @return std::string UTF-8 encoded representation
 */
std::string wstring_to_utf8(const std::wstring &str) noexcept;

/**
 * @brief Generates current timestamp in a string format.
 * Format: "years.months.day_hours.minutes.seconds"
 * 
 * @return std::string formatted current time string
 */
std::string generateTimeStamp() noexcept;

/**
 * @brief Converts an async callback-based function into a blocking call.
 * Blocks until the callback is invoked or the timeout elapses.
 * On timeout, @p arg is set to a default-constructed Arg().
 * 
 * @tparam Arg type of the result value delivered by the async function
 * @param fn async function that accepts a callback of type void(Arg)
 * @param arg (out) result value filled in by the async function
 * @param timeout maximum wait time in seconds (default: 10)
 */
template <typename Arg>
void convertToSyncCall(std::function<void(std::function<void(Arg)>)> fn, Arg &arg, uint8_t timeout = 10) noexcept
{
    std::promise<Arg> p;
    std::future<Arg> f = p.get_future();
    auto validator = std::make_shared<bool>(false);
    auto cb = [&p, validator](Arg res) {
        if (!validator || *validator) {
            return;
        }
        *validator = true;
        p.set_value(res);
    };
    fn(cb);

    std::future_status callStatus = f.wait_for(std::chrono::seconds(timeout));
    if (std::future_status::timeout == callStatus) {
        std::cout << "Async call timeout. Sending fallback" << std::endl;
        arg = Arg();
    } else {
        arg = f.get();
    }
    *validator = true;
}

/**
 * @brief Returns object's address and name.
 * 
 * @tparam T type of object
 * @param obj object
 * @return std::string new string, format: "(0xhex_address):typeid"
 */
template <typename T>
inline std::string objName(const T &obj) noexcept
{
    const size_t address = reinterpret_cast<size_t>(&obj);
    return "(0x" + to_hex_string(address) + "):" + typeid(obj).name();
}

/**
 * @brief Reverses the byte order (endianness) of a trivially-copyable value.
 * 
 * @tparam T trivially-copyable type
 * @param val input value
 * @return T value with bytes in reversed order
 */
template <typename T>
inline T swapEndian(const T &val)
{
    auto in = std::as_bytes(std::span(&val, 1));
    T result;
    auto out = std::as_writable_bytes(std::span(&result, 1));
    std::copy(in.rbegin(), in.rend(), out.begin());
    return result;
}

} // namespace psi::tools
