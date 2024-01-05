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

    for (size_t i = 0; i < m_bufferSz; ++i) {
        result.emplace_back(m_buffer[i]);
    }

    return result;
}

const std::vector<uint64_t> ByteBuffer::asHash() const
{
    std::vector<uint64_t> result;

    const size_t blocks = m_bufferSz / 8;
    const size_t remain = m_bufferSz % 8;

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
    std::ostringstream os;

    for (size_t i = 0; i < m_bufferSz; ++i) {
        uint16_t n = m_buffer[i];
        os << to_hex_string(n);
    }

    return os.str();
}

const std::string ByteBuffer::asHexStringFormatted() const
{
    std::ostringstream os;
    os << "[ ";

    for (size_t i = 0; i < m_bufferSz; ++i) {
        uint16_t n = m_buffer[i];
        os << to_hex_string(n) << " ";
    }

    os << "]";

    return os.str();
}

const std::string ByteBuffer::asString() const
{
    std::ostringstream os;

    for (size_t i = 0; i < m_bufferSz; ++i) {
        uint8_t c = m_buffer[i];
        if ((c >= 0x20 && c <= 0x7E) || c == '\n' || c == '\t' || c == '\r') {
            os << m_buffer[i];
        }
    }

    return os.str();
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

    for (const uint8_t value : data) {
        m_buffer[m_writeIndex] = value;
        ++m_writeIndex;
    }

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

    for (size_t i = 0; i < data.size(); i += 2) {
        const std::string temp = data.substr(i, 2);
        const uint8_t v = static_cast<uint8_t>(std::stoul(temp, nullptr, 16));
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
    for (size_t i = 0; i < sz; ++i) {
        data[i] = m_buffer[m_readIndex];
        ++m_readIndex;
    }

    return true;
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