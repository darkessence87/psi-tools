#pragma once

#include <array>
#include <stdint.h>

namespace psi::tools::crypt {

class x25519
{
public:
    static void generate_keypair(uint8_t *pk, uint8_t *sk);
    static void scalarmult_base(uint8_t *out, const uint8_t *sk);
    static void scalarmult(uint8_t *out, const uint8_t *sk, const uint8_t *pk);

    using field_elem = std::array<int64_t, 16>;
    static void unpack25519(field_elem& out, const uint8_t *in);
    static inline void carry25519(field_elem& elem);
    static inline void fadd(field_elem& out, const field_elem& a, const field_elem& b);
    static inline void fsub(field_elem& out, const field_elem& a, const field_elem& b);
    static void fmul(field_elem& out, const field_elem& a, const field_elem& b);
    static void finverse(field_elem& out, const field_elem& in);
    static inline void swap25519(field_elem& p, field_elem& q, int64_t bit);
    static void pack25519(uint8_t *out, const field_elem& in);

private:
    static const field_elem _121665;
};

} // namespace psi::tools::crypt
