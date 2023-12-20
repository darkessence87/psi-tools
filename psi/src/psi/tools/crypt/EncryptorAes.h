#pragma once

#include "psi/tools/ByteBuffer.h"

namespace psi::tools::crypt {

class EncryptorAes
{
public:
    using DataBlock16 = uint8_t[4u][4u];
    using SubKey = uint8_t[16u];
    using SubKey128 = uint32_t;
    using SubKeys128 = SubKey[11u];
    using SubKeys256 = SubKey[15u];

    static const uint8_t m_sBox[256u];
    static const uint8_t m_iBox[256u];
    static const uint8_t m_rCon[10u];

    static void rotWord(uint8_t[4]);
    static void subWord(uint8_t[4]);

    static void subBytes(const uint8_t box[256u], DataBlock16 &block);
    static void shiftRows(DataBlock16 &block);
    static void invShiftRows(DataBlock16 &block);
    static void mixColumns(DataBlock16 &block);
    static void invMixColumns(DataBlock16 &block);
    static void applySubKey(const uint8_t key[16u], DataBlock16 &block);
    static void writeBlock(const uint8_t *const data, size_t dataSz, DataBlock16 &block);
    static void readBlock(const DataBlock16 &block, uint8_t *data, size_t dataSz);

    template <uint8_t Nk, uint8_t Nr>
    static ByteBuffer encryptAes_impl(const ByteBuffer &, const ByteBuffer &);

    template <uint8_t Nk, uint8_t Nr>
    static ByteBuffer decryptAes_impl(const ByteBuffer &, const ByteBuffer &);

    template <uint8_t Nk, uint8_t Nr>
    static void generateSubKeys_impl(uint8_t[Nk * 4u], SubKey[Nr + 1u]);
};

} // namespace psi::tools::crypt
