#include <chrono>

#include "psi/tools/Encryptor.h"

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

namespace psi::tools {

const uint8_t Encryptor::CONST_BLOCK_LEN_BYTES = 16u;
const uint8_t Encryptor::CONST_KEY_LEN_BYTES = 32u;
const uint8_t Encryptor::CONST_ROUNDS_NUMBER = 14u;

const uint8_t Encryptor::m_sBox[256u] = {
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

const uint8_t Encryptor::m_iBox[256u] = {
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

const uint8_t Encryptor::m_rCon[10u] = {0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80, 0x1b, 0x36};

const uint8_t Encryptor::m_base64Table[] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M',
                                            'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',
                                            'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm',
                                            'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z',
                                            '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '+', '/'};

const uint8_t Encryptor::m_base64Pad = '=';

ByteBuffer Encryptor::encryptBase64(const ByteBuffer &inputBuffer)
{
    const size_t outSize = ((inputBuffer.size() / 3) + (inputBuffer.size() % 3 > 0)) * 4;
    ByteBuffer encodedString(outSize);

    uint8_t temp1, temp2, temp3;
    uint32_t temp;
    for (size_t idx = 0; idx < inputBuffer.size() / 3; ++idx) {
        if (!inputBuffer.read(temp1)) {
            return ByteBuffer(0u);
        }
        if (!inputBuffer.read(temp2)) {
            return ByteBuffer(0u);
        }
        if (!inputBuffer.read(temp3)) {
            return ByteBuffer(0u);
        }
        temp = (temp1 << 16) | (temp2 << 8) | temp3;

        if (!encodedString.write(m_base64Table[(temp & 0x00FC0000) >> 18])) {
            return ByteBuffer(0u);
        }
        if (!encodedString.write(m_base64Table[(temp & 0x0003F000) >> 12])) {
            return ByteBuffer(0u);
        }
        if (!encodedString.write(m_base64Table[(temp & 0x00000FC0) >> 6])) {
            return ByteBuffer(0u);
        }
        if (!encodedString.write(m_base64Table[(temp & 0x0000003F)])) {
            return ByteBuffer(0u);
        }
    }

    switch (inputBuffer.size() % 3) {
    case 1:
        if (!inputBuffer.read(temp1)) {
            return ByteBuffer(0u);
        }
        temp = temp1 << 16;

        if (!encodedString.write(m_base64Table[(temp & 0x00FC0000) >> 18])) {
            return ByteBuffer(0u);
        }
        if (!encodedString.write(m_base64Table[(temp & 0x0003F000) >> 12])) {
            return ByteBuffer(0u);
        }
        if (!encodedString.write(m_base64Pad)) {
            return ByteBuffer(0u);
        }
        if (!encodedString.write(m_base64Pad)) {
            return ByteBuffer(0u);
        }
        break;
    case 2:
        if (!inputBuffer.read(temp1)) {
            return ByteBuffer(0u);
        }
        if (!inputBuffer.read(temp2)) {
            return ByteBuffer(0u);
        }
        temp = (temp1 << 16) | (temp2 << 8);

        if (!encodedString.write(m_base64Table[(temp & 0x00FC0000) >> 18])) {
            return ByteBuffer(0u);
        }
        if (!encodedString.write(m_base64Table[(temp & 0x0003F000) >> 12])) {
            return ByteBuffer(0u);
        }
        if (!encodedString.write(m_base64Table[(temp & 0x00000FC0) >> 6])) {
            return ByteBuffer(0u);
        }
        if (!encodedString.write(m_base64Pad)) {
            return ByteBuffer(0u);
        }
        break;
    }
    return encodedString;
}

ByteBuffer Encryptor::decryptBase64(const ByteBuffer &inputBuffer)
{
    const size_t inputLen = inputBuffer.size();
    if (inputLen % 4 || inputBuffer.size() == 0) {
        LOG_ERROR_STATIC("Incorrect incoming buffer length: " << inputLen << " for data:" << inputBuffer.asString());
        return ByteBuffer(0u);
    }

    size_t padding = 0;
    if (m_base64Pad == inputBuffer.at(inputLen - 1))
        ++padding;
    if (m_base64Pad == inputBuffer.at(inputLen - 2))
        ++padding;

    const size_t outSize = ((inputLen / 4) * 3) - padding;
    ByteBuffer decodedString(outSize);

    uint8_t tempByte;
    uint32_t temp = 0;
    size_t idx = 0;

    while (idx != inputLen) {
        for (uint8_t q = 0; q < 4; ++q) {
            if (!inputBuffer.read(tempByte)) {
                return ByteBuffer(0u);
            }

            temp <<= 6;

            if (tempByte >= 0x41 && tempByte <= 0x5A)
                temp |= tempByte - 0x41;
            else if (tempByte >= 0x61 && tempByte <= 0x7A)
                temp |= tempByte - 0x47;
            else if (tempByte >= 0x30 && tempByte <= 0x39)
                temp |= tempByte + 0x04;
            else if (tempByte == 0x2B)
                temp |= 0x3E;
            else if (tempByte == 0x2F)
                temp |= 0x3F;
            else if (tempByte == m_base64Pad) {
                switch (inputLen - idx) {
                case 1:
                    if (!decodedString.write(uint8_t((temp >> 16) & 0x000000FF))) {
                        return ByteBuffer(0u);
                    }
                    if (!decodedString.write(uint8_t((temp >> 8) & 0x000000FF))) {
                        return ByteBuffer(0u);
                    }
                    return decodedString;
                case 2:
                    if (!decodedString.write(uint8_t((temp >> 10) & 0x000000FF))) {
                        return ByteBuffer(0u);
                    }
                    return decodedString;
                default:
                    LOG_ERROR_STATIC("Invalid padding in Base64!");
                    return ByteBuffer(0u);
                }
            } else {
                LOG_ERROR_STATIC("Non-valid character in Base64!");
                return ByteBuffer(0u);
            }

            ++idx;
        }

        if (!decodedString.write(uint8_t((temp >> 16) & 0x000000FF))) {
            return ByteBuffer(0u);
        }
        if (!decodedString.write(uint8_t((temp >> 8) & 0x000000FF))) {
            return ByteBuffer(0u);
        }
        if (!decodedString.write(uint8_t((temp)&0x000000FF))) {
            return ByteBuffer(0u);
        }
    }

    return decodedString;
}

void Encryptor::subBytes(const uint8_t box[256u], Encryptor::DataBlock16 &block)
{
    for (size_t k = 0; k < 16; ++k) {
        const size_t r = k / 4;
        const size_t c = k % 4;
        block[r][c] = box[block[r][c]];
    }
}

void Encryptor::shiftRows(Encryptor::DataBlock16 &block)
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

void Encryptor::invShiftRows(Encryptor::DataBlock16 &block)
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

void Encryptor::mixColumns(Encryptor::DataBlock16 &block)
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

void Encryptor::invMixColumns(Encryptor::DataBlock16 &block)
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

void Encryptor::applySubKey(const uint8_t key[16u], Encryptor::DataBlock16 &block)
{
    for (size_t k = 0; k < 16; ++k) {
        const size_t r = k / 4;
        const size_t c = k % 4;
        block[r][c] ^= key[c * 4 + r];
    }
}

void Encryptor::writeBlock(const uint8_t *const data, size_t dataSz, Encryptor::DataBlock16 &block)
{
    const size_t sz = dataSz > 16 ? 16 : dataSz;
    // swap rows and columns
    for (size_t k = 0; k < sz; ++k) {
        const size_t r = k / 4;
        const size_t c = k % 4;
        block[c][r] = data[k];
    }
}

void Encryptor::readBlock(const Encryptor::DataBlock16 &block, uint8_t *data, size_t dataSz)
{
    const size_t sz = dataSz > 16 ? 16 : dataSz;
    // swap rows and columns
    for (size_t k = 0; k < sz; ++k) {
        const size_t r = k / 4;
        const size_t c = k % 4;
        data[k] = block[c][r];
    }
}

void Encryptor::doRoundKeyEncode(const SubKey256 &key, DataBlock16 &block, bool isFinal)
{
    subBytes(m_sBox, block);
    shiftRows(block);
    if (!isFinal) {
        mixColumns(block);
    }
    applySubKey(key, block);
}

ByteBuffer Encryptor::encryptAes256(const ByteBuffer &inputData, const ByteBuffer &key)
{
    if (key.size() != 32u) {
        return {};
    }

    inputData.reset();
    key.reset();

    const uint8_t extraBytes = inputData.size() % 16u;
    ByteBuffer result(extraBytes == 0 ? inputData.size() : (inputData.size() + 16u - extraBytes + 1));

    SubKeys256 m_subKeys;
    generateSubKeys(key.data(), m_subKeys);

    const auto &m_inData = inputData.data();
    auto m_outData = result.data();

    const size_t cycles = inputData.size() / 16u;

    // main cycles
    for (size_t cycleN = 0; cycleN < cycles; ++cycleN) {
        DataBlock16 block;
        writeBlock(&m_inData[cycleN * 16u], 16u, block);
        applySubKey(m_subKeys[0], block);
        for (uint8_t round = 1; round < CONST_ROUNDS_NUMBER; ++round) {
            doRoundKeyEncode(m_subKeys[round], block);
        }
        doRoundKeyEncode(m_subKeys[CONST_ROUNDS_NUMBER], block, true);
        readBlock(block, &m_outData[cycleN * 16u], 16u);
    }
    // additional cycle
    if (extraBytes) {
        uint8_t lastChunk[16u] = {'\0'};
        memcpy(lastChunk, &m_inData[inputData.size() - extraBytes], extraBytes);

        DataBlock16 block;
        writeBlock(lastChunk, 16u, block);
        applySubKey(m_subKeys[0], block);
        for (uint8_t round = 1; round < CONST_ROUNDS_NUMBER; ++round) {
            doRoundKeyEncode(m_subKeys[round], block);
        }
        doRoundKeyEncode(m_subKeys[CONST_ROUNDS_NUMBER], block, true);
        readBlock(block, &m_outData[cycles * 16u], 16u);
        memset(&m_outData[(cycles + 1) * 16u], uint8_t(extraBytes), 1);
    }

    return result;
}

void Encryptor::doRoundKeyDecode(const SubKey256 &key, DataBlock16 &block, bool isFinal)
{
    invShiftRows(block);
    subBytes(m_iBox, block);
    applySubKey(key, block);
    if (!isFinal) {
        invMixColumns(block);
    }
}

ByteBuffer Encryptor::decryptAes256(const ByteBuffer &inputData, const ByteBuffer &key)
{
    if (key.size() != 32u) {
        return {};
    }

    inputData.reset();
    key.reset();

    // make data copy
    const uint8_t lenOffset = inputData.size() % 16u;
    const uint8_t extraBytes = lenOffset ? inputData.at(inputData.size() - 1) : 0;
    const size_t resultLen = lenOffset ? inputData.size() - 1 - 16u + extraBytes : inputData.size();
    ByteBuffer result(resultLen);

    SubKeys256 m_subKeys;
    generateSubKeys(key.data(), m_subKeys);

    const auto &m_inData = inputData.data();
    auto m_outData = result.data();

    const size_t cycles = resultLen / 16u;

    // main cycles
    for (size_t cycleN = 0; cycleN < cycles; ++cycleN) {
        DataBlock16 block;
        writeBlock(&m_inData[cycleN * 16u], 16u, block);
        applySubKey(m_subKeys[CONST_ROUNDS_NUMBER], block);
        for (uint8_t round = CONST_ROUNDS_NUMBER - 1; round > 0; --round) {
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
        applySubKey(m_subKeys[CONST_ROUNDS_NUMBER], block);
        for (uint8_t round = CONST_ROUNDS_NUMBER - 1; round > 0; --round) {
            doRoundKeyDecode(m_subKeys[round], block);
        }
        doRoundKeyDecode(m_subKeys[0], block, true);
        readBlock(block, &m_outData[cycles * 16u], extraBytes);
    }

    return result;
}

void Encryptor::scheduleKey(uint8_t word[4], const uint8_t rConIndex)
{
    rotWord(word);
    subWord(word);
    word[0] ^= m_rCon[rConIndex];
}

void Encryptor::rotWord(uint8_t word[4])
{
    uint8_t t = word[0];
    word[0] = word[1];
    word[1] = word[2];
    word[2] = word[3];
    word[3] = t;
}

void Encryptor::subWord(uint8_t word[4])
{
    word[0] = m_sBox[word[0]];
    word[1] = m_sBox[word[1]];
    word[2] = m_sBox[word[2]];
    word[3] = m_sBox[word[3]];
};

void Encryptor::generateSubKeys(uint8_t key[32], SubKeys256 &subKeys)
{
    for (uint8_t i = 0; i < 2; ++i) {
        for (uint8_t j = 0; j < 16; ++j) {
            subKeys[i][j] = key[i * 16 + j];
        }
    }

    // rounds
    uint8_t rConIteration = 0u;
    uint8_t currIndex = 0;

    const uint8_t maxLen = CONST_ROUNDS_NUMBER * 16u;

    // setup last 4 bytes
    uint8_t prevWord[4u];
    memcpy(prevWord, &subKeys[1][12], 4u);

    while (currIndex < maxLen) {
        // pick and modify previous word
        scheduleKey(prevWord, rConIteration);
        ++rConIteration;

        const auto i = currIndex / 16u;
        const auto j = currIndex % 16u;

        for (uint8_t idx = 0; idx <= 1; ++idx) {
            const auto ii = i + 2 + idx;
            if (ii > CONST_ROUNDS_NUMBER) {
                break;
            }

            if (idx > 0) {
                subWord(prevWord);
            }

            // new 4 bytes (total: 4)
            for (uint8_t k = 0; k < 4; ++k) {
                const auto jj = j + k;
                subKeys[ii][jj] = subKeys[i + idx][jj] ^ prevWord[k];
                prevWord[k] = subKeys[ii][jj];
            }

            // new 12 bytes (total: 16)
            for (uint8_t k = 1; k < 4; ++k) {
                for (uint8_t z = 0; z < 4; ++z) {
                    const auto jj = j + k * 4 + z;
                    subKeys[ii][jj] = subKeys[i + idx][jj] ^ prevWord[z];
                    prevWord[z] = subKeys[ii][jj];
                }
            }
        }

        currIndex += 32;
    }
}

ByteBuffer Encryptor::generateSessionKey()
{
    using namespace std::chrono;

    auto timePoint = high_resolution_clock::now();
    auto inMicroSec = duration_cast<microseconds>(timePoint.time_since_epoch());
    uint32_t rand1 = static_cast<uint32_t>(inMicroSec.count());
    srand(rand1);
    uint16_t rand2 = static_cast<uint16_t>(rand() % 0xffff);
    uint16_t rand3 = static_cast<uint16_t>(rand() % 0xffff);
    uint16_t rand4 = static_cast<uint16_t>(rand() % 0xffff);
    uint16_t rand5 = static_cast<uint16_t>(rand() % 0xffff);
    uint16_t rand6 = static_cast<uint16_t>(rand() % 0xffff);
    uint16_t rand7 = static_cast<uint16_t>(rand() % 0xffff);
    uint16_t rand8 = static_cast<uint16_t>(rand() % 0xffff);
    uint16_t rand9 = static_cast<uint16_t>(rand() % 0xffff);
    uint16_t randa = static_cast<uint16_t>(rand() % 0xffff);
    uint16_t randb = static_cast<uint16_t>(rand() % 0xffff);
    uint16_t randc = static_cast<uint16_t>(rand() % 0xffff);
    uint16_t randd = static_cast<uint16_t>(rand() % 0xffff);
    uint16_t rande = static_cast<uint16_t>(rand() % 0xffff);
    uint16_t randf = static_cast<uint16_t>(rand() % 0xffff);
    uint16_t rand0 = static_cast<uint16_t>(rand() % 0xffff);

    ByteBuffer sessionKey(32u);
    sessionKey.write(rand1);
    sessionKey.write(rand2);
    sessionKey.write(rand3);
    sessionKey.write(rand4);
    sessionKey.write(rand5);
    sessionKey.write(rand6);
    sessionKey.write(rand7);
    sessionKey.write(rand8);
    sessionKey.write(rand9);
    sessionKey.write(randa);
    sessionKey.write(randb);
    sessionKey.write(randc);
    sessionKey.write(randd);
    sessionKey.write(rande);
    sessionKey.write(randf);
    sessionKey.write(rand0);

    // LOG_TRACE_STATIC("sessionKey: " << sessionKey.asHexString());

    return sessionKey;
}

} // namespace psi::tools