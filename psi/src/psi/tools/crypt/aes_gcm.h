#pragma once

#include "psi/tools/ByteBuffer.h"

#include <bitset>

namespace psi::tools::crypt {

class aes_gcm
{
public:
    using DataBlock16 = uint8_t[16];
    using Tag = uint8_t[16];

    static void gfMultBlock(const uint8_t *x, const uint8_t *y, uint8_t *z);
    static void ghashBlock(const uint8_t *h, const uint8_t *data, size_t dataLen, uint8_t *result);
    static void ghash(const uint8_t *h, const uint8_t *acc, size_t accLen, const uint8_t *cipher, size_t cipherLen, uint8_t *result);
    static void xorBlocks(const uint8_t *a, const uint8_t *b, uint8_t *result);
    static void gfMult(const uint8_t x, const uint8_t y, uint8_t &z);
    static void incr(uint8_t *counter);
    static ByteBuffer encrypt(const ByteBuffer &data,
                              const ByteBuffer &key,
                              const ByteBuffer &iv,
                              Tag &tag,
                              const ByteBuffer &acc = ByteBuffer(0));

private:
    static const uint8_t R_POLY;
};

} // namespace psi::tools::crypt
