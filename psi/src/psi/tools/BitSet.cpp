#include "psi/tools/BitSet.h"

#include <vector>

namespace psi::tools {

struct BitSet::BitSetImpl {
    BitSetImpl(size_t bitsNumber)
        : m_bitsNumber(bitsNumber)
    {
        const size_t requiredMore = bitsNumber % 64;
        m_value.resize(bitsNumber / 64 + (requiredMore ? 1 : 0));
    }

    std::vector<uint64_t> m_value;
    size_t m_bitsNumber;
};

BitSet::BitSet()
    : BitSet(0)
{
}

BitSet::BitSet(size_t bitsNumber)
    : m_value(std::make_shared<BitSetImpl>(bitsNumber))
{
}

auto fill64(uint8_t *data, size_t bytesNumber)
{
    uint64_t value = 0;
    for (uint8_t k = 0; k < bytesNumber; ++k) {
        value |= uint64_t(data[k]) << (k * 8);
    }
    return value;
}

BitSet::BitSet(uint8_t *data, size_t sz)
    : BitSet(sz * 8)
{
    // main chunks
    const auto mainChunksNumber = sz / 8;
    for (size_t i = 0; i < mainChunksNumber; ++i) {
        m_value->m_value[i] = fill64(&data[i * 8], 8);
    }

    // last chunk
    const auto lastChunkSz = sz % 8;
    if (lastChunkSz) {
        *m_value->m_value.rbegin() = fill64(&data[mainChunksNumber * 8], lastChunkSz);
    }
}

auto fill64Bin(uint8_t *data, size_t bitsNumber)
{
    uint64_t value = 0;
    for (uint8_t k = 0; k < bitsNumber; ++k) {
        if (data[k] == '1') {
            value |= 1ull << k;
        } else if (data[k] != '0') {
            return 0ull;
        }
    }
    return value;
}

BitSet::BitSet(const std::string &bits)
    : BitSet(bits.size())
{
    // main chunks
    const auto mainChunksNumber = bits.size() / 64;
    for (size_t i = 0; i < mainChunksNumber; ++i) {
        m_value->m_value[i] = fill64Bin((uint8_t *)&bits.data()[i * 64], 64);
    }

    // last chunk
    const auto lastChunkSz = bits.size() % 64;
    if (lastChunkSz) {
        *m_value->m_value.rbegin() = fill64Bin((uint8_t *)&bits.data()[mainChunksNumber * 64], lastChunkSz);
    }
}

size_t BitSet::size() const
{
    return m_value->m_bitsNumber;
}

void BitSet::set(size_t bitNumber, bool set)
{
    if (bitNumber >= m_value->m_bitsNumber) {
        return;
    }

    auto &sub = m_value->m_value[bitNumber / 64u];
    if (set) {
        sub |= 1ull << (bitNumber % 64u);
    } else {
        sub &= ~(1ull << (bitNumber % 64u));
    }
}

bool BitSet::test(size_t bitNumber) const
{
    if (bitNumber >= m_value->m_bitsNumber) {
        return false;
    }

    auto &sub = m_value->m_value[bitNumber / 64u];
    return (sub >> (bitNumber % 64u)) & 0x1;
}

void BitSet::inverse()
{
    for (auto &v : m_value->m_value) {
        v = ~v;
    }
}

uint8_t reverseByte(uint8_t b)
{
    b = (b & 0xf0) >> 4 | (b & 0x0f) << 4;
    b = (b & 0xcc) >> 2 | (b & 0x33) << 2;
    b = (b & 0xaa) >> 1 | (b & 0x55) << 1;
    return b;
}

uint64_t reverse64(uint64_t v)
{
    uint64_t r = 0;
    for (uint8_t i = 0; i < 8; ++i) {
        r |= uint64_t(reverseByte((v >> (i * 8)) & 0xff)) << ((7 - i) * 8);
    }
    return r;
}

void BitSet::reverse()
{
    auto &data = m_value->m_value;

    const auto requiredShiftBits = m_value->m_bitsNumber % 64;

    std::vector<uint64_t> temp;
    temp.resize(data.size());

    if (requiredShiftBits) {
        size_t index = 0;
        for (auto it = data.crbegin(); it != data.crend(); ++it) {
            auto reversedData = reverse64(*it);
            if (index == 0) {
                temp[index++] = reversedData >> (64 - requiredShiftBits);
            } else {
                temp[index - 1] |= reversedData << requiredShiftBits;
                temp[index++] |= reversedData >> (64 - requiredShiftBits);
            }
        }

        data = temp;
        return;
    }

    size_t index = 0;
    for (auto it = data.crbegin(); it != data.crend(); ++it) {
        temp[index++] = reverse64(*it);
    }

    data = temp;
}

const std::string BitSet::toString() const
{
    std::string result;
    result.resize(m_value->m_bitsNumber);

    for (size_t i = 0; i < m_value->m_bitsNumber; ++i) {
        result[i] = test(i) ? '1' : '0';
    }

    return result;
}

} // namespace psi::tools