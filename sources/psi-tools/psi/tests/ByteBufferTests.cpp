
#include <gmock/gmock.h>
#include <gtest/gtest.h>

#define protected public
#include "psi/tools/ByteBuffer.h"
#undef private

using namespace psi::tools;

TEST(ByteBufferTests, default_ctor)
{
    ByteBuffer data;
    EXPECT_EQ(data.size(), 1024u);
    EXPECT_NE(data.data(), nullptr);

    EXPECT_EQ(data.m_readIndex, 0u);
    EXPECT_EQ(data.m_writeIndex, 0u);
}

TEST(ByteBufferTests, ctor_customSize)
{
    const size_t N = 47u;
    ByteBuffer data(N);
    EXPECT_EQ(data.size(), N);
    EXPECT_NE(data.data(), nullptr);

    EXPECT_EQ(data.m_readIndex, 0u);
    EXPECT_EQ(data.m_writeIndex, 0u);
}

TEST(ByteBufferTests, ctor_RValuedData)
{
    const size_t N = 47u;
    uint8_t *buffer = new uint8_t[N] {'a', 'b'};
    ByteBuffer data(std::move(*buffer), N);
    EXPECT_EQ(data.size(), N);
    EXPECT_EQ(data.data(), buffer);
    // delete[] buffer; // generates error because object is moved into the buffer

    EXPECT_EQ(data.m_readIndex, 0u);
    EXPECT_EQ(data.m_writeIndex, N);
}

TEST(ByteBufferTests, ctor_LValuedData)
{
    const size_t N = 47u;

    {
        SCOPED_TRACE("// case 1. moved from a stack");

        uint8_t *buffer = new uint8_t[N]();
        ByteBuffer data(buffer, N);
        EXPECT_EQ(data.size(), N);
        EXPECT_EQ(data.data(), buffer);

        for (size_t i = 0; i < N; ++i) {
            EXPECT_EQ(data.at(i), buffer[i]);
        }

        EXPECT_EQ(data.m_readIndex, 0u);
        EXPECT_EQ(data.m_writeIndex, N);

        // delete[] buffer; // generates error because object is moved into the buffer
    }

    {
        SCOPED_TRACE("// case 2. moved from a temporary");

        ByteBuffer data(new uint8_t[N] {'a', 'b'}, N);
        EXPECT_EQ(data.size(), N);
        EXPECT_NE(data.data(), nullptr);

        EXPECT_EQ(data.at(0), 'a');
        EXPECT_EQ(data.at(1), 'b');

        EXPECT_EQ(data.m_readIndex, 0u);
        EXPECT_EQ(data.m_writeIndex, N);
    }
}

TEST(ByteBufferTests, dtor)
{
    const size_t N = 47u;

    {
        ByteBuffer data(new uint8_t[N] {'a', 'b'}, N);
    }
}

TEST(ByteBufferTests, ctor_copy)
{
    const size_t N = 47u;

    {
        SCOPED_TRACE("// case 1. copy to new buffer via ctor");

        ByteBuffer data1(new uint8_t[N] {'a', 'b'}, N);
        ByteBuffer data2(data1);
        EXPECT_EQ(data2.size(), N);
        EXPECT_NE(data2.data(), data1.data());

        EXPECT_EQ(data2.at(0), 'a');
        EXPECT_EQ(data2.at(1), 'b');

        EXPECT_EQ(data2.m_readIndex, data1.m_readIndex);
        EXPECT_EQ(data2.m_writeIndex, data1.m_writeIndex);
    }

    {
        SCOPED_TRACE("// case 2. copy to new buffer via operator");

        ByteBuffer data1(new uint8_t[N] {'a', 'b'}, N);
        ByteBuffer data2 = data1;
        EXPECT_EQ(data2.size(), N);
        EXPECT_NE(data2.data(), data1.data());

        EXPECT_EQ(data2.at(0), 'a');
        EXPECT_EQ(data2.at(1), 'b');

        EXPECT_EQ(data2.m_readIndex, data1.m_readIndex);
        EXPECT_EQ(data2.m_writeIndex, data1.m_writeIndex);
    }
}

