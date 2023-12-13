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
    using SubKey256 = uint8_t[16u];
    using SubKeys256 = SubKey256[15u];

    /**
     * @brief Encodes provided buffer to Base64 buffer
     * 
     * @return ByteBuffer encoded buffer
     */
    static ByteBuffer encryptBase64(const ByteBuffer &);

    /**
     * @brief Decodes provided Base64 buffer
     * 
     * @return ByteBuffer decoded buffer
     */
    static ByteBuffer decryptBase64(const ByteBuffer &);

    /**
     * @brief Encodes provided buffer using key to AES256 buffer
     * 
     * @return ByteBuffer encoded buffer
     */
    static ByteBuffer encryptAes256(const ByteBuffer &, const ByteBuffer &);

    /**
     * @brief Decodes provided AES256 buffer using key
     * 
     * @return ByteBuffer decoded buffer
     */
    static ByteBuffer decryptAes256(const ByteBuffer &, const ByteBuffer &);

    /**
     * @brief Generates random 32 bytes length buffer
     * 
     * @return ByteBuffer session key
     */
    static ByteBuffer generateSessionKey();

    static const uint8_t CONST_BLOCK_LEN_BYTES;
    static const uint8_t CONST_KEY_LEN_BYTES;
    static const uint8_t CONST_ROUNDS_NUMBER;
    static const uint8_t m_sBox[256u];
    static const uint8_t m_iBox[256u];
    static const uint8_t m_rCon[10u];
    static const uint8_t m_base64Table[64u];
    static const uint8_t m_base64Pad;

    static void generateSubKeys(uint8_t[32], SubKeys256 &);

    static void scheduleKey(uint8_t[4], const uint8_t);
    static void rotWord(uint8_t[4]);
    static void subWord(uint8_t[4]);

    static void subBytes(const uint8_t box[256u], Encryptor::DataBlock16 &block);
    static void shiftRows(Encryptor::DataBlock16 &block);
    static void invShiftRows(Encryptor::DataBlock16 &block);
    static void mixColumns(Encryptor::DataBlock16 &block);
    static void invMixColumns(Encryptor::DataBlock16 &block);
    static void applySubKey(const uint8_t key[16u], Encryptor::DataBlock16 &block);
    static void writeBlock(const uint8_t *const data, size_t dataSz, Encryptor::DataBlock16 &block);
    static void readBlock(const Encryptor::DataBlock16 &block, uint8_t *data, size_t dataSz);

    static inline void doRoundKeyEncode(const SubKey256 &, DataBlock16 &, bool /*isFinal*/ = false);
    static inline void doRoundKeyDecode(const SubKey256 &, DataBlock16 &, bool /*isFinal*/ = false);
};

} // namespace psi::tools
