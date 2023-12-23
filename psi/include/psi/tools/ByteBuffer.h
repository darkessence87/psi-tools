#pragma once

#include <stdint.h>
#include <string>
#include <vector>

#include "psi/tools/Tools.h"

namespace psi::tools {

/**
 * @brief ByteBuffer class represents a wrapper of C-style 1-byte buffer.
 * Automatically manages memory. Provides interface to read/write/convert operations on a byte buffer.
 * 
 */
class ByteBuffer
{
public:
    /**
     * @brief Construct a new Byte Buffer object. Default size: 1024 bytes.
     * 
     */
    ByteBuffer();

    /**
     * @brief Construct a new Byte Buffer object using provided size.
     * 
     */
    ByteBuffer(size_t);

    /**
     * @brief Construct a new Byte Buffer object by moving an existing byte buffer into ByteBuffer as r-value.
     * 
     */
    explicit ByteBuffer(uint8_t &&, size_t);

    /**
     * @brief Construct a new Byte Buffer object by moving an existing byte buffer into ByteBuffer as l-value.
     * 
     */
    explicit ByteBuffer(uint8_t *, size_t);

    /**
     * @brief Construct a new Byte Buffer object by string
     * 
     * @param data string
     * @param isHex if true buffer interprets data as hex input
     * default value: false
     */
    ByteBuffer(const std::string &data, bool isHex = false);

    /**
     * @brief Destroy the Byte Buffer object. Frees memory.
     * 
     */
    virtual ~ByteBuffer();

    /**
     * @brief Construct a new Byte Buffer object from another ByteBuffer object.
     * 
     */
    ByteBuffer(const ByteBuffer &);

    /**
     * @brief Replaces existing ByteBuffer object with provided ByteBuffer object.
     * 
     * @return ByteBuffer& reference to existing ByteBuffer object
     */
    ByteBuffer &operator=(const ByteBuffer &);

    /**
     * @brief Clears the data without freeing or resizing the memory.
     * Sets the buffer to default state. ByteBuffer might be read/write like after first creation.
     * 
     */
    void clear();

    /**
     * @brief Sets the buffer to default state. ByteBuffer might be read/write like after first creation.
     * This method does not change the data already written to buffer.
     * 
     */
    void reset() const;

    /**
     * @brief Writes custom data into ByteBuffer.
     * 
     * @tparam T type of custom data
     * @param data custom data
     * @return true if operation is successful, writeIndex is increased by size of custom data type
     * @return false if operation is failed, writeIndex is not changed
     */
    template <typename T>
    bool write(const T &data)
    {
        const auto sz = sizeof(T);
        if (m_writeIndex + sz > m_bufferSz) {
            //LOG_ERROR("Data (size: " << sz << ") cannot be written to buffer. " << (m_bufferSz - m_writeIndex) << " free bytes left. Cannot write!");
            return false;
        }

        memcpy(m_buffer + m_writeIndex, &data, sz);
        m_writeIndex += sz;

        return true;
    }

    /**
     * @brief Writes custom data into ByteBuffer in opposite endian.
     * 
     * @tparam T type of custom data
     * @param data custom data
     * @return true if operation is successful, writeIndex is increased by size of custom data type
     * @return false if operation is failed, writeIndex is not changed
     */
    template <typename T>
    bool writeSwapped(const T &data)
    {
        return write(swapEndian(data));
    }

    /**
     * @brief Reads custom data from ByteBuffer.
     * 
     * @tparam T type of custom data
     * @param data custom data
     * @return true if operation is successful, readIndex is increased by size of custom data type
     * @return false if operation is failed, readIndex is not changed
     */
    template <typename T>
    bool read(T &data) const
    {
        const auto sz = sizeof(T);
        if (m_readIndex + sz > m_bufferSz) {
            //LOG_ERROR("Data (size: " << sz << ") cannot be read. " << (m_bufferSz - m_readIndex) << " bytes left to reach the end.");
            return false;
        }

        memcpy(&data, m_buffer + m_readIndex, sz);
        m_readIndex += sz;

        return true;
    }

    /**
     * @brief Reads custom data from ByteBuffer in opposite endian.
     * 
     * @tparam T type of custom data
     * @param data custom data
     * @return true if operation is successful, readIndex is increased by size of custom data type
     * @return false if operation is failed, readIndex is not changed
     */
    template <typename T>
    bool readSwapped(T &data) const
    {
        if (read(data)) {
            data = swapEndian(data);
            return true;
        }

        return false;
    }

    /**
     * @brief Writes custom data array into ByteBuffer.
     * 
     * @tparam T type of custom data array
     * @param data custom data array
     * @param N number of elements in a custom data array
     * @return true if operation is successful, writeIndex is increased by total size of custom data type
     * @return false if operation is failed, writeIndex is not changed
     */
    template <typename T>
    bool writeArray(const T *data, const size_t N)
    {
        const auto sz = sizeof(T) * N;
        if (m_writeIndex + sz > m_bufferSz) {
            //LOG_ERROR("Data (size: " << sz << ") cannot be written to buffer. " << (m_bufferSz - m_writeIndex) << " free bytes left. Cannot write!");
            return false;
        }

        memcpy(m_buffer + m_writeIndex, data, sz);
        m_writeIndex += sz;

        return true;
    }

