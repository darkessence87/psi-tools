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

const std::array<uint8_t, 64u> base64::m_base64Table = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M',
                                                        'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',
                                                        'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm',
                                                        'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z',
                                                        '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '+', '/'};

const uint8_t base64::m_base64Pad = '=';

ByteBuffer base64::encryptBase64(const ByteBuffer &in)
{
    const size_t outSize = ((in.size() / 3) + (in.size() % 3 > 0)) * 4;
    ByteBuffer out(outSize);

    auto generate_fail = [&]() { out.resize(0u); };

    uint8_t temp1, temp2, temp3;
    uint32_t temp;
    for (size_t idx = 0; idx < in.size() / 3; ++idx) {
        if (!in.read(temp1) || !in.read(temp2) || !in.read(temp3)) {
            generate_fail();
            return out;
        }
        temp = uint32_t(temp1 << 16u) | uint32_t(temp2 << 8u) | uint32_t(temp3);

        if (!out.write(m_base64Table[(temp & 0x00fc0000) >> 18]) || !out.write(m_base64Table[(temp & 0x0003f000) >> 12])
            || !out.write(m_base64Table[(temp & 0x00000fc0) >> 6]) || !out.write(m_base64Table[(temp & 0x0000003f)])) {
            generate_fail();
            return out;
        }
    }

    switch (in.size() % 3) {
    case 1:
        if (!in.read(temp1)) {
            generate_fail();
            return out;
        }
        temp = uint32_t(temp1 << 16);

        if (!out.write(m_base64Table[(temp & 0x00FC0000) >> 18]) || !out.write(m_base64Table[(temp & 0x0003F000) >> 12])
            || !out.write(m_base64Pad) || !out.write(m_base64Pad)) {
            generate_fail();
            return out;
        }
        break;
    case 2:
        if (!in.read(temp1) || !in.read(temp2)) {
            generate_fail();
            return out;
        }
        temp = uint32_t(temp1 << 16) | uint32_t(temp2 << 8);

        if (!out.write(m_base64Table[(temp & 0x00FC0000) >> 18]) || !out.write(m_base64Table[(temp & 0x0003F000) >> 12])
            || !out.write(m_base64Table[(temp & 0x00000FC0) >> 6]) || !out.write(m_base64Pad)) {
            generate_fail();
            return out;
        }
        break;
    }

    return out;
}

ByteBuffer base64::decryptBase64(const ByteBuffer &in)
{
    ByteBuffer out(0u);

    const size_t inputLen = in.size();
    if (inputLen % 4 || in.size() == 0) {
        LOG_ERROR_STATIC("Incorrect incoming buffer length: " << inputLen << " for data:" << in.asString());
        return out;
    }

    size_t padding = 0;
    if (m_base64Pad == in.at(inputLen - 1)) {
        ++padding;
    }
    if (m_base64Pad == in.at(inputLen - 2)) {
        ++padding;
    }

    const size_t outSize = ((inputLen / 4) * 3) - padding;
    out.resize(outSize);

    auto generate_fail = [&]() { out.resize(0u); };

    uint8_t tempByte;
    uint32_t temp = 0;
    size_t idx = 0;

    while (idx != inputLen) {
        for (uint8_t q = 0; q < 4; ++q) {
            if (!in.read(tempByte)) {
                generate_fail();
                return out;
            }

            temp <<= 6;

            if (tempByte >= 0x41 && tempByte <= 0x5A) {
                temp |= tempByte - 0x41;
            } else if (tempByte >= 0x61 && tempByte <= 0x7A) {
                temp |= tempByte - 0x47;
            } else if (tempByte >= 0x30 && tempByte <= 0x39) {
                temp |= tempByte + 0x04;
            } else if (tempByte == 0x2B) {
                temp |= 0x3E;
            } else if (tempByte == 0x2F) {
                temp |= 0x3F;
            } else if (tempByte == m_base64Pad) {
                switch (inputLen - idx) {
                case 1:
                    if (!out.write(uint8_t((temp >> 16) & 0x000000FF)) || !out.write(uint8_t((temp >> 8) & 0x000000FF))) {
                        generate_fail();
                    }
                    return out;
                case 2:
                    if (!out.write(uint8_t((temp >> 10) & 0x000000FF))) {
                        generate_fail();
                    }
                    return out;
                default:
                    LOG_ERROR_STATIC("Invalid padding in Base64!");
                    generate_fail();
                    return out;
                }
            } else {
                LOG_ERROR_STATIC("Non-valid character in Base64!");
                generate_fail();
                return out;
            }

            ++idx;
        }

        if (!out.write(uint8_t((temp >> 16) & 0x000000FF)) || !out.write(uint8_t((temp >> 8) & 0x000000FF))
            || !out.write(uint8_t((temp) & 0x000000FF))) {
            generate_fail();
            return out;
        }
    }

    return out;
}

} // namespace psi::tools::crypt
