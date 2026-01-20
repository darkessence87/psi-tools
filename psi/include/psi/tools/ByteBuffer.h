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
class ByteBuffer final
{
public:
    /**
     * @brief Construct a new ByteBuffer object. Default size: 0 bytes.
     * 
     */
    ByteBuffer();

    /**
     * @brief Construct a new ByteBuffer object using provided size.
     * 
     * @param sz number of bytes
     */
    explicit ByteBuffer(size_t sz);

    /**
     * @brief Construct a new ByteBuffer object by moving an existing array into ByteBuffer as r-value.
     * 
     * @param arr existing array
     * @param sz size of existing array
     */
    explicit ByteBuffer(uint8_t &&arr, size_t sz);

    /**
     * @brief Construct a new ByteBuffer object by moving an existing array into ByteBuffer as l-value.
     * 
     * @param arr existing array
     * @param sz size of existing array
     */
    explicit ByteBuffer(uint8_t *arr, size_t sz);

    /**
     * @brief Construct a new ByteBuffer object by copying an existing array into ByteBuffer.
     * 
     * @param arr existing array
     * @param sz size of existing array
     */
    explicit ByteBuffer(const uint8_t *arr, size_t sz);

    /**
     * @brief Construct a new ByteBuffer object by string
     * 
     * @param data string
     * @param isHex if 'true' buffer interprets data as hex input
     * default value: false
     */
    ByteBuffer(const std::string &data, bool isHex = false);

    /**
     * @brief Construct a new ByteBuffer object from another ByteBuffer object.
     * 
     * @param bb another byte buffer
     */
    ByteBuffer(const ByteBuffer &bb);

    /**
     * @brief Destroy the ByteBuffer object. Free's memory.
     * 
     */
    ~ByteBuffer();

    /**
     * @brief Replace existing ByteBuffer object with provided ByteBuffer object.
     * 
     * @param bb another byte buffer
     * @return ByteBuffer& reference to existing ByteBuffer object
     */
    ByteBuffer &operator=(const ByteBuffer &bb);

    /**
     * @brief Append existing ByteBuffer object by another ByteBuffer object.
     * 
     * @param bb another byte buffer
     * @return ByteBuffer& reference to existing ByteBuffer object
     */
    ByteBuffer &operator+=(const ByteBuffer &bb);

    /**
     * @brief Concatenate one ByteBuffer object with another ByteBuffer object.
     * 
     * @param bb another byte buffer
     * @return ByteBuffer new concatenated ByteBuffer object
     */
    ByteBuffer operator+(const ByteBuffer &bb) const;

    /**
     * @brief Clear the data without freeing or resizing the memory.
     * Set the buffer to default state. ByteBuffer might be read/write like after first creation.
     * 
     */
    void clear();

    /**
     * @brief Set the buffer to default state. ByteBuffer might be read/write like after first creation.
     * This method does not change the data already written to buffer.
     * 
     */
    void reset() const;

    /**
     * @brief Set read index to default value. ByteBuffer might be read like after first creation.
     * This method does not change the data already written to buffer.
     * 
     */
    void resetRead() const;

    /**
     * @brief Set write index to default value. ByteBuffer might be written like after first creation.
     * This method does not change the data already written to buffer.
     * 
     */
    void resetWrite() const;

    /**
     * @brief Write custom data into ByteBuffer.
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

        std::memcpy(m_buffer + m_writeIndex, &data, sz);
        m_writeIndex += sz;

        return true;
    }

    /**
     * @brief Write custom data into ByteBuffer in opposite endian.
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
     * @brief Read custom data from ByteBuffer.
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

        std::memcpy(&data, m_buffer + m_readIndex, sz);
        m_readIndex += sz;

        return true;
    }

    /**
     * @brief Read custom data from ByteBuffer in opposite endian.
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
     * @brief Write custom data array into ByteBuffer.
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

        std::memcpy(m_buffer + m_writeIndex, data, sz);
        m_writeIndex += sz;

        return true;
    }

    /**
     * @brief Read custom data array from ByteBuffer.
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

        std::memcpy(&data, m_buffer + m_readIndex, sz);
        m_readIndex += sz;

        return true;
    }

    /**
     * @brief Read N bytes into custom data array from ByteBuffer.
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

        std::memcpy(data, m_buffer + m_readIndex, sz);
        m_readIndex += sz;

        return true;
    }

    /**
     * @brief Write string data into ByteBuffer.
     * 
     * @param data string data
     * @return true if operation is successful, writeIndex is increased by length of string
     * @return false if operation is failed, writeIndex is not changed
     */
    bool writeString(const std::string &data);

