#pragma once

#include <algorithm>
#include <functional>
#include <future>
#include <iomanip>
#include <iostream>
#include <locale>
#include <span>
#include <sstream>
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
 * @brief Converts input utf-8 formatted string to wstring
 * 
 * @return std::wstring new string
 */
std::wstring utf8_to_wstring(const std::string &) noexcept;

/**
 * @brief Converts input wstring to utf-8 formatted string
 * 
 * @return std::string new string
 */
std::string wstring_to_utf8(const std::wstring &) noexcept;

/**
 * @brief Generates current timestamp in a string format.
 * Format: "years.months.day_hours.minutes.seconds"
 * 
 * @return std::string formatted current time string
 */
std::string generateTimeStamp() noexcept;

/**
 * @brief Converts async function call to sync call.
 * The default timeout is: 10 seconds
 * 
 * @tparam Arg type of argument to be sent by async function
 * @param fn async function
 * @param arg argument to be sent by async function
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
 * @brief Swaps endianess 
 * 
 * @tparam T T
 * @param val val
 * @return T T
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

template <typename T>
inline T *shift_ptr(T *src, size_t src_offset)
{
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunsafe-buffer-usage"
    return src + src_offset;
#pragma clang diagnostic pop
}

template <typename T1, typename T2>
inline void mem_copy(T1 *to, size_t to_offset, const T2 *from, size_t from_offset, size_t sz)
{
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunsafe-buffer-usage-in-libc-call"
    std::memcpy(shift_ptr(to, to_offset), shift_ptr(from, from_offset), sz);
#pragma clang diagnostic pop
}

template <typename T>
inline void mem_set(T *to, size_t to_offset, T value, size_t sz)
{
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunsafe-buffer-usage-in-libc-call"
    std::memset(shift_ptr(to, to_offset), value, sz);
#pragma clang diagnostic pop
}

} // namespace psi::tools
