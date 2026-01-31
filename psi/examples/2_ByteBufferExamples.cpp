#include "psi/tools/ByteBuffer.h"

#include <iostream>

int main()
{
    using namespace psi;

    tools::ByteBuffer buf(255);
    buf.writeString("Hello world!");
    buf.writeHexString("aabbccddeeff00010203040506070809");
    buf.write(uint64_t(1024));
    uint16_t arr[8] {1, 2, 3, 4, 5, 6, 7, 8};
    buf.writeArray(arr, 8);

    std::cout << buf.asHexStringFormatted() << std::endl;
}
