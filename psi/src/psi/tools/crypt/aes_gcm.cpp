/**
 * @brief https://luca-giuzzi.unibs.it/corsi/Support/papers-cryptography/gcm-spec.pdf
 * 
 */
#include "aes_gcm.h"
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

inline void rightshiftBlock(uint8_t *a)
{
    for (int8_t k = 15; k > 0; --k) {
        a[k] = (a[k] >> 1) | (a[k - 1] << 7);
    }
    a[0] >>= 1;
}

void aes_gcm::xorBlocks(const uint8_t *a, const uint8_t *b, uint8_t *result)
{
    for (size_t i = 0; i < 16u; ++i) {
        result[i] = a[i] ^ b[i];
    }
}

void aes_gcm::incr(uint8_t *counter)
{
    for (int8_t i = 15; i >= 0; --i) {
        if (++counter[i] != 0) {
            break;
        }
    }
}

/*
    Z ← 0, V ← X
    for i = 0 to 127 do
        if Yi = 1 then
            Z ← Z ⊕ V
        end if
        if V127 = 0 then
            V ← rightshift(V )
        else
            V ← rightshift(V ) ⊕ R
        end if
    end for
    return Z
*/
const uint8_t aes_gcm::R_POLY = 0xe1;
void aes_gcm::gfMultBlock(const uint8_t *x, const uint8_t *y, uint8_t *z)
{
    DataBlock16 p = {};
    uint8_t v[16u];
    memcpy(v, x, 16u);

    for (int i = 0; i < 16; ++i) {
        for (int j = 0; j < 8; ++j) {
            if (y[i] & (1 << (7 - j))) {
                for (int k = 0; k < 16; ++k) {
                    p[k] ^= v[k];
                }
            }

            if (v[15] & 0x01) {
                rightshiftBlock(v);
                v[0] ^= R_POLY;
            } else {
                rightshiftBlock(v);
            }
        }
    }

    memcpy(z, p, 16);
}

// GHASH(H,A,C) = X[m+n+1]
// X[i] = 0                                 // for i = 0
// (X[i-1] XOR A[i]) mul H                  // for i = 1, ..., m - 1
// (X[m-1] XOR (A*[m] || 0^128-v)) mul H    // for i = m
// (X[i-1] XOR C[i]) mul H                  // for i = m + 1, ..., m + n - 1
// (X[m+n-1] XOR (C*[m] || 0^128-u)) mul H  // for i = m + n
// (X[m+n] XOR (len(A) || len(C))) mul H    // for i = m + n + 1
void aes_gcm::ghashBlock(const uint8_t *h, const uint8_t *data, size_t dataLen, uint8_t *result)
{
    DataBlock16 temp = {};
    memcpy(temp, result, 16);

    const size_t m = dataLen / 16u;
    for (size_t i = 0; i < m; ++i) {
        xorBlocks(temp, data + i * 16u, temp);
        gfMultBlock(temp, h, temp);
    }

    if (auto extra = dataLen % 16u) {
        DataBlock16 tempExtra = {};
        memcpy(tempExtra, data + m * 16u, extra);

        xorBlocks(tempExtra, temp, temp);
        gfMultBlock(temp, h, temp);
    }

    memcpy(result, temp, 16);
}

void aes_gcm::ghash(const uint8_t *h, const uint8_t *acc, size_t accLen, const uint8_t *cipher, size_t cipherLen, uint8_t *result)
{
    memset(result, 0, 16);

    DataBlock16 hashBlock = {};

    ghashBlock(h, acc, accLen, hashBlock);
    ghashBlock(h, cipher, cipherLen, hashBlock);

    DataBlock16 lengthBlock = {};
    accLen *= 8u;
    lengthBlock[4] = uint8_t(accLen >> 24);
    lengthBlock[5] = uint8_t(accLen >> 16);
    lengthBlock[6] = uint8_t(accLen >> 8);
    lengthBlock[7] = uint8_t(accLen);
    cipherLen *= 8u;
    lengthBlock[12] = uint8_t(cipherLen >> 24);
    lengthBlock[13] = uint8_t(cipherLen >> 16);
    lengthBlock[14] = uint8_t(cipherLen >> 8);
    lengthBlock[15] = uint8_t(cipherLen);
    xorBlocks(hashBlock, lengthBlock, hashBlock);
    gfMultBlock(hashBlock, h, result);
}

