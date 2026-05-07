#pragma once

#include <stdint.h>
#include <string>
#include <vector>

namespace psi::tools {

/**
 * @brief Represent integer value up to [2^max(uint64_t) * 8] bits.
 * 
 */
class BigInteger
{
public:
    /**
     * @brief Construct a new BigInteger object.
     * 
     */
    BigInteger();

    /**
     * @brief Construct a new BigInteger object.
     * 
     * @param v 0-index bit value
     */
    explicit BigInteger(uint64_t v);

    /**
     * @brief Construct a new BigInteger object.
     * 
     * @param v n-index bit value, lower index = lower bit
     */
    explicit BigInteger(const std::vector<uint64_t> &v);

    /**
     * @brief Sum current value with input value.
     * New object returned.
     * 
     * @param i BigInteger input value
     * @return BigInteger new result value
     */
    BigInteger operator+(const BigInteger &i) const;

    /**
     * @brief Sum current value with input value.
     * New object returned.
     * 
     * @param i uint64_t input value
     * @return BigInteger new result value
     */
    BigInteger operator+(uint64_t i) const;

    /**
     * @brief Sum current value with input value.
     * Modified object returned.
     * 
     * @param i BigInteger input value
     * @return BigInteger& reference to modified object
     */
    BigInteger &operator+=(const BigInteger &i);

    /**
     * @brief Sum current value with input value.
     * Modified object returned.
     * 
     * @param i uint64_t input value
     * @return BigInteger& reference to modified object
     */
    BigInteger &operator+=(uint64_t i);

    /**
     * @brief Subtract input value from current value.
     * New object returned.
     * 
     * @param i BigInteger input value
     * @return BigInteger new result value
     */
    BigInteger operator-(const BigInteger &i) const;

    /**
     * @brief Subtract input value from current value.
     * New object returned.
     * 
     * @param i uint64_t input value
     * @return BigInteger new result value
     */
    BigInteger operator-(uint64_t i) const;

    /**
     * @brief Subtract input value from current value.
     * Modified object returned.
     * 
     * @param i BigInteger input value
     * @return BigInteger& reference to modified object
     */
    BigInteger &operator-=(const BigInteger &i);

    /**
     * @brief Subtract input value from current value.
     * Modified object returned.
     * 
     * @param i uint64_t input value
     * @return BigInteger& reference to modified object
     */
    BigInteger &operator-=(uint64_t i);

    /**
     * @brief Represent BigInteger object as string.
     * Format: each 64-bit word printed in hex separated by spaces, from most significant to least significant.
     * 
     * @return std::string hex string representation
     */
    std::string toString() const;

    /**
     * @brief Add a value at the given word index.
     * 
     * @param i uint64_t value to add
     * @param index word index (0 = least significant word)
     */
    void add(uint64_t i, uint64_t index);

    /**
     * @brief Subtract input bit value from current value at the given word index.
     * 
     * @param i uint64_t value to subtract
     * @param index word index (0 = least significant word)
     */
    void substract(uint64_t i, uint64_t index);

    struct BigInteger_Tests;
private:
    // lower index = lower bits
    std::vector<uint64_t> m_value;
};

} // namespace psi::tools
