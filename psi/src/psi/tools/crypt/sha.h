#pragma once

#include "psi/tools/ByteBuffer.h"

namespace psi::tools::crypt {

class sha
{
public:
    static ByteBuffer padMessage(const ByteBuffer &data);
    static void prepareMessageSchedule(const uint8_t *block, uint32_t *w);
    static uint32_t rightRotate(uint32_t v, uint8_t n);
    static void hashInit(uint32_t h[8u]);
    static ByteBuffer encode256(const ByteBuffer &data);

    static ByteBuffer hmac256(const ByteBuffer &key, const ByteBuffer &data);

    static ByteBuffer hkdf256Extract(const ByteBuffer &kMat, const ByteBuffer &seed);
    static ByteBuffer hkdf256Expand(const ByteBuffer &prk, const ByteBuffer &info, size_t len);
    static ByteBuffer hkdf256(const ByteBuffer &key, const ByteBuffer &seed, const ByteBuffer &info, size_t len);

private:
    static const uint32_t K[64];
    static const uint32_t H[8];
    static const uint8_t INN_PAD[64];
    static const uint8_t OUT_PAD[64];
};

} // namespace psi::tools::crypt
