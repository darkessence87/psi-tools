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
     * @brief Encode provided buffer to Base64 buffer.
     * 
     * @param data (in) input buffer
     * @return ByteBuffer encoded buffer
     */
    static ByteBuffer encryptBase64(const ByteBuffer &data);

    /**
     * @brief Decode provided Base64 buffer.
     * 
     * @param data (in) input buffer
     * @return ByteBuffer decoded buffer
     */
    static ByteBuffer decryptBase64(const ByteBuffer &data);

    /**
     * @brief Encode provided buffer using key to AES-128 buffer.
     * 
     * @param data (in) input buffer
     * @param key (in) key buffer
     * @return ByteBuffer encoded buffer
     */
    static ByteBuffer encryptAes128(const ByteBuffer &data, const ByteBuffer &key);

    /**
     * @brief Decode provided AES-128 buffer using key.
     * 
     * @param data (in) input buffer
     * @param key (in) key buffer
     * @return ByteBuffer decoded buffer
     */
    static ByteBuffer decryptAes128(const ByteBuffer &data, const ByteBuffer &key);

    /**
     * @brief Encode provided buffer using key to AES-128 buffer in GCM mode.
     * 
     * @param data (in) input buffer
     * @param key (in) key buffer
     * @param iv (in) iv buffer
     * @param tag (out) tag to be filled in
     * @param add (in, optional) additional data buffer
     * @return ByteBuffer encrypted input buffer
     */
    static ByteBuffer encryptAes128Gcm(const ByteBuffer &data,
                                       const ByteBuffer &key,
                                       const ByteBuffer &iv,
                                       ByteBuffer &tag,
                                       const ByteBuffer &add = {});

    /**
     * @brief Decode provided AES-128 buffer using key in GCM mode.
     * 
     * @param data (in) encrypted input buffer
     * @param key (in) key buffer
     * @param iv (in) iv buffer
     * @param tag (in) tag buffer
     * @param add (in, optional) additional data buffer
     * @return ByteBuffer decrypted input buffer
     */
    static ByteBuffer decryptAes128Gcm(const ByteBuffer &data,
                                       const ByteBuffer &key,
                                       const ByteBuffer &iv,
                                       const ByteBuffer &tag,
                                       const ByteBuffer &add = {});

    /**
     * @brief Encode provided buffer using key to AES-256 buffer.
     * 
     * @param data (in) input buffer
     * @param key (in) key buffer
     * @return ByteBuffer encoded buffer
     */
    static ByteBuffer encryptAes256(const ByteBuffer &data, const ByteBuffer &key);

    /**
     * @brief Decode provided AES-256 buffer using key.
     * 
     * @param data (in) input buffer
     * @param key (in) key buffer
     * @return ByteBuffer decoded buffer
     */
    static ByteBuffer decryptAes256(const ByteBuffer &data, const ByteBuffer &key);

    /**
     * @brief Generate SHA-256 hash for provided byte bufer.
     * 
     * @param data (in) input buffer
     * @return ByteBuffer hash buffer
     */
    static ByteBuffer sha256(const ByteBuffer &data);

    /**
     * @brief Generate HMAC-256 code for provided data using provided key.
     * 
     * @param key (in) key buffer
     * @param data (in) data buffer
     * @return ByteBuffer code buffer
     */
    static ByteBuffer hmac256(const ByteBuffer &key, const ByteBuffer &data);

    /**
     * @brief Generate derived key using HKDF algorithm.
     * 
     * @param key (in) key buffer
     * @param seed (in) seed buffer
     * @param info (in) info buffer
     * @param len (in) expected size of key
     * @return ByteBuffer derived key buffer
     */
    static ByteBuffer hkdf256(const ByteBuffer &key, const ByteBuffer &seed, const ByteBuffer &info, size_t len);

    /**
     * @brief Generate key material using HKDF algorithm.
     * 
     * @param prk (in) pseudo-random-key buffer
     * @param info (in) info buffer
     * @param len (in) expected size of key material
     * @return ByteBuffer key material buffer
     */
    static ByteBuffer hkdf256Expand(const ByteBuffer &prk, const ByteBuffer &info, size_t len);

    /**
     * @brief Generate key material using HKDF algorithm, label and hash.
     * 
     * @param prk (in) pseudo-random-key buffer
     * @param label (in) label of key
     * @param hash (in) hash buffer
     * @param len (in) expected size of key material
     * @return ByteBuffer key material buffer
     */
    static ByteBuffer hkdf256ExpandLabel(const ByteBuffer &prk, const std::string &label, const ByteBuffer &hash, size_t len);

    /**
     * @brief Generate random 32 bytes length buffer.
     * 
     * @return ByteBuffer session key buffer
     */
    static ByteBuffer generateSessionKey();

    static void x25519_generate_keypair(uint8_t *pk, uint8_t *sk);
    static void x25519_scalarmult_base(uint8_t *out, const uint8_t *sk);
    static void x25519_scalarmult(uint8_t *out, const uint8_t *sk, const uint8_t *pk);
};

} // namespace psi::tools
