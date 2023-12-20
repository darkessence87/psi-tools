
#include "EncryptorAes.hpp"

namespace psi::tools::crypt {

// AES-128
template ByteBuffer EncryptorAes::encryptAes_impl<4u, 10u>(const ByteBuffer &, const ByteBuffer &);
template ByteBuffer EncryptorAes::decryptAes_impl<4u, 10u>(const ByteBuffer &, const ByteBuffer &);
template void EncryptorAes::generateSubKeys_impl<4u, 10u>(uint8_t[4u * 4u], SubKey[10u + 1u]);
// AES-256
template ByteBuffer EncryptorAes::encryptAes_impl<8u, 14u>(const ByteBuffer &, const ByteBuffer &);
template ByteBuffer EncryptorAes::decryptAes_impl<8u, 14u>(const ByteBuffer &, const ByteBuffer &);
template void EncryptorAes::generateSubKeys_impl<8u, 14u>(uint8_t[8u * 4u], SubKey[14u + 1u]);

} // namespace psi::tools::crypt