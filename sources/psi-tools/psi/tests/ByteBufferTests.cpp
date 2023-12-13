
#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "psi/tools/ByteBuffer.h"

using namespace psi::tools;

TEST(ByteBufferTests, CopyBuffer)
{
    ByteBuffer buffer1;
    ByteBuffer buffer2 = buffer1;
    EXPECT_EQ(sizeof(buffer1), sizeof(buffer2));

    ByteBuffer buffer3(buffer2);
    EXPECT_EQ(sizeof(buffer2), sizeof(buffer3));
}

TEST(ByteBufferTests, ReadWriteBuffer)
{
    { /// int
        ByteBuffer buffer;
        int expectedData = 255;
        buffer.write(expectedData);
        int data;
        buffer.read(data);
        EXPECT_EQ(data, expectedData);
    }

    { /// string
        ByteBuffer buffer;
        const char *expectedData = "255";
        buffer.write(expectedData);
        const char *data;
        buffer.read(data);
        EXPECT_EQ(data, expectedData);
    }

    { /// struct
        struct A {
            int value = 255;

            bool operator==(const A &right) const
            {
                return value == right.value;
            }
        };

        ByteBuffer buffer;
        A expectedData;
        expectedData.value = 1500;
        buffer.write(expectedData);
        A data;
        buffer.read(data);
        EXPECT_EQ(data, expectedData);
    }
}