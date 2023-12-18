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

/**
 * @brief Converts input byte buffer to string in hex format.
 * 
 * @param buffer input buffer
 * @param sz length of input buffer
 * @return std::string hex formatted input data
 */
inline std::string to_hex_string(uint8_t *buffer, size_t sz) noexcept
{
    std::ostringstream os;
    os << std::hex << std::setfill('0');
    for (size_t i = 0; i < sz; ++i) {
        os << std::setw(2) << uint16_t(buffer[i]) << " ";
    }
    return os.str();
}

/**
 * @brief Converts input 8-bytes integer to string in hex format.
 * 
 * @param val input 8-bytes integer
 * @return std::string hex formatted input data
 */
inline std::string to_hex_string(uint64_t val) noexcept
{
    std::ostringstream os;
    os << std::setfill('0') << std::setw(2) << std::hex << val;
    return os.str();
}

/**
 * @brief Converts pointer to address in hex format.
 * 
 * @tparam T type of object
 * @param ptr pointer to object
 * @return std::string std::string hex formatted input data
 */
template <typename T>
inline std::string ptr_to_address(T *ptr) noexcept
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
            tokens.emplace_back(std::pair {tokenName, tokenValue});
        }
    }
    return tokens;
}

/**
 * @brief Parses input string to list of tokens using provided delimiter.
 * 
 * @param s input string
 * @param delimiter 
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
        data[index] = uint8_t(::toupper(str[index]));
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
    auto cb = [&p](Arg res) { p.set_value(res); };
    fn(cb);

    std::future_status callStatus = f.wait_for(std::chrono::seconds(timeout));
    if (std::future_status::timeout == callStatus) {
        std::cout << "Async call timeout. Sending fallback" << std::endl;
        arg = Arg();
    } else {
        arg = f.get();
    }
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
 * @brief 
 * 
 * @tparam T 
 * @param val 
 * @return T 
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
