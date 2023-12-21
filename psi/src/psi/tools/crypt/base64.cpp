#include "psi/tools/Encryptor.h"

#include "base64.h"

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

const uint8_t base64::m_base64Table[] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M',
                                         'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',
                                         'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm',
                                         'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z',
                                         '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '+', '/'};

const uint8_t base64::m_base64Pad = '=';

ByteBuffer base64::encryptBase64(const ByteBuffer &inputBuffer)
{
    const size_t outSize = ((inputBuffer.size() / 3) + (inputBuffer.size() % 3 > 0)) * 4;
    ByteBuffer encodedString(outSize);

    uint8_t temp1, temp2, temp3;
    uint32_t temp;
    for (size_t idx = 0; idx < inputBuffer.size() / 3; ++idx) {
        if (!inputBuffer.read(temp1)) {
            return ByteBuffer(0u);
        }
        if (!inputBuffer.read(temp2)) {
            return ByteBuffer(0u);
        }
        if (!inputBuffer.read(temp3)) {
            return ByteBuffer(0u);
        }
        temp = (temp1 << 16) | (temp2 << 8) | temp3;

        if (!encodedString.write(m_base64Table[(temp & 0x00FC0000) >> 18])) {
            return ByteBuffer(0u);
        }
        if (!encodedString.write(m_base64Table[(temp & 0x0003F000) >> 12])) {
            return ByteBuffer(0u);
        }
        if (!encodedString.write(m_base64Table[(temp & 0x00000FC0) >> 6])) {
            return ByteBuffer(0u);
        }
        if (!encodedString.write(m_base64Table[(temp & 0x0000003F)])) {
            return ByteBuffer(0u);
        }
    }

    switch (inputBuffer.size() % 3) {
    case 1:
        if (!inputBuffer.read(temp1)) {
            return ByteBuffer(0u);
        }
        temp = temp1 << 16;

        if (!encodedString.write(m_base64Table[(temp & 0x00FC0000) >> 18])) {
            return ByteBuffer(0u);
        }
        if (!encodedString.write(m_base64Table[(temp & 0x0003F000) >> 12])) {
            return ByteBuffer(0u);
        }
        if (!encodedString.write(m_base64Pad)) {
            return ByteBuffer(0u);
        }
        if (!encodedString.write(m_base64Pad)) {
            return ByteBuffer(0u);
        }
        break;
    case 2:
        if (!inputBuffer.read(temp1)) {
            return ByteBuffer(0u);
        }
        if (!inputBuffer.read(temp2)) {
            return ByteBuffer(0u);
        }
        temp = (temp1 << 16) | (temp2 << 8);

        if (!encodedString.write(m_base64Table[(temp & 0x00FC0000) >> 18])) {
            return ByteBuffer(0u);
        }
        if (!encodedString.write(m_base64Table[(temp & 0x0003F000) >> 12])) {
            return ByteBuffer(0u);
        }
        if (!encodedString.write(m_base64Table[(temp & 0x00000FC0) >> 6])) {
            return ByteBuffer(0u);
        }
        if (!encodedString.write(m_base64Pad)) {
            return ByteBuffer(0u);
        }
        break;
    }
    return encodedString;
}

ByteBuffer base64::decryptBase64(const ByteBuffer &inputBuffer)
{
    const size_t inputLen = inputBuffer.size();
    if (inputLen % 4 || inputBuffer.size() == 0) {
        LOG_ERROR_STATIC("Incorrect incoming buffer length: " << inputLen << " for data:" << inputBuffer.asString());
        return ByteBuffer(0u);
    }

    size_t padding = 0;
    if (m_base64Pad == inputBuffer.at(inputLen - 1))
        ++padding;
    if (m_base64Pad == inputBuffer.at(inputLen - 2))
        ++padding;

    const size_t outSize = ((inputLen / 4) * 3) - padding;
    ByteBuffer decodedString(outSize);

    uint8_t tempByte;
    uint32_t temp = 0;
    size_t idx = 0;

    while (idx != inputLen) {
        for (uint8_t q = 0; q < 4; ++q) {
            if (!inputBuffer.read(tempByte)) {
                return ByteBuffer(0u);
            }

            temp <<= 6;

            if (tempByte >= 0x41 && tempByte <= 0x5A)
                temp |= tempByte - 0x41;
            else if (tempByte >= 0x61 && tempByte <= 0x7A)
                temp |= tempByte - 0x47;
            else if (tempByte >= 0x30 && tempByte <= 0x39)
                temp |= tempByte + 0x04;
            else if (tempByte == 0x2B)
                temp |= 0x3E;
            else if (tempByte == 0x2F)
                temp |= 0x3F;
            else if (tempByte == m_base64Pad) {
                switch (inputLen - idx) {
                case 1:
                    if (!decodedString.write(uint8_t((temp >> 16) & 0x000000FF))) {
                        return ByteBuffer(0u);
                    }
                    if (!decodedString.write(uint8_t((temp >> 8) & 0x000000FF))) {
                        return ByteBuffer(0u);
                    }
                    return decodedString;
                case 2:
                    if (!decodedString.write(uint8_t((temp >> 10) & 0x000000FF))) {
                        return ByteBuffer(0u);
                    }
                    return decodedString;
                default:
                    LOG_ERROR_STATIC("Invalid padding in Base64!");
                    return ByteBuffer(0u);
                }
            } else {
                LOG_ERROR_STATIC("Non-valid character in Base64!");
                return ByteBuffer(0u);
            }

            ++idx;
        }

        if (!decodedString.write(uint8_t((temp >> 16) & 0x000000FF))) {
            return ByteBuffer(0u);
        }
        if (!decodedString.write(uint8_t((temp >> 8) & 0x000000FF))) {
            return ByteBuffer(0u);
        }
        if (!decodedString.write(uint8_t((temp)&0x000000FF))) {
            return ByteBuffer(0u);
        }
    }

    return decodedString;
}

} // namespace psi::tools::crypt
