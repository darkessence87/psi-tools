#pragma once

#include "psi/tools/ByteBuffer.h"

#include <array>

namespace psi::tools::crypt {

class aes
{
public:
    using DataWord4 = std::array<uint8_t, 4>;
    using DataBlock16 = std::array<DataWord4, 4>;
    using SubKey = std::array<uint8_t, 16>;
    template <size_t N>
    using SubKeys = std::array<SubKey, N>;

    static const std::array<uint8_t, 256u> m_sBox;
    static const std::array<uint8_t, 256u> m_iBox;
    static const std::array<uint8_t, 10u> m_rCon;

    static void rotWord(DataWord4 &);
    static void subWord(DataWord4 &);
    static void transpose(DataBlock16 &);

    static void subBytes(const std::array<uint8_t, 256u> &box, DataBlock16 &block);
    static void shiftRows(DataBlock16 &block);
    static void invShiftRows(DataBlock16 &block);
    static void mixColumns(DataBlock16 &block);
    static void invMixColumns(DataBlock16 &block);
    static void applySubKey(const SubKey &key, DataBlock16 &block);
    static void writeBlock(const uint8_t *const data, size_t dataSz, DataBlock16 &block);
    static void readBlock(const DataBlock16 &block, uint8_t *data, size_t dataSz);
    static void doRoundKeyEncode(const SubKey &key, DataBlock16 &block, bool isFinal = false);
    static void doRoundKeyDecode(const SubKey &key, DataBlock16 &block, bool isFinal = false);

    template <uint8_t Nk, uint8_t Nr>
    static ByteBuffer encryptAes_impl(const ByteBuffer &data, const ByteBuffer &key);
    template <uint8_t Nk, uint8_t Nr>
    static ByteBuffer encryptAes_impl2(const ByteBuffer &data, const ByteBuffer &key);
    template <uint8_t Nk, uint8_t Nr>
    static ByteBuffer encryptAes_impl(const uint8_t *data, size_t dataLen, const ByteBuffer &key);
    template <uint8_t Nk, uint8_t Nr>
    static ByteBuffer encryptAes_impl2(const uint8_t *data, size_t dataLen, const ByteBuffer &key);

    template <uint8_t Nk, uint8_t Nr>
    static ByteBuffer decryptAes_impl(const ByteBuffer &data, const ByteBuffer &key);
    template <uint8_t Nk, uint8_t Nr>
    static ByteBuffer decryptAes_impl(const uint8_t *data, size_t dataLen, const ByteBuffer &key);

    template <uint8_t Nk, uint8_t Nr>
    static void generateSubKeys_impl(uint8_t key[Nk * 4u], SubKeys<Nr + 1u> &);
};

extern template ByteBuffer aes::encryptAes_impl<4u, 10u>(const ByteBuffer &, const ByteBuffer &);
extern template ByteBuffer aes::encryptAes_impl<4u, 10u>(const uint8_t *, size_t, const ByteBuffer &);
extern template ByteBuffer aes::decryptAes_impl<4u, 10u>(const ByteBuffer &, const ByteBuffer &);
extern template ByteBuffer aes::decryptAes_impl<4u, 10u>(const uint8_t *, size_t, const ByteBuffer &);
extern template void aes::generateSubKeys_impl<4u, 10u>(uint8_t[4u * 4u], aes::SubKeys<10u + 1u> &);

extern template ByteBuffer aes::encryptAes_impl<8u, 14u>(const ByteBuffer &, const ByteBuffer &);
extern template ByteBuffer aes::decryptAes_impl<8u, 14u>(const ByteBuffer &, const ByteBuffer &);
extern template void aes::generateSubKeys_impl<8u, 14u>(uint8_t[8u * 4u], aes::SubKeys<14u + 1u> &);

} // namespace psi::tools::crypt
