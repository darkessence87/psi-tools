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
    using DataBlock16 = uint8_t[4u][4u];
    using SubKey = uint8_t[16u];
    using SubKey128 = uint32_t;
    using SubKeys128 = SubKey[11u];
    using SubKeys256 = SubKey[15u];

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

    static const uint8_t m_sBox[256u];
    static const uint8_t m_iBox[256u];
    static const uint8_t m_rCon[10u];
    static const uint8_t m_base64Table[64u];
    static const uint8_t m_base64Pad;

    static void rotWord(uint8_t[4]);
    static void subWord(uint8_t[4]);
    static uint32_t &rotWord(uint32_t &);

    static void subBytes(const uint8_t box[256u], Encryptor::DataBlock16 &block);
    static void shiftRows(Encryptor::DataBlock16 &block);
    static void invShiftRows(Encryptor::DataBlock16 &block);
    static void mixColumns(Encryptor::DataBlock16 &block);
    static void invMixColumns(Encryptor::DataBlock16 &block);
    static void applySubKey(const uint8_t key[16u], Encryptor::DataBlock16 &block);
    static void writeBlock(const uint8_t *const data, size_t dataSz, Encryptor::DataBlock16 &block);
    static void readBlock(const Encryptor::DataBlock16 &block, uint8_t *data, size_t dataSz);

    template <uint8_t Nk, uint8_t Nr>
    static ByteBuffer encryptAes_impl(const ByteBuffer &, const ByteBuffer &);

    template <uint8_t Nk, uint8_t Nr>
    static ByteBuffer decryptAes_impl(const ByteBuffer &, const ByteBuffer &);

    template <uint8_t Nk, uint8_t Nr>
    static void generateSubKeys_impl(uint8_t[Nk * 4u], SubKey[Nr + 1u]);
};

class Encryptor_x25519
{
public:
    static void generate_keypair(uint8_t *pk, uint8_t *sk);
    static void scalarmult_base(uint8_t *out, const uint8_t *sk);
    static void scalarmult(uint8_t *out, const uint8_t *sk, const uint8_t *pk);

private:
    using field_elem = int64_t[16];
    static void unpack25519(field_elem out, const uint8_t *in);
    static void carry25519(field_elem elem);
    static void fadd(field_elem out, const field_elem a, const field_elem b);
    static void fsub(field_elem out, const field_elem a, const field_elem b);
    static void fmul(field_elem out, const field_elem a, const field_elem b);
    static void finverse(field_elem out, const field_elem in);
    static void swap25519(field_elem p, field_elem q, int64_t bit);
    static void pack25519(uint8_t *out, const field_elem in);

private:
    static const field_elem _121665;
}; // namespace x25519

} // namespace psi::tools