    /**
     * @brief Reads custom data array from ByteBuffer.
     * 
     * @tparam T type of custom data array
     * @tparam N number of elements in a custom data array
     * @return true if operation is successful, readIndex is increased by total size of custom data type
     * @return false if operation is failed, readIndex is not changed
     */
    template <typename T, size_t N>
    bool readArray(T (&data)[N]) const
    {
        const auto sz = sizeof(T) * N;
        if (m_readIndex + sz > m_bufferSz) {
            //LOG_ERROR("Data (size: " << sz << ") cannot be read. " << (m_bufferSz - m_readIndex) << " bytes left to reach the end.");
            return false;
        }

        memcpy(&data, m_buffer + m_readIndex, sz);
        m_readIndex += sz;

        return true;
    }

    /**
     * @brief Reads N bytes into custom data array from ByteBuffer.
     * 
     * @tparam T type of custom data array
     * @tparam N number of elements in a custom data array
     * @return true if operation is successful, readIndex is increased by total size of custom data type
     * @return false if operation is failed, readIndex is not changed
     */
    template <typename T>
    bool readBytes(T *data, const size_t sz) const
    {
        if (m_readIndex + sz > m_bufferSz) {
            //LOG_ERROR("Data (size: " << sz << ") cannot be read. " << (m_bufferSz - m_readIndex) << " bytes left to reach the end.");
            return false;
        }

        memcpy(data, m_buffer + m_readIndex, sz);
        m_readIndex += sz;

        return true;
    }

    /**
     * @brief Writes string data into ByteBuffer.
     * 
     * @param data string data
     * @return true if operation is successful, writeIndex is increased by length of string
     * @return false if operation is failed, writeIndex is not changed
     */
    bool writeString(const std::string &data);

    /**
     * @brief Writes string data represented as hex into ByteBuffer.
     * 
     * @param data string data in hex format, for instance "0104fdae00a0"
     * @return true if operation is successful, writeIndex is increased by half length of string
     * @return false if operation is failed, writeIndex is not changed
     */
    bool writeHexString(const std::string &data);

    /**
     * @brief Reads string data from ByteBuffer.
     * 
     * @param data reference to string data
     * @param N length of string to be read
     * @return true if operation is successful, readIndex is increased by length of string
     * @return false if operation is failed, readIndex is not changed
     */
    bool readString(std::string &data, const size_t N) const;

    /**
     * @brief Reads N number of bytes to new ByteBuffer.
     * 
     * @param N number of bytes to be read
     * @return ByteBuffer new ByteBuffer
     */
    ByteBuffer readToByteBuffer(const size_t N) const;

    /**
     * @brief Reads N number of bytes to existing ByteBuffer.
     * 
     * @param buffer target buffer
     * @param N number of bytes to be read
     * @return true if operation is successful, readIndex is increased by length of string
     * @return false if operation is failed, readIndex is not changed
     */
    bool readToByteBuffer(ByteBuffer &buffer, const size_t N) const;

    /**
     * @brief Increases readIndex by N.
     * 
     * @param N number of bytes to skip
     * @return true if operation is successful, readIndex is increased by N
     * @return false if operation is failed, readIndex is not changed
     */
    bool skipRead(const size_t N) const;

    /**
     * @brief Increases writeIndex by N.
     * 
     * @param N number of bytes to skip
     * @return true if operation is successful, writeIndex is increased by N
     * @return false if operation is failed, writeIndex is not changed
     */
    bool skipWrite(const size_t N) const;

    /**
     * @brief Returns value of specified byte from ByteBuffer.
     * 
     * @param pos poisition of byte
     * @return uint8_t value
     */
    uint8_t at(const size_t pos) const;

    /**
     * @brief Returns pointer to memory of buffer.
     * 
     * @return uint8_t* pointer to buffer's data
     */
    uint8_t *data() const;

    /**
     * @brief Converts ByteBuffer to vector of bytes.
     * 
     * @return const std::vector<uint8_t> vector of bytes
     */
    const std::vector<uint8_t> asVector() const;

    /**
     * @brief Converts ByteBuffer to vector of uint64_t.
     * 
     * @return const std::vector<uint64_t> represents full buffer as hexed integers
     */
    const std::vector<uint64_t> asHash() const;

    /**
     * @brief Converts ByteBuffer to string in hex format.
     * 
     * @return const std::string string in hex format
     */
    const std::string asHexString() const;

    /**
     * @brief Converts ByteBuffer to string in hex format more readable for human.
     * Additionally formatted like: "[ 01 04 fd ae 00 a0 ]"
     * 
     * @return const std::string string in hex format
     */
    const std::string asHexStringFormatted() const;

    /**
     * @brief Converts ByteBuffer to string in a format readable for human.
     * Non-readable symbols are ignored.
     * 
     * @return const std::string string in readable format
     */
    const std::string asString() const;

    /**
     * @brief Returns size of ByteBuffer.
     * 
     * @return size_t 
     */
    size_t size() const;

    /**
     * @brief Returns length of data of ByteBuffer.
     * 
     * @return size_t 
     */
    size_t length() const;

    /**
     * @brief Returns remaining length of data of ByteBuffer.
     * 
     * @return size_t 
     */
    size_t remainingLength() const;

protected:
    size_t m_bufferSz = 0u;
    uint8_t *m_buffer = nullptr;
    mutable size_t m_readIndex = 0u;
    mutable size_t m_writeIndex = 0u;
};

} // namespace psi::tools
