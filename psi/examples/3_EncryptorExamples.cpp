#include "psi/tools/Encryptor.h"

#include <iostream>

int main()
{
    using namespace psi;

    const auto key = tools::Encryptor::generateSessionKey();

    tools::ByteBuffer message(512);
    message.writeString("In a world where technology had advanced beyond our \
wildest dreams, one programming language stood head and shoulders above the \
rest: C++. This powerful language had become the backbone of modern society, \
driving everything from the smartphones in our pockets to the cars we drove on \
the roads.");
    std::cout << message.asString() << std::endl << std::endl;
    std::cout << message.asHexString() << std::endl << std::endl;

    const auto encryptedMessage = tools::Encryptor::encryptAes256(message, key);
    std::cout << encryptedMessage.asHexString() << std::endl << std::endl;

    const auto decryptedMessage = tools::Encryptor::decryptAes256(encryptedMessage, key);
    std::cout << decryptedMessage.asHexString() << std::endl << std::endl;
    std::cout << decryptedMessage.asString() << std::endl << std::endl;
}