ByteBuffer aes_gcm::encrypt(const ByteBuffer &data, const ByteBuffer &key, const ByteBuffer &iv, Tag &tag, const ByteBuffer &acc)
{
    ByteBuffer out(data.size());

    // H: E(K, 0^128);
    DataBlock16 h = {};
    aes::encryptAes_impl<4, 10>(DataBlock16 {}, 16u, key).read(h);

    // Y[0]: IV || 0^31;            // if len(IV) = 96 bits
    // Y[0]: GHASH(H,{},IV);        // otherwise
    DataBlock16 counter = {};
    if (iv.length() == 12u) {
        iv.readBytes(counter, 12u);
        counter[15] = 0x01;
    } else {
        ghash(h, DataBlock16 {}, 0, iv.length() ? iv.data() : DataBlock16 {}, iv.length(), counter);
    }

    DataBlock16 y0_encrypted = {};
    aes::encryptAes_impl<4, 10>(counter, 16u, key).read(y0_encrypted);

    // Y[i]: incr(Y[i-1])           // for i = 1, ..., n - 1
    // C[i]: P[i] XOR E(K, Y[i])    // for i = 1, ..., n - 1
    const size_t n = data.length() / 16u;
    for (size_t i = 0; i < n; ++i) {
        incr(counter);
        // printBlockHex(counter, "y[i]: ");

        DataBlock16 cipherBlock = {};
        DataBlock16 dataBlock = {};
        data.read(dataBlock);
        aes::encryptAes_impl<4, 10>(counter, 16, key).read(cipherBlock);
        xorBlocks(dataBlock, cipherBlock, cipherBlock);

        out.write(cipherBlock);
    }

    if (auto extra = data.length() % 16u) {
        incr(counter);

        DataBlock16 cipherBlock = {};
        DataBlock16 dataBlock = {};
        data.readBytes(dataBlock, extra);
        aes::encryptAes_impl<4, 10>(counter, 16, key).read(cipherBlock);
        xorBlocks(dataBlock, cipherBlock, cipherBlock);

        out.writeArray(cipherBlock, extra);
    }

    // C*[n]: P*[n] XOR MSB[u](E(K, Y[n]))      // u - number of bits in final block
    // T: MSB[t](GHASH(H,A,C) XOR E(K, Y[0]))
    ghash(h,
          acc.length() ? acc.data() : DataBlock16 {},
          acc.length(),
          out.length() ? out.data() : DataBlock16 {},
          out.length(),
          tag);
    xorBlocks(tag, y0_encrypted, tag);

    return out;
}

ByteBuffer aes_gcm::encrypt(const ByteBuffer &data, const ByteBuffer &key, const ByteBuffer &iv)
{
    Tag t = {};
    return encrypt(data, key, iv, t);
}

ByteBuffer aes_gcm::decrypt(const ByteBuffer &data,
                            const ByteBuffer &key,
                            const ByteBuffer &iv,
                            const ByteBuffer &tag,
                            const ByteBuffer &acc)
{
    ByteBuffer out(data.size());

    // H: E(K, 0^128);
    DataBlock16 h = {};
    aes::encryptAes_impl<4, 10>(DataBlock16 {}, 16u, key).read(h);

    // Y[0]: IV || 0^31;            // if len(IV) = 96 bits
    // Y[0]: GHASH(H,{},IV);        // otherwise
    DataBlock16 counter = {};
    if (iv.length() == 12u) {
        iv.readBytes(counter, 12u);
        counter[15] = 0x01;
    } else {
        ghash(h, DataBlock16 {}, 0, iv.length() ? iv.data() : DataBlock16 {}, iv.length(), counter);
    }

    DataBlock16 y0_encrypted = {};
    aes::encryptAes_impl<4, 10>(counter, 16u, key).read(y0_encrypted);

    // C*[n]: P*[n] XOR MSB[u](E(K, Y[n]))      // u - number of bits in final block
    // T: MSB[t](GHASH(H,A,C) XOR E(K, Y[0]))
    ByteBuffer deTag(16u);
    ghash(h,
          acc.length() ? acc.data() : DataBlock16 {},
          acc.length(),
          data.length() ? data.data() : DataBlock16 {},
          data.length(),
          deTag.data());
    xorBlocks(deTag.data(), y0_encrypted, deTag.data());
    if (tag.size() && deTag.asHexString() != tag.asHexString()) {
        LOG_ERROR_STATIC("deTag: " << deTag.asHexString());
        LOG_ERROR_STATIC("tag: " << tag.asHexString());
        return {};
    }

    // Y[i]: incr(Y[i-1])           // for i = 1, ..., n - 1
    // C[i]: P[i] XOR E(K, Y[i])    // for i = 1, ..., n - 1
    const size_t n = data.length() / 16u;
    for (size_t i = 0; i < n; ++i) {
        incr(counter);

        DataBlock16 cipherBlock = {};
        DataBlock16 dataBlock = {};
        data.read(dataBlock);
        aes::encryptAes_impl<4, 10>(counter, 16, key).read(cipherBlock);
        xorBlocks(dataBlock, cipherBlock, cipherBlock);

        out.write(cipherBlock);
    }

    if (auto extra = data.length() % 16u) {
        incr(counter);

        DataBlock16 cipherBlock = {};
        DataBlock16 dataBlock = {};
        data.readBytes(dataBlock, extra);
        aes::encryptAes_impl<4, 10>(counter, 16, key).read(cipherBlock);
        xorBlocks(dataBlock, cipherBlock, cipherBlock);

        out.writeArray(cipherBlock, extra);
    }

    return out;
}

ByteBuffer aes_gcm::decrypt(const ByteBuffer &data, const ByteBuffer &key, const ByteBuffer &tag)
{
    return decrypt(data, key, {}, tag);
}

} // namespace psi::tools::crypt