TEST(ByteBufferTests, ctor_copy_operator)
{
    const size_t N = 47u;

    ByteBuffer data1(new uint8_t[N] {'b', 'a', 'c'}, N - 1);
    data1.m_readIndex = 1;

    ByteBuffer data2(new uint8_t[N] {'a', 'b'}, N);
    data2 = data1;

    EXPECT_EQ(data2.size(), N - 1);
    EXPECT_NE(data2.data(), data1.data());

    EXPECT_EQ(data2.at(0), 'b');
    EXPECT_EQ(data2.at(1), 'a');
    EXPECT_EQ(data2.at(2), 'c');

    EXPECT_EQ(data2.m_readIndex, data1.m_readIndex);
    EXPECT_EQ(data2.m_writeIndex, data1.m_writeIndex);
}

TEST(ByteBufferTests, clear)
{
    const size_t N = 47u;

    ByteBuffer data(new uint8_t[N] {'b', 'a', 'c'}, N);
    data.m_readIndex = 1;
    data.m_writeIndex = 2;

    data.clear();

    EXPECT_EQ(data.at(0), '\0');
    EXPECT_EQ(data.at(1), '\0');
    EXPECT_EQ(data.at(2), '\0');
    EXPECT_EQ(data.m_readIndex, 0u);
    EXPECT_EQ(data.m_writeIndex, 0u);
}

TEST(ByteBufferTests, reset)
{
    const size_t N = 47u;

    ByteBuffer data(new uint8_t[N] {'b', 'a', 'c'}, N);
    data.m_readIndex = 1;
    data.m_writeIndex = 2;

    data.reset();

    EXPECT_EQ(data.at(0), 'b');
    EXPECT_EQ(data.at(1), 'a');
    EXPECT_EQ(data.at(2), 'c');
    EXPECT_EQ(data.m_readIndex, 0u);
    EXPECT_EQ(data.m_writeIndex, 0u);
}

TEST(ByteBufferTests, skip)
{
    const size_t N = 3u;

    ByteBuffer data(new uint8_t[N] {'b', 'a', 'c'}, N);

    EXPECT_EQ(data.skip(2), true);
    EXPECT_EQ(data.m_readIndex, 2u);

    EXPECT_EQ(data.skip(2), false);
    EXPECT_EQ(data.m_readIndex, 2u);
}

TEST(ByteBufferTests, at)
{
    const size_t N = 3u;

    ByteBuffer data(new uint8_t[N] {'b', 'a', 'c'}, N);

    EXPECT_EQ(data.at(0), 'b');
    EXPECT_EQ(data.at(1), 'a');
    EXPECT_EQ(data.at(2), 'c');
}

TEST(ByteBufferTests, data)
{
    const size_t N = 3u;

    ByteBuffer data(new uint8_t[N] {'b', 'a', 'c'}, N);

    const auto dataPtr = data.data();
    for (size_t i = 0; i < N; ++i) {
        EXPECT_EQ(dataPtr[i], data.at(i));
    }
}

TEST(ByteBufferTests, asVector)
{
    const size_t N = 3u;

    ByteBuffer data(new uint8_t[N] {'b', 'a', 'c'}, N);

    const std::vector<uint8_t> expected {'b', 'a', 'c'};
    EXPECT_EQ(expected, data.asVector());
}

