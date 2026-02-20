#pragma once

#include "psi/tools/ByteBuffer.h"

#include <array>

namespace psi::tools::crypt {

class aes_gcm
{
public:
    using DataBlock16 = std::array<uint8_t, 16>;
    using Tag = DataBlock16;

    static void gfMultBlock(const DataBlock16 &x, const DataBlock16 &y, DataBlock16 &z);
    static void ghashBlock(const DataBlock16 &h, const uint8_t *data, size_t dataLen, DataBlock16 &result);
    static void ghash(const DataBlock16 &h, const uint8_t *acc, size_t accLen, const uint8_t *cipher, size_t cipherLen, DataBlock16 &result);
    static void xorBlocks(const DataBlock16 &a, const DataBlock16 &b, DataBlock16 &result);
    static void xorBlocksInPlace(const uint8_t *src, DataBlock16 &dst);
    static void gfMult(const uint8_t x, const uint8_t y, uint8_t &z);
    static void incr(DataBlock16 &counter);
    static ByteBuffer encrypt(const ByteBuffer &data,
                              const ByteBuffer &key,
                              const ByteBuffer &iv,
                              Tag &tag,
                              const ByteBuffer &acc = {});
    static ByteBuffer encrypt(const ByteBuffer &data, const ByteBuffer &key, const ByteBuffer &iv);
    static ByteBuffer decrypt(const ByteBuffer &encryptedData,
                              const ByteBuffer &key,
                              const ByteBuffer &iv,
                              const ByteBuffer &tag,
                              const ByteBuffer &acc = {});
    static ByteBuffer decrypt(const ByteBuffer &encryptedData, const ByteBuffer &key, const ByteBuffer &tag);

private:
    static const uint8_t R_POLY;
};

} // namespace psi::tools::crypt
