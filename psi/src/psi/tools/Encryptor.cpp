#include "psi/tools/Encryptor.h"

#include "crypt/aes.h"
#include "crypt/aes_gcm.h"
#include "crypt/base64.h"
#include "crypt/sha.h"
#include "crypt/x25519.h"

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

ByteBuffer Encryptor::encryptBase64(const ByteBuffer &inputBuffer)
{
    return crypt::base64::encryptBase64(inputBuffer);
}

ByteBuffer Encryptor::decryptBase64(const ByteBuffer &inputBuffer)
{
    return crypt::base64::decryptBase64(inputBuffer);
}

ByteBuffer Encryptor::encryptAes128(const ByteBuffer &inputData, const ByteBuffer &key)
{
    return crypt::aes::encryptAes_impl<4, 10>(inputData, key);
}

ByteBuffer Encryptor::decryptAes128(const ByteBuffer &inputData, const ByteBuffer &key)
{
    return crypt::aes::decryptAes_impl<4, 10>(inputData, key);
}

ByteBuffer Encryptor::encryptAes128Gcm(const ByteBuffer &inputData,
                                       const ByteBuffer &key,
                                       const ByteBuffer &iv,
                                       ByteBuffer &tagBuffer,
                                       const ByteBuffer &acc)
{
    crypt::aes_gcm::Tag tag;
    auto encoded = crypt::aes_gcm::encrypt(inputData, key, iv, tag, acc);
    tagBuffer.write(tag);
    return encoded;
}

ByteBuffer Encryptor::decryptAes128Gcm(const ByteBuffer &inputData,
                                       const ByteBuffer &key,
                                       const ByteBuffer &iv,
                                       const ByteBuffer &tag,
                                       const ByteBuffer &acc)
{
    return crypt::aes_gcm::decrypt(inputData, key, iv, tag, acc);
}

ByteBuffer Encryptor::encryptAes256(const ByteBuffer &inputData, const ByteBuffer &key)
{
    return crypt::aes::encryptAes_impl<8, 14>(inputData, key);
}

ByteBuffer Encryptor::decryptAes256(const ByteBuffer &inputData, const ByteBuffer &key)
{
    return crypt::aes::decryptAes_impl<8, 14>(inputData, key);
}

ByteBuffer Encryptor::sha256(const ByteBuffer &data)
{
    return crypt::sha::encode256(data);
}

ByteBuffer Encryptor::hmac256(const ByteBuffer &key, const ByteBuffer &data)
{
    return crypt::sha::hmac256(key, data);
}

ByteBuffer Encryptor::hkdf256(const ByteBuffer &key, const ByteBuffer &seed, const ByteBuffer &info, size_t len)
{
    return crypt::sha::hkdf256(key, seed, info, len);
}

ByteBuffer Encryptor::hkdf256Expand(const ByteBuffer &prk, const ByteBuffer &info, size_t len)
{
    return crypt::sha::hkdf256Expand(prk, info, len);
}

ByteBuffer Encryptor::hkdf256ExpandLabel(const ByteBuffer &prk, const std::string &label, const ByteBuffer &hash, size_t len)
{
    prk.resetRead();
    hash.resetRead();

    ByteBuffer info(3u + label.size() + 1u + hash.size());
    info.write(uint8_t(0));
    info.write(uint8_t(len));
    info.write(uint8_t(label.size()));
    info.writeString(label);
    info.write(uint8_t(hash.size()));
    hash.readToByteBuffer(info, hash.size());
    return crypt::sha::hkdf256Expand(prk, info, len);
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

    return sessionKey;
}

void Encryptor::x25519_generate_keypair(ByteBuffer &publicKey, ByteBuffer &privateKey)
{
    crypt::x25519::generate_keypair(publicKey.data(), privateKey.data());
}

ByteBuffer Encryptor::x25519_scalarmult_base(const ByteBuffer &privateKey)
{
    ByteBuffer out(32u);
    crypt::x25519::scalarmult_base(out.data(), privateKey.data());
    return out;
}

ByteBuffer Encryptor::x25519_scalarmult(const ByteBuffer &privateKey, const ByteBuffer &publicKey)
{
    ByteBuffer out(32u);
    crypt::x25519::scalarmult(out.data(), privateKey.data(), publicKey.data());
    return out;
}

} // namespace psi::tools
