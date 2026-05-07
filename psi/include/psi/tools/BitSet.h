#pragma once

#include <memory>
#include <stdint.h>
#include <string>

namespace psi::tools {

/**
 * @brief Represent bit set up to [max(size_t)] bits.
 * 
 */
class BitSet
{
    struct BitSetImpl;

public:
    /**
     * @brief Construct a new BitSet object.
     * 
     */
    BitSet();

    /**
     * @brief Construct a new BitSet object.
     * 
     * @param bitsNumber number of bits
     */
    explicit BitSet(size_t bitsNumber);

    /**
     * @brief Construct a new Bit Set object from a byte stream.
     * 
     * @param data pointer to data
     * @param sz data size in bytes
     */
    BitSet(const uint8_t *data, size_t sz);

    /**
     * @brief Construct a new BitSet object.
     * 
     * @param bits string of bits
     */
    explicit BitSet(const std::string &bits);

    /**
     * @brief Return number of bits in an object.
     * 
     * @return size_t total number of bits
     */
    size_t size() const;

    /**
     * @brief Set or unset a bit.
     * 
     * @param bitNumber 0-based index of the bit to modify
     * @param set true to set the bit, false to clear it (default: true)
     */
    void set(size_t bitNumber, bool set = true);

    /**
     * @brief Test whether a bit is set.
     * 
     * @param bitNumber 0-based index of the bit to test
     * @return true if the bit is set
     * @return false if the bit is not set
     */
    bool test(size_t bitNumber) const;

    /**
     * @brief Flip (invert) all bits.
     * 
     */
    void inverse();

    /**
     * @brief Reverse all bits.
     * 
     */
    void reverse();

    /**
     * @brief Represent as string.
     * Each character is '0' or '1', ordered from the most significant to the least significant bit.
     * 
     * @return std::string string representation of all bits
     */
    const std::string toString() const;

private:
    std::shared_ptr<BitSetImpl> m_value;
};

} // namespace psi::tools
