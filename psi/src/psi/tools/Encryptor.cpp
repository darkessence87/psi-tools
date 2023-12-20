#include "psi/tools/Encryptor.h"

#include "crypt/EncryptorAes.h"
#include "crypt/EncryptorBase64.h"
#include "crypt/Encryptor_x25519.h"


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
    return crypt::EncryptorBase64::encryptBase64(inputBuffer);
}

ByteBuffer Encryptor::decryptBase64(const ByteBuffer &inputBuffer)
{
    return crypt::EncryptorBase64::decryptBase64(inputBuffer);
}

ByteBuffer Encryptor::encryptAes128(const ByteBuffer &inputData, const ByteBuffer &key)
{
    return crypt::EncryptorAes::encryptAes_impl<4, 10>(inputData, key);
}

ByteBuffer Encryptor::decryptAes128(const ByteBuffer &inputData, const ByteBuffer &key)
{
    return crypt::EncryptorAes::decryptAes_impl<4, 10>(inputData, key);
}

ByteBuffer Encryptor::encryptAes256(const ByteBuffer &inputData, const ByteBuffer &key)
{
    return crypt::EncryptorAes::encryptAes_impl<8, 14>(inputData, key);
}

ByteBuffer Encryptor::decryptAes256(const ByteBuffer &inputData, const ByteBuffer &key)
{
    return crypt::EncryptorAes::decryptAes_impl<8, 14>(inputData, key);
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

void Encryptor::x25519_generate_keypair(uint8_t *pk, uint8_t *sk)
{
    crypt::Encryptor_x25519::generate_keypair(pk, sk);
}

void Encryptor::x25519_scalarmult_base(uint8_t *out, const uint8_t *sk)
{
    crypt::Encryptor_x25519::scalarmult_base(out, sk);
}

void Encryptor::x25519_scalarmult(uint8_t *out, const uint8_t *sk, const uint8_t *pk)
{
    crypt::Encryptor_x25519::scalarmult(out, sk, pk);
}

} // namespace psi::tools
