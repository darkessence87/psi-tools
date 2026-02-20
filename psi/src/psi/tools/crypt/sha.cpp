/**
 * https://csrc.nist.gov/files/pubs/fips/180-4/upd1/final/docs/fips180-4-draft-aug2014.pdf
 * https://datatracker.ietf.org/doc/html/rfc4231#page-4
 * 
 */
#include "sha.h"

namespace psi::tools::crypt {

const uint32_t sha::K[64] = {0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1, 0x923f82a4,
                             0xab1c5ed5, 0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3, 0x72be5d74, 0x80deb1fe,
                             0x9bdc06a7, 0xc19bf174, 0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc, 0x2de92c6f,
                             0x4a7484aa, 0x5cb0a9dc, 0x76f988da, 0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7,
                             0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967, 0x27b70a85, 0x2e1b2138, 0x4d2c6dfc,
                             0x53380d13, 0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85, 0xa2bfe8a1, 0xa81a664b,
                             0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070, 0x19a4c116,
                             0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
                             0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208, 0x90befffa, 0xa4506ceb, 0xbef9a3f7,
                             0xc67178f2};
const uint32_t sha::H[8] = {0x6a09e667, 0xbb67ae85, 0x3c6ef372, 0xa54ff53a, 0x510e527f, 0x9b05688c, 0x1f83d9ab, 0x5be0cd19};
const uint8_t sha::INN_PAD[64] = {0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36,
                                  0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36,
                                  0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36,
                                  0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36,
                                  0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36};
const uint8_t sha::OUT_PAD[64] = {0x5c, 0x5c, 0x5c, 0x5c, 0x5c, 0x5c, 0x5c, 0x5c, 0x5c, 0x5c, 0x5c, 0x5c, 0x5c,
                                  0x5c, 0x5c, 0x5c, 0x5c, 0x5c, 0x5c, 0x5c, 0x5c, 0x5c, 0x5c, 0x5c, 0x5c, 0x5c,
                                  0x5c, 0x5c, 0x5c, 0x5c, 0x5c, 0x5c, 0x5c, 0x5c, 0x5c, 0x5c, 0x5c, 0x5c, 0x5c,
                                  0x5c, 0x5c, 0x5c, 0x5c, 0x5c, 0x5c, 0x5c, 0x5c, 0x5c, 0x5c, 0x5c, 0x5c, 0x5c,
                                  0x5c, 0x5c, 0x5c, 0x5c, 0x5c, 0x5c, 0x5c, 0x5c, 0x5c, 0x5c, 0x5c, 0x5c};

ByteBuffer sha::padMessage(const ByteBuffer &data)
{
    const size_t requiredBytes = 64u - (data.size() % 64u);
    ByteBuffer result(data.size() + requiredBytes);
    result.writeArray(data.data(), data.size());
    result.write(uint8_t(0x80));
    result.skipWrite(requiredBytes - 1u - 8u);
    result.writeSwapped(uint64_t(data.size() << 3));
    return result;
}

void sha::prepareMessageSchedule(const uint8_t *block, uint32_t *w)
{
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunsafe-buffer-usage"
    for (uint8_t i = 0; i < 16u; ++i) {
        uint32_t a = uint32_t(*block++) << 24;
        uint32_t b = uint32_t(*block++) << 16;
        uint32_t c = uint32_t(*block++) << 8;
        uint32_t d = uint32_t(*block++);
        w[i] = a | b | c | d;
    }

    for (uint8_t i = 16; i < 64; ++i) {
        uint32_t s0 = rightRotate(w[i - 15], 7) ^ rightRotate(w[i - 15], 18) ^ (w[i - 15] >> 3);
        uint32_t s1 = rightRotate(w[i - 2], 17) ^ rightRotate(w[i - 2], 19) ^ (w[i - 2] >> 10);
        w[i] = w[i - 16] + s0 + w[i - 7] + s1;
    }
#pragma clang diagnostic pop
}

uint32_t sha::rightRotate(uint32_t v, uint8_t n)
{
    n %= 32u;
    if (n == 0) {
        return v;
    }
    return (v << (32u - n)) | (v >> n);
}

void sha::hashInit(uint32_t h[8u])
{
    mem_copy(&h[0], 0, &H[0], 0, sizeof(H));
}

ByteBuffer sha::encode256(const ByteBuffer &data)
{
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunsafe-buffer-usage"
    auto paddedData = padMessage(data);

    uint8_t dataBlock[64] = {};
    uint32_t hash[8] = {};
    hashInit(hash);
    uint32_t h[8] = {};
    hashInit(h);

    while (paddedData.read(dataBlock)) {
        uint32_t w[64] = {};
        prepareMessageSchedule(dataBlock, w);

        for (uint8_t i = 0; i < 64u; ++i) {
            uint32_t S1 = rightRotate(h[4], 6) ^ rightRotate(h[4], 11) ^ rightRotate(h[4], 25);
            uint32_t ch = (h[4] & h[5]) ^ (~h[4] & h[6]);
            uint32_t tmp1 = h[7] + S1 + ch + K[i] + w[i];
            uint32_t S0 = rightRotate(h[0], 2) ^ rightRotate(h[0], 13) ^ rightRotate(h[0], 22);
            uint32_t maj = (h[0] & h[1]) ^ (h[0] & h[2]) ^ (h[1] & h[2]);
            uint32_t tmp2 = S0 + maj;

            h[7] = h[6];
            h[6] = h[5];
            h[5] = h[4];
            h[4] = h[3] + tmp1;
            h[3] = h[2];
            h[2] = h[1];
            h[1] = h[0];
            h[0] = tmp1 + tmp2;
        }

        for (uint8_t i = 0; i < 8; ++i) {
            hash[i] += h[i];
            h[i] = hash[i];
        }
    }

    ByteBuffer out(32u);
    for (uint8_t i = 0; i < 8; ++i) {
        out.writeSwapped(hash[i]);
    }

#pragma clang diagnostic pop
    return out;
}

ByteBuffer sha::hmac256(const ByteBuffer &key, const ByteBuffer &data)
{
    key.resetRead();
    data.resetRead();

    ByteBuffer paddedKey(64u);
    if (key.size() > 64u) {
        encode256(key).readToByteBuffer(paddedKey, 32u);
    } else {
        key.readToByteBuffer(paddedKey, key.size());
    }

    ByteBuffer iKeyPad(INN_PAD, 64u);
    ByteBuffer oKeyPad(OUT_PAD, 64u);
    for (uint8_t i = 0; i < 64u; ++i) {
        iKeyPad[i] ^= paddedKey.at(i);
        oKeyPad[i] ^= paddedKey.at(i);
    }

    return encode256(oKeyPad + encode256(iKeyPad + data));
}

ByteBuffer sha::hkdf256Extract(const ByteBuffer &kMat, const ByteBuffer &seed)
{
    return sha::hmac256(seed, kMat);
};

ByteBuffer sha::hkdf256Expand(const ByteBuffer &prk, const ByteBuffer &info, size_t len)
{
    const auto blocks = len / 32u + 1;
    ByteBuffer okm(blocks * 32u);
    ByteBuffer temp(32u + info.size() + 1u);

    uint8_t i = 1;
    while (okm.length() < len) {
        prk.resetRead();
        info.resetRead();
        info.readToByteBuffer(temp, info.size());
        temp.write(i);

        temp.resetRead();
        auto t = hmac256(prk, temp.readToByteBuffer(temp.length()));
        t.readToByteBuffer(okm, t.size());

        temp.clear();
        t.resetRead();
        t.readToByteBuffer(temp, t.size());
        ++i;
    }

    return okm.readToByteBuffer(len);
};

ByteBuffer sha::hkdf256(const ByteBuffer &key, const ByteBuffer &seed, const ByteBuffer &info, size_t len)
{
    auto prk = hkdf256Extract(key, seed);
    return hkdf256Expand(prk, info, len);
};

} // namespace psi::tools::crypt
