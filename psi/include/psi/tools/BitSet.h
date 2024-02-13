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
    BitSet(uint8_t *data, size_t sz);

    /**
     * @brief Construct a new BitSet object.
     * 
     * @param bits string of bits
     */
    explicit BitSet(const std::string &bits);

    /**
     * @brief Return number of bits in an object.
     * 
     * @return size_t 
     */
    size_t size() const;

    /**
     * @brief Set or unset bit.
     * 
     * @param bitNumber 
     * @param set 
     */
    void set(size_t bitNumber, bool set = true);

    /**
     * @brief Test bit.
     * 
     * @param bitNumber 
     * @return true 
     * @return false 
     */
    bool test(size_t bitNumber) const;

    /**
     * @brief Inverse all bits.
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
     * 
     * @return std::string 
     */
    std::string toString() const;

private:
    std::shared_ptr<BitSetImpl> m_value;
};

} // namespace psi::tools