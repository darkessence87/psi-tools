/**
 * @brief https://luca-giuzzi.unibs.it/corsi/Support/papers-cryptography/gcm-spec.pdf
 * 
 */
#include "aes_gcm.h"
#include "aes.h"

#include <array>

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

inline void rightshiftBlock(std::array<uint8_t, 16> &a)
{
    for (size_t k = 15; k > 0; --k) {
        a[k] = uint8_t(a[k] >> 1) | uint8_t(a[k - 1] << 7);
    }
    a[0] >>= 1;
}

void aes_gcm::xorBlocks(const DataBlock16 &a, const DataBlock16 &b, DataBlock16 &result)
{
    for (size_t i = 0; i < 16u; ++i) {
        result[i] = a[i] ^ b[i];
    }
}

void aes_gcm::xorBlocksInPlace(const uint8_t *src, DataBlock16 &dst)
{
    for (size_t i = 0; i < 16u; ++i) {
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunsafe-buffer-usage"
        dst[i] ^= src[i];
#pragma clang diagnostic pop
    }
}

void aes_gcm::incr(DataBlock16 &counter)
{
    for (size_t i = 16; i != 0; --i) {
        if (++counter[i - 1] != 0) {
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
void aes_gcm::gfMultBlock(const DataBlock16 &x, const DataBlock16 &y, DataBlock16 &z)
{
    DataBlock16 p = {};
    auto v = x;

    for (size_t i = 0; i < 16; ++i) {
        for (size_t j = 0; j < 8; ++j) {
            if (y[i] & (1 << (7 - j))) {
                for (size_t k = 0; k < 16; ++k) {
                    p[k] ^= v[k];
                }
            }

            const bool lsb = v[15] & 0x01;
            rightshiftBlock(v);
            if (lsb) {
                v[0] ^= R_POLY;
            }
        }
    }

    z = p;
}

// GHASH(H,A,C) = X[m+n+1]
// X[i] = 0                                 // for i = 0
// (X[i-1] XOR A[i]) mul H                  // for i = 1, ..., m - 1
// (X[m-1] XOR (A*[m] || 0^128-v)) mul H    // for i = m
// (X[i-1] XOR C[i]) mul H                  // for i = m + 1, ..., m + n - 1
// (X[m+n-1] XOR (C*[m] || 0^128-u)) mul H  // for i = m + n
// (X[m+n] XOR (len(A) || len(C))) mul H    // for i = m + n + 1
void aes_gcm::ghashBlock(const DataBlock16 &h, const uint8_t *data, size_t dataLen, DataBlock16 &result)
{
    DataBlock16 temp = result;

    const size_t m = dataLen / 16u;
    for (size_t i = 0; i < m; ++i) {
        xorBlocksInPlace(shift_ptr(data, i * 16u), temp);
        gfMultBlock(temp, h, temp);
    }

    if (auto extra = dataLen % 16u) {
        DataBlock16 tempExtra = {};
        mem_copy(tempExtra.data(), 0, data, m * 16u, extra);

        xorBlocksInPlace(tempExtra.data(), temp);
        gfMultBlock(temp, h, temp);
    }

    result = temp;
}

void aes_gcm::ghash(const DataBlock16 &h, const uint8_t *acc, size_t accLen, const uint8_t *cipher, size_t cipherLen, DataBlock16 &result)
{
    // std::memset(result.data(), 0, 16);

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
    aes::encryptAes_impl<4, 10>(DataBlock16().data(), 16u, key).read(h);

    // Y[0]: IV || 0^31;            // if len(IV) = 96 bits
    // Y[0]: GHASH(H,{},IV);        // otherwise
    DataBlock16 counter = {};
    if (iv.length() == 12u) {
        iv.readBytes(counter.data(), 12u);
        counter[15] = 0x01;
    } else {
        ghash(h, DataBlock16().data(), 0, iv.length() ? iv.data() : DataBlock16().data(), iv.length(), counter);
    }

    DataBlock16 y0_encrypted = {};
    aes::encryptAes_impl<4, 10>(counter.data(), 16u, key).read(y0_encrypted);

    // Y[i]: incr(Y[i-1])           // for i = 1, ..., n - 1
    // C[i]: P[i] XOR E(K, Y[i])    // for i = 1, ..., n - 1
    const size_t n = data.length() / 16u;
    for (size_t i = 0; i < n; ++i) {
        incr(counter);
        // printBlockHex(counter, "y[i]: ");

        DataBlock16 cipherBlock = {};
        DataBlock16 dataBlock = {};
        data.read(dataBlock);
        aes::encryptAes_impl<4, 10>(counter.data(), 16, key).read(cipherBlock);
        xorBlocks(dataBlock, cipherBlock, cipherBlock);

        out.write(cipherBlock);
    }

    if (auto extra = data.length() % 16u) {
        incr(counter);

        DataBlock16 cipherBlock = {};
        DataBlock16 dataBlock = {};
        data.readBytes(dataBlock.data(), extra);
        aes::encryptAes_impl<4, 10>(counter.data(), 16, key).read(cipherBlock);
        xorBlocks(dataBlock, cipherBlock, cipherBlock);

        out.writeArray(cipherBlock.data(), extra);
    }

    // C*[n]: P*[n] XOR MSB[u](E(K, Y[n]))      // u - number of bits in final block
    // T: MSB[t](GHASH(H,A,C) XOR E(K, Y[0]))
    ghash(h,
          acc.length() ? acc.data() : DataBlock16().data(),
          acc.length(),
          out.length() ? out.data() : DataBlock16().data(),
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
    // H: E(K, 0^128);
    DataBlock16 h = {};
    aes::encryptAes_impl<4, 10>(DataBlock16().data(), 16u, key).read(h);

    // Y[0]: IV || 0^31;            // if len(IV) = 96 bits
    // Y[0]: GHASH(H,{},IV);        // otherwise
    DataBlock16 counter = {};
    if (iv.length() == 12u) {
        iv.readBytes(counter.data(), 12u);
        counter[15] = 0x01;
    } else {
        ghash(h, DataBlock16().data(), 0, iv.length() ? iv.data() : DataBlock16().data(), iv.length(), counter);
    }

    DataBlock16 y0_encrypted = {};
    aes::encryptAes_impl<4, 10>(counter.data(), 16u, key).read(y0_encrypted);

    // C*[n]: P*[n] XOR MSB[u](E(K, Y[n]))      // u - number of bits in final block
    // T: MSB[t](GHASH(H,A,C) XOR E(K, Y[0]))
    DataBlock16 deTag = {};
    ghash(h,
          acc.length() ? acc.data() : DataBlock16().data(),
          acc.length(),
          data.length() ? data.data() : DataBlock16().data(),
          data.length(),
          deTag);
    xorBlocks(deTag, y0_encrypted, deTag);
    const uint8_t* ptr = deTag.data();
    ByteBuffer deTagBuffer(ptr, 16);
    if (tag.size() && deTagBuffer.asHexString() != tag.asHexString()) {
        LOG_ERROR_STATIC("deTag: " << deTagBuffer.asHexString());
        LOG_ERROR_STATIC("tag: " << tag.asHexString());
        return {};
    }

    ByteBuffer out(data.size());
    // Y[i]: incr(Y[i-1])           // for i = 1, ..., n - 1
    // C[i]: P[i] XOR E(K, Y[i])    // for i = 1, ..., n - 1
    const size_t n = data.length() / 16u;
    for (size_t i = 0; i < n; ++i) {
        incr(counter);

        DataBlock16 cipherBlock = {};
        DataBlock16 dataBlock = {};
        data.read(dataBlock);
        aes::encryptAes_impl<4, 10>(counter.data(), 16, key).read(cipherBlock);
        xorBlocks(dataBlock, cipherBlock, cipherBlock);

        out.write(cipherBlock);
    }

    if (auto extra = data.length() % 16u) {
        incr(counter);

        DataBlock16 cipherBlock = {};
        DataBlock16 dataBlock = {};
        data.readBytes(dataBlock.data(), extra);
        aes::encryptAes_impl<4, 10>(counter.data(), 16, key).read(cipherBlock);
        xorBlocks(dataBlock, cipherBlock, cipherBlock);

        out.writeArray(cipherBlock.data(), extra);
    }

    return out;
}

ByteBuffer aes_gcm::decrypt(const ByteBuffer &data, const ByteBuffer &key, const ByteBuffer &tag)
{
    return decrypt(data, key, {}, tag);
}

} // namespace psi::tools::crypt