    /**
     * @brief Write string data represented as hex into ByteBuffer.
     * 
     * @param data string data in hex format, for instance "0104fdae00a0"
     * @return true if operation is successful, writeIndex is increased by half length of string
     * @return false if operation is failed, writeIndex is not changed
     */
    bool writeHexString(const std::string &data);

    /**
     * @brief Read string data from ByteBuffer.
     * 
     * @param data reference to string data
     * @param N length of string to be read
     * @return true if operation is successful, readIndex is increased by length of string
     * @return false if operation is failed, readIndex is not changed
     */
    bool readString(std::string &data, const size_t N) const;

    /**
     * @brief Read string data from ByteBuffer until any of delimiters is met.
     * 
     * @param data reference to string data
     * @param delimiters list of delimiters. Default: {0x0a, 0x0d, 0x17}
     * @param N number of delimiters in list. Default: 0
     * @return true if operation is successful, readIndex is increased by length of string
     * @return false if operation is failed, readIndex is not changed
     */
    bool readLine(std::string &data, const uint8_t *delimiters = nullptr, size_t N = 0) const;

    /**
     * @brief Read N number of bytes to new ByteBuffer.
     * 
     * @param N number of bytes to be read
     * @return ByteBuffer new ByteBuffer
     */
    ByteBuffer readToByteBuffer(const size_t N) const;

    /**
     * @brief Read N number of bytes to existing ByteBuffer.
     * 
     * @param buffer target buffer
     * @param N number of bytes to be read
     * @return true if operation is successful, readIndex is increased by length of string
     * @return false if operation is failed, readIndex is not changed
     */
    bool readToByteBuffer(ByteBuffer &buffer, const size_t N) const;

    /**
     * @brief Read all bytes to existing ByteBuffer.
     * 
     * @param buffer target buffer
     * @return true if operation is successful, readIndex is increased by length of string
     * @return false if operation is failed, readIndex is not changed
     */
    bool readToByteBuffer(ByteBuffer &buffer) const;

    /**
     * @brief Increase readIndex by N.
     * 
     * @param N number of bytes to skip
     * @return true if operation is successful, readIndex is increased by N
     * @return false if operation is failed, readIndex is not changed
     */
    bool skipRead(const size_t N) const;

    /**
     * @brief Increase writeIndex by N.
     * 
     * @param N number of bytes to skip
     * @return true if operation is successful, writeIndex is increased by N
     * @return false if operation is failed, writeIndex is not changed
     */
    bool skipWrite(const size_t N) const;

    /**
     * @brief Return value of specified byte from ByteBuffer.
     * 
     * @param pos poisition of byte
     * @return uint8_t value
     */
    uint8_t at(const size_t pos) const;

    /**
     * @brief Return pointer to memory of buffer.
     * 
     * @return uint8_t* pointer to buffer's data
     */
    uint8_t *data() const;

    /**
     * @brief Convert ByteBuffer to vector of bytes.
     * 
     * @return const std::vector<uint8_t> vector of bytes
     */
    const std::vector<uint8_t> asVector() const;

    /**
     * @brief Convert ByteBuffer to vector of uint64_t.
     * 
     * @return const std::vector<uint64_t> represents full buffer as hexed integers
     */
    const std::vector<uint64_t> asHash() const;

    /**
     * @brief Convert ByteBuffer to string in hex format.
     * 
     * @return const std::string string in hex format
     */
    const std::string asHexString() const;

    /**
     * @brief Convert ByteBuffer to string in hex format more readable for human.
     * Additionally formatted like: "[ 01 04 fd ae 00 a0 ]"
     * 
     * @return const std::string string in hex format
     */
    const std::string asHexStringFormatted() const;

    /**
     * @brief Convert ByteBuffer to string in a format readable for human.
     * Non-readable symbols are ignored.
     * 
     * @return const std::string string in readable format
     */
    const std::string asString() const;

    /**
     * @brief Return size of ByteBuffer.
     * Size: memory allocated for buffer.
     * 
     * @return size_t memory allocated for buffer
     */
    size_t size() const;

    /**
     * @brief Return length of data of ByteBuffer.
     * Length: number of bytes written to the buffer.
     * 
     * @return size_t number of bytes written to the buffer
     */
    size_t length() const;

    /**
     * @brief Return remaining length of data of ByteBuffer.
     * Remaining length: number of bytes can be read from buffer.
     * 
     * @return size_t number of bytes can be read from buffer
     */
    size_t remainingLength() const;

protected:
    size_t m_bufferSz = 0u;
    uint8_t *m_buffer = nullptr;
    mutable size_t m_readIndex = 0u;
    mutable size_t m_writeIndex = 0u;
};

} // namespace psi::tools
