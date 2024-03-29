#include "psi/tools/ByteBuffer.h"
#include "psi/tools/Tools.h"

#ifdef PSI_LOGGER
#include "psi/logger/Logger.h"
#else
#include <iostream>
#include <sstream>
#define LOG_ERROR(x)                                                                                                   \
    do {                                                                                                               \
        std::ostringstream os;                                                                                         \
        os << x;                                                                                                       \
        std::cout << os.str() << std::endl;                                                                            \
    } while (0)
#endif

namespace psi::tools {

ByteBuffer::ByteBuffer()
    : m_bufferSz(0u)
    , m_buffer(new uint8_t[m_bufferSz]())
{
}

ByteBuffer::ByteBuffer(size_t sz)
    : m_bufferSz(sz)
    , m_buffer(new uint8_t[m_bufferSz]())
{
}

ByteBuffer::ByteBuffer(uint8_t &&arr, size_t sz)
    : m_bufferSz(sz)
    , m_buffer(std::move(&arr))
    , m_writeIndex(sz)
{
}

ByteBuffer::ByteBuffer(uint8_t *arr, size_t sz)
    : m_bufferSz(sz)
    , m_buffer(std::move(arr))
    , m_writeIndex(sz)
{
}

ByteBuffer::ByteBuffer(const uint8_t *arr, size_t sz)
    : ByteBuffer(sz)
{
    writeArray(arr, sz);
}

ByteBuffer::ByteBuffer(const std::string &data, bool isHex)
    : m_bufferSz(data.size() / (isHex ? 2 : 1))
    , m_buffer(new uint8_t[m_bufferSz]())
{
    if (isHex) {
        writeHexString(data);
    } else {
        writeString(data);
    }
}

ByteBuffer::ByteBuffer(const ByteBuffer &bb)
    : m_bufferSz(bb.m_bufferSz)
    , m_buffer(new uint8_t[m_bufferSz]())
    , m_readIndex(bb.m_readIndex)
    , m_writeIndex(bb.m_writeIndex)
{
    memcpy(m_buffer, bb.m_buffer, m_bufferSz);
}

ByteBuffer::~ByteBuffer()
{
    if (m_buffer) {
        delete[] m_buffer;
        m_buffer = nullptr;
    }
}

ByteBuffer &ByteBuffer::operator=(const ByteBuffer &bb)
{
    if (this == &bb) {
        return *this;
    }

    if (m_buffer) {
        delete[] m_buffer;
        m_buffer = nullptr;
    }

    m_bufferSz = bb.size();
    m_buffer = new uint8_t[m_bufferSz]();
    memcpy(m_buffer, bb.m_buffer, m_bufferSz);

    m_readIndex = bb.m_readIndex;
    m_writeIndex = bb.m_writeIndex;

    return *this;
}

ByteBuffer &ByteBuffer::operator+=(const ByteBuffer &bb)
{
    auto newBufferSz = this->m_bufferSz + bb.size();
    auto newBuffer = new uint8_t[newBufferSz]();

    if (m_buffer) {
        memcpy(newBuffer, m_buffer, m_bufferSz);

        delete[] m_buffer;
        m_buffer = nullptr;
    }

    m_buffer = newBuffer;
    m_bufferSz = newBufferSz;

    bb.readToByteBuffer(*this, bb.m_bufferSz);

    return *this;
}

ByteBuffer ByteBuffer::operator+(const ByteBuffer &bb) const
{
    this->resetRead();
    bb.resetRead();

    ByteBuffer newBuffer(this->m_bufferSz + bb.size());
    this->readToByteBuffer(newBuffer, m_bufferSz);
    bb.readToByteBuffer(newBuffer, bb.m_bufferSz);
    return newBuffer;
}

void ByteBuffer::clear()
{
    memset(m_buffer, 0, m_bufferSz);
    reset();
}

void ByteBuffer::reset() const
{
    resetRead();
    resetWrite();
}

void ByteBuffer::resetRead() const
{
    m_readIndex = 0;
}

void ByteBuffer::resetWrite() const
{
    m_writeIndex = 0;
}

bool ByteBuffer::skipRead(const size_t N) const
{
    if (m_readIndex + N > m_bufferSz) {
        LOG_ERROR("Data (size: " << N << ") cannot be skipped. " << (m_bufferSz - m_readIndex)
                                 << " bytes left to reach the end.");
        return false;
    }

    m_readIndex += N;
    return true;
}

bool ByteBuffer::skipWrite(const size_t N) const
{
    if (m_writeIndex + N > m_bufferSz) {
        LOG_ERROR("Data (size: " << N << ") cannot be skipped. " << (m_bufferSz - m_writeIndex)
                                 << " bytes left to reach the end.");
        return false;
    }

    m_writeIndex += N;
    return true;
}

uint8_t ByteBuffer::at(const size_t pos) const
{
    return m_buffer[pos];
}

uint8_t *ByteBuffer::data() const
{
    if (m_bufferSz == 0) {
        return nullptr;
    }

    return m_buffer;
}

const std::vector<uint8_t> ByteBuffer::asVector() const
{
    std::vector<uint8_t> result;
    result.resize(m_bufferSz);
    memcpy(&result[0], m_buffer, m_bufferSz);
    return result;
}

const std::vector<uint64_t> ByteBuffer::asHash() const
{
    const size_t blocks = m_bufferSz / 8;
    const size_t remain = m_bufferSz % 8;
    std::vector<uint64_t> result;
    result.reserve(blocks + (remain ? 1 : 0));

    for (size_t i = 0; i < blocks * 8; i += 8) {
        uint64_t v = m_buffer[i];
        for (uint8_t k = 1; k < 8; ++k) {
            v |= (uint64_t)m_buffer[i + k] << (8 * k);
        }
        result.emplace_back(v);
    }

    if (remain) {
        const size_t baseIndex = blocks * 8;
        uint64_t v = m_buffer[baseIndex];
        for (size_t i = 0; i < remain; ++i) {
            v |= (uint64_t)m_buffer[baseIndex + i] << (8 * i);
        }
        result.emplace_back(v);
    }

    return result;
}

const std::string ByteBuffer::asHexString() const
{
    return tools::to_hex_string(m_buffer, m_bufferSz);
}

const std::string ByteBuffer::asHexStringFormatted() const
{
    std::string result;
    result.resize(m_bufferSz * 3 + 3);

    auto toHex = [](uint8_t c) -> uint8_t {
        uint8_t r = c & 0xf;
        if (r <= 0x9) {
            return r + uint8_t(0x30);
        }
        return r + uint8_t(0x57);
    };

    size_t index = 0;
    result[index++] = '[';
    result[index++] = ' ';
    for (size_t i = 0; i < m_bufferSz; ++i) {
        result[index++] = toHex(m_buffer[i] >> 4);
        result[index++] = toHex(m_buffer[i]);
        result[index++] = ' ';
    }
    result[index++] = ']';

    return result;
}

const std::string ByteBuffer::asString() const
{
    std::string result;
    result.reserve(m_bufferSz);

    for (size_t i = 0; i < m_bufferSz; ++i) {
        uint8_t c = m_buffer[i];
        if ((c >= 0x20 && c <= 0x7e) || c == '\n' || c == '\t' || c == '\r') {
            result.push_back(c);
        }
    }
    result.shrink_to_fit();

    return result;
}

size_t ByteBuffer::size() const
{
    return m_bufferSz;
}

size_t ByteBuffer::length() const
{
    return m_writeIndex;
}

size_t ByteBuffer::remainingLength() const
{
    return m_writeIndex - m_readIndex;
}

bool ByteBuffer::writeString(const std::string &data)
{
    const auto sz = data.size();
    if (m_writeIndex + sz > m_bufferSz) {
        LOG_ERROR("Data (size: " << sz << ") cannot be written to buffer. " << (m_bufferSz - m_writeIndex)
                                 << " free bytes left. Cannot write!");
        return false;
    }

    memcpy(&m_buffer[m_writeIndex], &data[0], sz);
    m_writeIndex += sz;

    return true;
}

bool ByteBuffer::writeHexString(const std::string &data)
{
    if (data.size() % 2 != 0) {
        LOG_ERROR("Incorrect hex string size, should be % 2, data:" << data);
        return false;
    }

    const auto sz = data.size() / 2;
    if (m_writeIndex + sz > m_bufferSz) {
        LOG_ERROR("Data (size: " << sz << ") cannot be written to buffer. " << (m_bufferSz - m_writeIndex)
                                 << " free bytes left. Cannot write!");
        return false;
    }

    // specific 'stoul' is faster than standart for hex digits
    auto stoul = [](uint8_t *data) -> uint8_t {
        auto get = [](uint8_t c) -> uint8_t {
            if (c >= 0x30 && c <= 0x39) {
                return c - uint8_t(0x30);
            } else if (c >= 0x61 && c <= 0x66) {
                return c - uint8_t(0x57);
            } else if (c >= 0x41 && c <= 0x46) {
                return c - uint8_t(0x37);
            }
            throw std::invalid_argument("Invalid argument provided: is not hex-digit");
        };
        return (get(data[0]) << 4) | get(data[1]);
    };

    for (size_t i = 0; i < data.size(); i += 2) {
        const uint8_t v = stoul((uint8_t *)&data[i]);
        m_buffer[m_writeIndex] = v;
        ++m_writeIndex;
    }

    return true;
}

bool ByteBuffer::readString(std::string &data, const size_t N) const
{
    const auto sz = N;
    if (m_readIndex + sz > m_bufferSz) {
        LOG_ERROR("Data (size: " << sz << ") cannot be read. " << (m_bufferSz - m_readIndex)
                                 << " bytes left to reach the end.");
        return false;
    }

    data.clear();
    data.resize(sz);
    memcpy(&data[0], &m_buffer[m_readIndex], sz);
    m_readIndex += sz;

    return true;
}

bool ByteBuffer::readLine(std::string &data, const uint8_t *delimiters, size_t N) const
{
    uint8_t delims[3] = {0x0a, 0x0d, 0x17};
    if (!delimiters) {
        delimiters = delims;
        N = 3;
    }
    constexpr size_t BLOCK_SZ = 64u;
    uint8_t cache[BLOCK_SZ];
    size_t index = 0;
    size_t blockIndex = 0;

    data.clear();
    data.resize(BLOCK_SZ);

    uint8_t b = 0;
    size_t trailedDelimiters = 0;
    while (read(b)) {
        bool isDelimiter = false;
        for (uint8_t k = 0; k < N; ++k) {
            if (delimiters[k] == b) {
                isDelimiter = true;
                break;
            }
        }
        if (isDelimiter) {
            ++trailedDelimiters;
        } else {
            if (trailedDelimiters) {
                --m_readIndex;
                break;
            }
            cache[index++] = b;
            if (index >= BLOCK_SZ) {
                memcpy(&data[BLOCK_SZ * blockIndex], cache, BLOCK_SZ);
                data.resize(BLOCK_SZ * (++blockIndex + 1));
                index = 0;
            }
        }
    }
    data.resize(BLOCK_SZ * blockIndex + index);
    memcpy(&data[BLOCK_SZ * blockIndex], cache, index);

    return remainingLength() > 0;
}

ByteBuffer ByteBuffer::readToByteBuffer(const size_t N) const
{
    const auto sz = N;
    if (m_readIndex + sz > m_bufferSz) {
        LOG_ERROR("Data (size: " << sz << ") cannot be read. " << (m_bufferSz - m_readIndex)
                                 << " bytes left to reach the end.");
        return ByteBuffer(0u);
    }

    uint8_t *temp = new uint8_t[sz]();
    memcpy(temp, &m_buffer[m_readIndex], sz);
    m_readIndex += sz;

    return ByteBuffer(std::move(*temp), sz);
}

bool ByteBuffer::readToByteBuffer(ByteBuffer &buffer, const size_t N) const
{
    const auto sz = N;
    if (m_readIndex + sz > m_bufferSz) {
        LOG_ERROR("Data (size: " << sz << ") cannot be read. " << (m_bufferSz - m_readIndex)
                                 << " bytes left to reach the end.");
        return false;
    }

    if (buffer.m_writeIndex + sz > buffer.m_bufferSz) {
        LOG_ERROR("Data (size: " << sz << ") cannot be written. " << (buffer.m_bufferSz - buffer.m_writeIndex)
                                 << " bytes left to reach the end.");
        return false;
    }

    readBytes(buffer.m_buffer + buffer.m_writeIndex, sz);
    buffer.m_writeIndex += sz;

    return true;
}

bool ByteBuffer::readToByteBuffer(ByteBuffer &buffer) const
{
    const auto sz = length();
    if (m_readIndex + sz > m_bufferSz) {
        LOG_ERROR("Data (size: " << sz << ") cannot be read. " << (m_bufferSz - m_readIndex)
                                 << " bytes left to reach the end.");
        return false;
    }

    if (buffer.m_writeIndex + sz > buffer.m_bufferSz) {
        LOG_ERROR("Data (size: " << sz << ") cannot be written. " << (buffer.m_bufferSz - buffer.m_writeIndex)
                                 << " bytes left to reach the end.");
        return false;
    }

    readBytes(buffer.m_buffer + buffer.m_writeIndex, sz);
    buffer.m_writeIndex += sz;

    return true;
}

} // namespace psi::tools