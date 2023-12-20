#pragma once

#include "ByteBuffer.h"

namespace psi::tools {

/**
 * @brief Encryptor class is used for encode/decode data to/from Base64/AES256 formats.
 * 
 */
class Encryptor
{
public:
    /**
     * @brief Encodes provided buffer to Base64 buffer
     * 
     * @param data input buffer
     * @return ByteBuffer encoded buffer
     */
    static ByteBuffer encryptBase64(const ByteBuffer &data);

    /**
     * @brief Decodes provided Base64 buffer
     * 
     * @param data input buffer
     * @return ByteBuffer decoded buffer
     */
    static ByteBuffer decryptBase64(const ByteBuffer &data);

    /**
     * @brief Encodes provided buffer using key to AES128 buffer
     * 
     * @param data input buffer
     * @param key key buffer
     * @return ByteBuffer encoded buffer
     */
    static ByteBuffer encryptAes128(const ByteBuffer &data, const ByteBuffer &key);

    /**
     * @brief Decodes provided AES128 buffer using key
     * 
     * @param data input buffer
     * @param key key buffer
     * @return ByteBuffer decoded buffer
     */
    static ByteBuffer decryptAes128(const ByteBuffer &data, const ByteBuffer &key);

    /**
     * @brief Encodes provided buffer using key to AES256 buffer
     * 
     * @param data input buffer
     * @param key key buffer
     * @return ByteBuffer encoded buffer
     */
    static ByteBuffer encryptAes256(const ByteBuffer &data, const ByteBuffer &key);

    /**
     * @brief Decodes provided AES256 buffer using key
     * 
     * @param data input buffer
     * @param key key buffer
     * @return ByteBuffer decoded buffer
     */
    static ByteBuffer decryptAes256(const ByteBuffer &data, const ByteBuffer &key);

    /**
     * @brief Generates random 32 bytes length buffer
     * 
     * @return ByteBuffer session key
     */
    static ByteBuffer generateSessionKey();

    static void x25519_generate_keypair(uint8_t *pk, uint8_t *sk);
    static void x25519_scalarmult_base(uint8_t *out, const uint8_t *sk);
    static void x25519_scalarmult(uint8_t *out, const uint8_t *sk, const uint8_t *pk);
};

} // namespace psi::tools