TEST(ByteBufferTests, asHash)
{
    const size_t N = 13u;

    ByteBuffer data(new uint8_t[N] {'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', '0', '1', '2', '3', '4'}, N);

    // 'a' = 0x61
    // 'b' = 0x62
    // 'c' = 0x63
    // 'd' = 0x64
    // 'e' = 0x65
    // 'f' = 0x66
    // 'g' = 0x67
    // 'h' = 0x68
    // '0' = 0x30
    // '1' = 0x31
    // '2' = 0x32
    // '3' = 0x33
    // '4' = 0x34
    // 0: uint64_t = 'h' 'g' 'f' 'e' 'd' 'c' 'b' 'a' = 0x 68 67 66 65 64 63 62 61 = 7'523'094'288'207'667'809
    // 1: uint64_t = '4' '3' '2' '1' '0' = 0x 34 33 32 31 30 = 224'197'226'800
    const std::vector<uint64_t> expected {7'523'094'288'207'667'809, 224'197'226'800};
    EXPECT_EQ(expected, data.asHash());
}

TEST(ByteBufferTests, asHexString)
{
    const size_t N = 13u;

    ByteBuffer data(new uint8_t[N] {'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', '0', '1', '2', '3', '4'}, N);

    // 'a' = 0x61
    // 'b' = 0x62
    // 'c' = 0x63
    // 'd' = 0x64
    // 'e' = 0x65
    // 'f' = 0x66
    // 'g' = 0x67
    // 'h' = 0x68
    // '0' = 0x30
    // '1' = 0x31
    // '2' = 0x32
    // '3' = 0x33
    // '4' = 0x34
    const std::string expected {"61626364656667683031323334"};
    EXPECT_EQ(expected, data.asHexString());
}

TEST(ByteBufferTests, asHexStringFormatted)
{
    const size_t N = 13u;

    ByteBuffer data(new uint8_t[N] {'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', '0', '1', '2', '3', '4'}, N);

    // 'a' = 0x61
    // 'b' = 0x62
    // 'c' = 0x63
    // 'd' = 0x64
    // 'e' = 0x65
    // 'f' = 0x66
    // 'g' = 0x67
    // 'h' = 0x68
    // '0' = 0x30
    // '1' = 0x31
    // '2' = 0x32
    // '3' = 0x33
    // '4' = 0x34
    const std::string expected {"[ 61 62 63 64 65 66 67 68 30 31 32 33 34 ]"};
    EXPECT_EQ(expected, data.asHexStringFormatted());
}

TEST(ByteBufferTests, asString)
{
    const size_t N = 13u;

    ByteBuffer data(new uint8_t[N] {'a', 'b', '\u0023', 'd', '\0', '\7', 'g', '\n', '\r', '1', '\\', '\t', '4'}, N);

    // 'a' = 0x61
    // 'b' = 0x62
    // 'c' = 0x63
    // 'd' = 0x64
    // 'e' = 0x65
    // 'f' = 0x66
    // 'g' = 0x67
    // 'h' = 0x68
    // '0' = 0x30
    // '1' = 0x31
    // '2' = 0x32
    // '3' = 0x33
    // '4' = 0x34
    const std::string expected {"ab#dg\n\r1\\\t4"};
    EXPECT_EQ(expected, data.asString());
}

TEST(ByteBufferTests, size)
{
    const size_t N = 13u;

    ByteBuffer data(new uint8_t[N] {'a', 'b', 'c'}, N);
    EXPECT_EQ(data.size(), N);
}

TEST(ByteBufferTests, writeString)
{
    const size_t N = 10u;
    ByteBuffer data(N);

    EXPECT_EQ(data.writeString("abcdefghijkl"), false);
    EXPECT_EQ(data.writeString("abcdefghij"), true);
    EXPECT_EQ(data.asString(), "abcdefghij");
}

TEST(ByteBufferTests, writeHexString)
{
    const size_t N = 10u;
    ByteBuffer data(N);

    EXPECT_EQ(data.writeHexString("abcdef010203407f7f7fff"), false);
    EXPECT_EQ(data.writeHexString("abcdef010203407f7f7"), false);
    EXPECT_EQ(data.writeHexString("abcdef0102030405067f"), true);
    EXPECT_EQ(data.asHexString(), "abcdef0102030405067f");
}

TEST(ByteBufferTests, readString)
{
    const size_t N = 10u;
    ByteBuffer data(N);
    data.writeString("abcdefghij");

    std::string str;
    EXPECT_EQ(data.readString(str, 11u), false);
    EXPECT_EQ(data.readString(str, 4u), true);
    EXPECT_EQ(str, "abcd");
}

TEST(ByteBufferTests, readToByteBuffer)
{
    const size_t N = 10u;
    ByteBuffer data(N);
    data.writeString("abcdefghij");

    ByteBuffer data2 = data.readToByteBuffer(15u);
    EXPECT_EQ(data2.size(), 0u);

    data2 = data.readToByteBuffer(5u);
    ASSERT_EQ(data2.size(), 5u);
    EXPECT_EQ(data2.asString(), "abcde");
}

TEST(ByteBufferTests, write)
{
    const size_t N = 10u;
    ByteBuffer data(N);

    // write uint16_t, success
    EXPECT_EQ(data.write(uint16_t(10u)), true);
    // write uint64_t, success
    EXPECT_EQ(data.write(uint64_t(10u)), true);
    // write uint8_t, failed because buffer is full
    EXPECT_EQ(data.write(uint8_t(10u)), false);
}

TEST(ByteBufferTests, read)
{
    const size_t N = 10u;
    ByteBuffer data(N);
    data.write(uint16_t(10u));
    data.write(uint64_t(10u));

    // read uint16_t, success
    uint16_t a;
    EXPECT_EQ(data.read(a), true);
    EXPECT_EQ(a, 10u);
    // read uint64_t, success
    uint64_t b;
    EXPECT_EQ(data.read(b), true);
    EXPECT_EQ(b, 10u);
    // read uint8_t, failed because nothing to read
    uint8_t c;
    EXPECT_EQ(data.read(c), false);
}

TEST(ByteBufferTests, writeArray)
{
    const size_t N = 10u;
    ByteBuffer data(N);

    // write uint16_t[2], success
    uint16_t a[2] {10u, 12u};
    EXPECT_EQ(data.writeArray(a, 2), true);
    // write uint64_t[1], failed because array does not match free space of buffer
    uint64_t b[1] {10u};
    EXPECT_EQ(data.writeArray(b, 1), false);
    // write uint8_t[7], failed because array does not match free space of buffer
    uint8_t c[7];
    EXPECT_EQ(data.writeArray(c, 7), false);
    // write uint8_t[6], success
    uint8_t d[6];
    EXPECT_EQ(data.writeArray(d, 6), true);
}

TEST(ByteBufferTests, readArray)
{
    const size_t N = 10u;
    ByteBuffer data(N);
    data.writeString("0123456789");

    /*
    '0' = 0x30
    '1' = 0x31
    '2' = 0x32
    '3' = 0x33
    '4' = 0x34
    '5' = 0x35
    '6' = 0x36
    '7' = 0x37
    '8' = 0x38
    '9' = 0x39
    */

    // read uint8_t[3], success
    uint8_t a[3];
    EXPECT_EQ(data.readArray(a), true);
    EXPECT_EQ(a[0], '0');
    EXPECT_EQ(a[1], '1');
    EXPECT_EQ(a[2], '2');
    // read uint16_t[3], success
    uint16_t b[3];
    EXPECT_EQ(data.readArray(b), true);
    EXPECT_EQ(b[0], 13363); // 0x34 << 8 | 0x33
    EXPECT_EQ(b[1], 13877); // 0x36 << 8 | 0x35
    EXPECT_EQ(b[2], 14391); // 0x38 << 8 | 0x37
    // read uint8_t[2], failed because requsted data size is larger than left for reading in buffer
    uint8_t c[2];
    EXPECT_EQ(data.readArray(c), false);
}
