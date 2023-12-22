/**
 * @brief https://nvlpubs.nist.gov/nistpubs/FIPS/NIST.FIPS.197.pdf
 * 
 */

#include "aes.h"

#ifdef PSI_LOGGER
#include "psi/logger/Logger.h"
#else
#include <iostream>
#include <sstream>
#define LOG_TRACE_STATIC(x)                                                                                            \
    do {                                                                                                               \
        std::ostringstream os;                                                                                         \
        os << x;                                                                                                       \
        std::cout << os.str() << std::endl;                                                                            \
    } while (0)
#define LOG_ERROR_STATIC(x) LOG_TRACE_STATIC(x)
#endif

namespace psi::tools::crypt {

inline void aes::doRoundKeyEncode(const aes::SubKey &key, aes::DataBlock16 &block, bool isFinal)
{
    aes::subBytes(aes::m_sBox, block);
    aes::shiftRows(block);
    if (!isFinal) {
        aes::mixColumns(block);
    }
    aes::applySubKey(key, block);
}

inline void aes::doRoundKeyDecode(const aes::SubKey &key, aes::DataBlock16 &block, bool isFinal)
{
    aes::invShiftRows(block);
    aes::subBytes(aes::m_iBox, block);
    aes::applySubKey(key, block);
    if (!isFinal) {
        aes::invMixColumns(block);
    }
}

template <uint8_t Nk, uint8_t Nr>
ByteBuffer aes::encryptAes_impl(const ByteBuffer &inputData, const ByteBuffer &key)
{
    return encryptAes_impl<Nk, Nr>(inputData.data(), inputData.length(), key);
}

template <uint8_t Nk, uint8_t Nr>
ByteBuffer aes::encryptAes_impl(const uint8_t *m_inData, size_t dataLen, const ByteBuffer &key)
{
    if (key.size() != Nk * 4u) {
        return {};
    }

    key.reset();

    const uint8_t extraBytes = dataLen % 16u;
    ByteBuffer result(extraBytes == 0 ? dataLen : (dataLen + 16u - extraBytes + 1));

    SubKey m_subKeys[Nr + 1u];
    generateSubKeys_impl<Nk, Nr>(key.data(), m_subKeys);

    auto m_outData = result.data();

    const size_t cycles = dataLen / 16u;

    // main cycles
    for (size_t cycleN = 0; cycleN < cycles; ++cycleN) {
        DataBlock16 block;
        writeBlock(&m_inData[cycleN * 16u], 16u, block);
        applySubKey(m_subKeys[0], block);
        for (uint8_t round = 1; round < Nr; ++round) {
            doRoundKeyEncode(m_subKeys[round], block);
        }
        doRoundKeyEncode(m_subKeys[Nr], block, true);
        readBlock(block, &m_outData[cycleN * 16u], 16u);
    }
    // additional cycle
    if (extraBytes) {
        uint8_t lastChunk[16u] = {'\0'};
        memcpy(lastChunk, &m_inData[dataLen - extraBytes], extraBytes);

        DataBlock16 block;
        writeBlock(lastChunk, 16u, block);
        applySubKey(m_subKeys[0], block);
        for (uint8_t round = 1; round < Nr; ++round) {
            doRoundKeyEncode(m_subKeys[round], block);
        }
        doRoundKeyEncode(m_subKeys[Nr], block, true);
        readBlock(block, &m_outData[cycles * 16u], 16u);
        memset(&m_outData[(cycles + 1) * 16u], uint8_t(extraBytes), 1);
    }

    return result;
}

template <uint8_t Nk, uint8_t Nr>
ByteBuffer aes::decryptAes_impl(const ByteBuffer &inputData, const ByteBuffer &key)
{
    if (key.size() != Nk * 4u) {
        return {};
    }

    inputData.reset();
    key.reset();

    // make data copy
    const uint8_t lenOffset = inputData.size() % 16u;
    const uint8_t extraBytes = lenOffset ? inputData.at(inputData.size() - 1) : 0;
    const size_t resultLen = lenOffset ? inputData.size() - 1 - 16u + extraBytes : inputData.size();
    ByteBuffer result(resultLen);

    SubKey m_subKeys[Nr + 1u];
    generateSubKeys_impl<Nk, Nr>(key.data(), m_subKeys);

    const auto &m_inData = inputData.data();
    auto m_outData = result.data();

    const size_t cycles = resultLen / 16u;

    // main cycles
    for (size_t cycleN = 0; cycleN < cycles; ++cycleN) {
        DataBlock16 block;
        writeBlock(&m_inData[cycleN * 16u], 16u, block);
        applySubKey(m_subKeys[Nr], block);
        for (uint8_t round = Nr - 1; round > 0; --round) {
            doRoundKeyDecode(m_subKeys[round], block);
        }
        doRoundKeyDecode(m_subKeys[0], block, true);
        readBlock(block, &m_outData[cycleN * 16u], 16u);
    }
    // additional cycle
    if (extraBytes) {
        uint8_t lastChunk[16u] = {'\0'};
        memcpy(lastChunk, &m_inData[inputData.size() - 1 - 16u], 16u);

        DataBlock16 block;
        writeBlock(lastChunk, 16u, block);
        applySubKey(m_subKeys[Nr], block);
        for (uint8_t round = Nr - 1; round > 0; --round) {
            doRoundKeyDecode(m_subKeys[round], block);
        }
        doRoundKeyDecode(m_subKeys[0], block, true);
        readBlock(block, &m_outData[cycles * 16u], extraBytes);
    }

    return result;
}

template <uint8_t Nk, uint8_t Nr>
void aes::generateSubKeys_impl(uint8_t key[Nk * 4u], SubKey subKeys[Nr + 1u])
{
    // (Nr + 1) * 4 rows, 4 cols
    constexpr uint8_t SUB_WORDS = (Nr + 1) * 4u;
    uint8_t w[SUB_WORDS][4u];
    memcpy(&w[0][0], key, Nk * 4u);

    uint8_t i = Nk;
    uint8_t temp[4u];
    while (i < SUB_WORDS) {
        memcpy(temp, w[i - 1u], 4u);
        if (i % Nk == 0u) {
            rotWord(temp);
            subWord(temp);
            temp[0] ^= m_rCon[i / Nk - 1u];
        } else if constexpr (Nk > 6u) {
            if (i % Nk == 4u) {
                subWord(temp);
            }
        }
        for (uint8_t k = 0; k < 4u; ++k) {
            w[i][k] = w[i - Nk][k] ^ temp[k];
        }
        ++i;
    }

    for (uint8_t k = 0; k < Nr + 1u; ++k) {
        memcpy(&subKeys[k], &w[k * 4u], 16u);
    }
}

inline uint8_t mul2(uint8_t v)
{
    return (v << 1) ^ (0x1b & uint8_t(int8_t(v) >> 7));
}

inline uint8_t mul3(uint8_t a)
{
    return mul2(a) ^ a;
}

inline uint8_t mul9(uint8_t a)
{
    return mul2(mul2(mul2(a))) ^ a;
}

inline uint8_t mul11(uint8_t a)
{
    const auto b = mul2(a);
    return mul2(mul2(b)) ^ b ^ a;
}

inline uint8_t mul13(uint8_t a)
{
    const auto c = mul2(mul2(a));
    return mul2(c) ^ c ^ a;
}

inline uint8_t mul14(uint8_t a)
{
    const auto b = mul2(a);
    const auto c = mul2(b);
    return mul2(c) ^ c ^ b;
}

const uint8_t aes::m_sBox[256u] = {
    //0   1     2     3     4     5     6     7     8     9     a     b     c     d     e     f
    0x63, 0x7c, 0x77, 0x7b, 0xf2, 0x6b, 0x6f, 0xc5, 0x30, 0x01, 0x67, 0x2b, 0xfe, 0xd7, 0xab, 0x76, 0xca, 0x82, 0xc9,
    0x7d, 0xfa, 0x59, 0x47, 0xf0, 0xad, 0xd4, 0xa2, 0xaf, 0x9c, 0xa4, 0x72, 0xc0, 0xb7, 0xfd, 0x93, 0x26, 0x36, 0x3f,
    0xf7, 0xcc, 0x34, 0xa5, 0xe5, 0xf1, 0x71, 0xd8, 0x31, 0x15, 0x04, 0xc7, 0x23, 0xc3, 0x18, 0x96, 0x05, 0x9a, 0x07,
    0x12, 0x80, 0xe2, 0xeb, 0x27, 0xb2, 0x75, 0x09, 0x83, 0x2c, 0x1a, 0x1b, 0x6e, 0x5a, 0xa0, 0x52, 0x3b, 0xd6, 0xb3,
    0x29, 0xe3, 0x2f, 0x84, 0x53, 0xd1, 0x00, 0xed, 0x20, 0xfc, 0xb1, 0x5b, 0x6a, 0xcb, 0xbe, 0x39, 0x4a, 0x4c, 0x58,
    0xcf, 0xd0, 0xef, 0xaa, 0xfb, 0x43, 0x4d, 0x33, 0x85, 0x45, 0xf9, 0x02, 0x7f, 0x50, 0x3c, 0x9f, 0xa8, 0x51, 0xa3,
    0x40, 0x8f, 0x92, 0x9d, 0x38, 0xf5, 0xbc, 0xb6, 0xda, 0x21, 0x10, 0xff, 0xf3, 0xd2, 0xcd, 0x0c, 0x13, 0xec, 0x5f,
    0x97, 0x44, 0x17, 0xc4, 0xa7, 0x7e, 0x3d, 0x64, 0x5d, 0x19, 0x73, 0x60, 0x81, 0x4f, 0xdc, 0x22, 0x2a, 0x90, 0x88,
    0x46, 0xee, 0xb8, 0x14, 0xde, 0x5e, 0x0b, 0xdb, 0xe0, 0x32, 0x3a, 0x0a, 0x49, 0x06, 0x24, 0x5c, 0xc2, 0xd3, 0xac,
    0x62, 0x91, 0x95, 0xe4, 0x79, 0xe7, 0xc8, 0x37, 0x6d, 0x8d, 0xd5, 0x4e, 0xa9, 0x6c, 0x56, 0xf4, 0xea, 0x65, 0x7a,
    0xae, 0x08, 0xba, 0x78, 0x25, 0x2e, 0x1c, 0xa6, 0xb4, 0xc6, 0xe8, 0xdd, 0x74, 0x1f, 0x4b, 0xbd, 0x8b, 0x8a, 0x70,
    0x3e, 0xb5, 0x66, 0x48, 0x03, 0xf6, 0x0e, 0x61, 0x35, 0x57, 0xb9, 0x86, 0xc1, 0x1d, 0x9e, 0xe1, 0xf8, 0x98, 0x11,
    0x69, 0xd9, 0x8e, 0x94, 0x9b, 0x1e, 0x87, 0xe9, 0xce, 0x55, 0x28, 0xdf, 0x8c, 0xa1, 0x89, 0x0d, 0xbf, 0xe6, 0x42,
    0x68, 0x41, 0x99, 0x2d, 0x0f, 0xb0, 0x54, 0xbb, 0x16};

const uint8_t aes::m_iBox[256u] = {
    //0   1     2     3     4     5     6     7     8     9     a     b     c     d     e     f
    0x52, 0x09, 0x6a, 0xd5, 0x30, 0x36, 0xa5, 0x38, 0xbf, 0x40, 0xa3, 0x9e, 0x81, 0xf3, 0xd7, 0xfb, 0x7c, 0xe3, 0x39,
    0x82, 0x9b, 0x2f, 0xff, 0x87, 0x34, 0x8e, 0x43, 0x44, 0xc4, 0xde, 0xe9, 0xcb, 0x54, 0x7b, 0x94, 0x32, 0xa6, 0xc2,
    0x23, 0x3d, 0xee, 0x4c, 0x95, 0x0b, 0x42, 0xfa, 0xc3, 0x4e, 0x08, 0x2e, 0xa1, 0x66, 0x28, 0xd9, 0x24, 0xb2, 0x76,
    0x5b, 0xa2, 0x49, 0x6d, 0x8b, 0xd1, 0x25, 0x72, 0xf8, 0xf6, 0x64, 0x86, 0x68, 0x98, 0x16, 0xd4, 0xa4, 0x5c, 0xcc,
    0x5d, 0x65, 0xb6, 0x92, 0x6c, 0x70, 0x48, 0x50, 0xfd, 0xed, 0xb9, 0xda, 0x5e, 0x15, 0x46, 0x57, 0xa7, 0x8d, 0x9d,
    0x84, 0x90, 0xd8, 0xab, 0x00, 0x8c, 0xbc, 0xd3, 0x0a, 0xf7, 0xe4, 0x58, 0x05, 0xb8, 0xb3, 0x45, 0x06, 0xd0, 0x2c,
    0x1e, 0x8f, 0xca, 0x3f, 0x0f, 0x02, 0xc1, 0xaf, 0xbd, 0x03, 0x01, 0x13, 0x8a, 0x6b, 0x3a, 0x91, 0x11, 0x41, 0x4f,
    0x67, 0xdc, 0xea, 0x97, 0xf2, 0xcf, 0xce, 0xf0, 0xb4, 0xe6, 0x73, 0x96, 0xac, 0x74, 0x22, 0xe7, 0xad, 0x35, 0x85,
    0xe2, 0xf9, 0x37, 0xe8, 0x1c, 0x75, 0xdf, 0x6e, 0x47, 0xf1, 0x1a, 0x71, 0x1d, 0x29, 0xc5, 0x89, 0x6f, 0xb7, 0x62,
    0x0e, 0xaa, 0x18, 0xbe, 0x1b, 0xfc, 0x56, 0x3e, 0x4b, 0xc6, 0xd2, 0x79, 0x20, 0x9a, 0xdb, 0xc0, 0xfe, 0x78, 0xcd,
    0x5a, 0xf4, 0x1f, 0xdd, 0xa8, 0x33, 0x88, 0x07, 0xc7, 0x31, 0xb1, 0x12, 0x10, 0x59, 0x27, 0x80, 0xec, 0x5f, 0x60,
    0x51, 0x7f, 0xa9, 0x19, 0xb5, 0x4a, 0x0d, 0x2d, 0xe5, 0x7a, 0x9f, 0x93, 0xc9, 0x9c, 0xef, 0xa0, 0xe0, 0x3b, 0x4d,
    0xae, 0x2a, 0xf5, 0xb0, 0xc8, 0xeb, 0xbb, 0x3c, 0x83, 0x53, 0x99, 0x61, 0x17, 0x2b, 0x04, 0x7e, 0xba, 0x77, 0xd6,
    0x26, 0xe1, 0x69, 0x14, 0x63, 0x55, 0x21, 0x0c, 0x7d};

const uint8_t aes::m_rCon[10u] = {0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80, 0x1b, 0x36};

void aes::subBytes(const uint8_t box[256u], aes::DataBlock16 &block)
{
    for (size_t k = 0; k < 16; ++k) {
        const size_t r = k / 4;
        const size_t c = k % 4;
        block[r][c] = box[block[r][c]];
    }
}

void aes::shiftRows(aes::DataBlock16 &block)
{
    // shift left 1 byte
    auto r1 = reinterpret_cast<uint32_t *>(&block[1]);
    uint8_t t = block[1][0];
    *r1 >>= 8 * 1;
    block[1][3] = t;
    // swap pairs of bytes
    std::swap(block[2][0], block[2][2]);
    std::swap(block[2][1], block[2][3]);
    // shift right 1 byte
    auto r3 = reinterpret_cast<uint32_t *>(&block[3]);
    t = block[3][3];
    *r3 <<= 8 * 1;
    block[3][0] = t;
}

void aes::invShiftRows(aes::DataBlock16 &block)
{
    // shift right 1 byte
    auto r1 = reinterpret_cast<uint32_t *>(&block[1]);
    uint8_t t = block[1][3];
    *r1 <<= 8 * 1;
    block[1][0] = t;
    // swap pairs of bytes
    std::swap(block[2][0], block[2][2]);
    std::swap(block[2][1], block[2][3]);
    // shift left 1 byte
    auto r3 = reinterpret_cast<uint32_t *>(&block[3]);
    t = block[3][0];
    *r3 >>= 8 * 1;
    block[3][3] = t;
}

void aes::mixColumns(aes::DataBlock16 &block)
{
    for (uint8_t col = 0; col < 4; ++col) {
        /**
        * Matrix GF2
        *  1 = 0x01
        *  2 = 0x10
        *  3 = 0x11
        *  0x10 0x11 0x01 0x01
        *  0x01 0x10 0x11 0x01
        *  0x01 0x01 0x10 0x11
        *  0x11 0x01 0x01 0x10
        **/

        auto a0 = block[0][col];
        auto a1 = block[1][col];
        auto a2 = block[2][col];
        auto a3 = block[3][col];

        block[0][col] = mul2(a0) ^ mul3(a1) ^ a2 ^ a3; // 2 * a0 + 3 * a1 + 1 * a2 + 1 * a3
        block[1][col] = a0 ^ mul2(a1) ^ mul3(a2) ^ a3; // 1 * a0 + 2 * a1 + 3 * a2 + 1 * a3
        block[2][col] = a0 ^ a1 ^ mul2(a2) ^ mul3(a3); // 1 * a0 + 1 * a1 + 2 * a2 + 3 * a3
        block[3][col] = mul3(a0) ^ a1 ^ a2 ^ mul2(a3); // 3 * a0 + 1 * a1 + 1 * a2 + 2 * a3
    }
}

void aes::invMixColumns(aes::DataBlock16 &block)
{
    for (uint8_t col = 0; col < 4; ++col) {
        /**
        * InvMatrix GF2
        *  9  = 0x09 = 2 * 2 * 2 + 1
        *  11 = 0x0b = (2 * 2 + 1) * 2 + 1
        *  13 = 0x0d = (2 + 1) * 2 * 2 + 1
        *  14 = 0x0e = ((2 + 1) * 2 + 1) * 2
        *  0x0e 0x0b 0x0d 0x09 // 14 11 13 9
        *  0x09 0x0e 0x0b 0x0d // 9 14 11 13
        *  0x0d 0x09 0x0e 0x0b // 13 9 14 11
        *  0x0b 0x0d 0x09 0x0e // 11 13 9 14
        **/

        // 14 * a0 + 11 * a1 + 13 * a2 +  9 * a3
        //  9 * a0 + 14 * a1 + 11 * a2 + 13 * a3
        // 13 * a0 +  9 * a1 + 14 * a2 + 11 * a3
        // 11 * a0 + 13 * a1 +  9 * a2 + 14 * a3

        auto a0 = block[0][col];
        auto a1 = block[1][col];
        auto a2 = block[2][col];
        auto a3 = block[3][col];

        block[0][col] = mul14(a0) ^ mul11(a1) ^ mul13(a2) ^ mul9(a3);
        block[1][col] = mul9(a0) ^ mul14(a1) ^ mul11(a2) ^ mul13(a3);
        block[2][col] = mul13(a0) ^ mul9(a1) ^ mul14(a2) ^ mul11(a3);
        block[3][col] = mul11(a0) ^ mul13(a1) ^ mul9(a2) ^ mul14(a3);
    }
}

void aes::applySubKey(const uint8_t key[16u], aes::DataBlock16 &block)
{
    for (size_t k = 0; k < 16; ++k) {
        const size_t r = k / 4;
        const size_t c = k % 4;
        block[r][c] ^= key[c * 4 + r];
    }
}

void aes::writeBlock(const uint8_t *const data, size_t dataSz, aes::DataBlock16 &block)
{
    const size_t sz = dataSz > 16 ? 16 : dataSz;
    // swap rows and columns
    for (size_t k = 0; k < sz; ++k) {
        const size_t r = k / 4;
        const size_t c = k % 4;
        block[c][r] = data[k];
    }
}

void aes::readBlock(const aes::DataBlock16 &block, uint8_t *data, size_t dataSz)
{
    const size_t sz = dataSz > 16 ? 16 : dataSz;
    // swap rows and columns
    for (size_t k = 0; k < sz; ++k) {
        const size_t r = k / 4;
        const size_t c = k % 4;
        data[k] = block[c][r];
    }
}

void aes::rotWord(uint8_t word[4])
{
    uint8_t t = word[0];
    word[0] = word[1];
    word[1] = word[2];
    word[2] = word[3];
    word[3] = t;
}

void aes::subWord(uint8_t word[4])
{
    word[0] = m_sBox[word[0]];
    word[1] = m_sBox[word[1]];
    word[2] = m_sBox[word[2]];
    word[3] = m_sBox[word[3]];
};

} // namespace psi::tools::crypt
