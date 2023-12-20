#pragma once

#include "psi/tools/ByteBuffer.h"

namespace psi::tools::crypt {

class EncryptorBase64
{
public:
    static const uint8_t m_base64Table[64u];
    static const uint8_t m_base64Pad;

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
};

} // namespace psi::tools
