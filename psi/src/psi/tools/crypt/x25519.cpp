#include "x25519.h"

#include "psi/tools/Encryptor.h"

namespace psi::tools::crypt {

static constexpr std::array<uint8_t, 32> _9 = {9};
const x25519::field_elem x25519::_121665 = {0xdb41, 1};

void x25519::unpack25519(field_elem &out, const uint8_t *in)
{
    for (uint8_t i = 0; i < 16; ++i) {
        out[i] = int64_t(in[0]) | (int64_t(*shift_ptr(in, 1)) << 8);
        in = shift_ptr(in, 2);
    }
    out[15] &= 0x7fff;
}

void x25519::carry25519(field_elem &elem)
{
    int64_t carry = 0;
    for (uint8_t i = 0; i < 16; ++i) {
        carry = elem[i] >> 16;
        elem[i] -= carry << 16;
        if (i < 15) {
            elem[i + 1] += carry;
        } else {
            elem[0] += 38 * carry;
        }
    }
}

/* out = a + b */
void x25519::fadd(field_elem &out, const field_elem &a, const field_elem &b)
{
    for (uint8_t i = 0; i < 16; ++i) {
        out[i] = a[i] + b[i];
    }
}

/* out = a - b */
void x25519::fsub(field_elem &out, const field_elem &a, const field_elem &b)
{
    for (uint8_t i = 0; i < 16; ++i) {
        out[i] = a[i] - b[i];
    }
}

/* out = a * b */
void x25519::fmul(field_elem &out, const field_elem &a, const field_elem &b)
{
    std::array<int64_t, 31> product;
    for (uint8_t i = 0; i < 31; ++i) {
        product[i] = 0;
    }
    for (uint8_t i = 0; i < 16; ++i) {
        for (uint8_t j = 0; j < 16; ++j) {
            product[i + j] += a[i] * b[j];
        }
    }
    for (uint8_t i = 0; i < 15; ++i) {
        product[i] += 38 * product[i + 16];
    }
    for (uint8_t i = 0; i < 16; ++i) {
        out[i] = product[i];
    }
    carry25519(out);
    carry25519(out);
}

void x25519::finverse(field_elem &out, const field_elem &in)
{
    field_elem c;
    for (uint8_t i = 0; i < 16; ++i) {
        c[i] = in[i];
    }
    for (int16_t i = 253; i >= 0; i--) {
        fmul(c, c, c);
        if (i != 2 && i != 4) {
            fmul(c, c, in);
        }
    }
    for (uint8_t i = 0; i < 16; ++i) {
        out[i] = c[i];
    }
}

void x25519::swap25519(field_elem &p, field_elem &q, int64_t bit)
{
    const int64_t c = ~(bit - 1);
    int64_t t;
    for (uint8_t i = 0; i < 16; ++i) {
        t = c & (p[i] ^ q[i]);
        p[i] ^= t;
        q[i] ^= t;
    }
}

void x25519::pack25519(uint8_t *out, const field_elem &in)
{
    int carry;
    field_elem m, t;
    for (uint8_t i = 0; i < 16; ++i) {
        t[i] = in[i];
    }
    carry25519(t);
    carry25519(t);
    carry25519(t);
    for (uint8_t j = 0; j < 2; ++j) {
        m[0] = t[0] - 0xffed;
        for (uint8_t i = 1; i < 15; i++) {
            m[i] = t[i] - 0xffff - ((m[i - 1] >> 16) & 1);
            m[i - 1] &= 0xffff;
        }
        m[15] = t[15] - 0x7fff - ((m[14] >> 16) & 1);
        carry = (m[15] >> 16) & 1;
        m[14] &= 0xffff;
        swap25519(t, m, 1 - carry);
    }
    for (uint8_t i = 0; i < 16; ++i) {
        *out = t[i] & 0xff;
        *shift_ptr(out, 1) = static_cast<uint8_t>(t[i] >> 8);
        out = shift_ptr(out, 2);
    }
}

void x25519::scalarmult_base(uint8_t *out, const uint8_t *scalar)
{
    scalarmult(out, scalar, _9.data());
}

void x25519::generate_keypair(uint8_t *pk, uint8_t *sk)
{
    mem_copy(sk, 0, Encryptor::generateSessionKey().data(), 0, 32u);
    scalarmult_base(pk, sk);
}

void x25519::scalarmult(uint8_t *out, const uint8_t *scalar, const uint8_t *point)
{
    std::array<uint8_t, 32> clamped;
    field_elem a, b, c, d, e, f, x;
    for (uint8_t i = 0; i < 32; ++i) {
        clamped[i] = *shift_ptr(scalar, i);
    }
    clamped[0] &= 0xf8;
    clamped[31] = (clamped[31] & 0x7f) | 0x40;
    unpack25519(x, point);
    for (uint8_t i = 0; i < 16; ++i) {
        b[i] = x[i];
        d[i] = a[i] = c[i] = 0;
    }
    a[0] = d[0] = 1;

    int64_t bit;
    for (int16_t i = 254; i >= 0; --i) {
        bit = (clamped[size_t(i) >> 3] >> (i & 7)) & 1;
        swap25519(a, b, bit);
        swap25519(c, d, bit);
        fadd(e, a, c);
        fsub(a, a, c);
        fadd(c, b, d);
        fsub(b, b, d);
        fmul(d, e, e);
        fmul(f, a, a);
        fmul(a, c, a);
        fmul(c, b, e);
        fadd(e, a, c);
        fsub(a, a, c);
        fmul(b, a, a);
        fsub(c, d, f);
        fmul(a, c, _121665);
        fadd(a, a, d);
        fmul(c, c, a);
        fmul(a, d, f);
        fmul(d, b, x);
        fmul(b, e, e);
        swap25519(a, b, bit);
        swap25519(c, d, bit);
    }
    finverse(c, c);
    fmul(a, a, c);
    pack25519(out, a);
}

} // namespace psi::tools::crypt
