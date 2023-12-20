#include "Encryptor_x25519.h"

#include "psi/tools/Encryptor.h"

namespace psi::tools::crypt {

static const uint8_t _9[32] = {9};
const Encryptor_x25519::field_elem Encryptor_x25519::_121665 = {0xDB41, 1};

void Encryptor_x25519::unpack25519(field_elem out, const uint8_t *in)
{
    int i;
    for (i = 0; i < 16; ++i)
        out[i] = in[2 * i] + ((int64_t)in[2 * i + 1] << 8);
    out[15] &= 0x7fff;
}

void Encryptor_x25519::carry25519(field_elem elem)
{
    int i;
    int64_t carry;
    for (i = 0; i < 16; ++i) {
        carry = elem[i] >> 16;
        elem[i] -= carry << 16;
        if (i < 15)
            elem[i + 1] += carry;
        else
            elem[0] += 38 * carry;
    }
}

void Encryptor_x25519::fadd(field_elem out, const field_elem a, const field_elem b) /* out = a + b */
{
    int i;
    for (i = 0; i < 16; ++i)
        out[i] = a[i] + b[i];
}

void Encryptor_x25519::fsub(field_elem out, const field_elem a, const field_elem b) /* out = a - b */
{
    int i;
    for (i = 0; i < 16; ++i)
        out[i] = a[i] - b[i];
}

void Encryptor_x25519::fmul(field_elem out, const field_elem a, const field_elem b) /* out = a * b */
{
    int64_t i, j, product[31];
    for (i = 0; i < 31; ++i)
        product[i] = 0;
    for (i = 0; i < 16; ++i) {
        for (j = 0; j < 16; ++j)
            product[i + j] += a[i] * b[j];
    }
    for (i = 0; i < 15; ++i)
        product[i] += 38 * product[i + 16];
    for (i = 0; i < 16; ++i)
        out[i] = product[i];
    carry25519(out);
    carry25519(out);
}

void Encryptor_x25519::finverse(field_elem out, const field_elem in)
{
    field_elem c;
    int i;
    for (i = 0; i < 16; ++i)
        c[i] = in[i];
    for (i = 253; i >= 0; i--) {
        fmul(c, c, c);
        if (i != 2 && i != 4)
            fmul(c, c, in);
    }
    for (i = 0; i < 16; ++i)
        out[i] = c[i];
}

void Encryptor_x25519::swap25519(field_elem p, field_elem q, int64_t bit)
{
    int64_t t, i, c = ~(bit - 1);
    for (i = 0; i < 16; ++i) {
        t = c & (p[i] ^ q[i]);
        p[i] ^= t;
        q[i] ^= t;
    }
}

void Encryptor_x25519::pack25519(uint8_t *out, const field_elem in)
{
    int i, j, carry;
    field_elem m, t;
    for (i = 0; i < 16; ++i)
        t[i] = in[i];
    carry25519(t);
    carry25519(t);
    carry25519(t);
    for (j = 0; j < 2; ++j) {
        m[0] = t[0] - 0xffed;
        for (i = 1; i < 15; i++) {
            m[i] = t[i] - 0xffff - ((m[i - 1] >> 16) & 1);
            m[i - 1] &= 0xffff;
        }
        m[15] = t[15] - 0x7fff - ((m[14] >> 16) & 1);
        carry = (m[15] >> 16) & 1;
        m[14] &= 0xffff;
        swap25519(t, m, 1 - carry);
    }
    for (i = 0; i < 16; ++i) {
        out[2 * i] = t[i] & 0xff;
        out[2 * i + 1] = static_cast<uint8_t>(t[i] >> 8);
    }
}

void Encryptor_x25519::scalarmult_base(uint8_t *out, const uint8_t *scalar)
{
    scalarmult(out, scalar, _9);
}

void Encryptor_x25519::generate_keypair(uint8_t *pk, uint8_t *sk)
{
    memcpy(sk, Encryptor::generateSessionKey().data(), 32);
    scalarmult_base(pk, sk);
}

void Encryptor_x25519::scalarmult(uint8_t *out, const uint8_t *scalar, const uint8_t *point)
{
    uint8_t clamped[32];
    int64_t bit, i;
    field_elem a, b, c, d, e, f, x;
    for (i = 0; i < 32; ++i)
        clamped[i] = scalar[i];
    clamped[0] &= 0xf8;
    clamped[31] = (clamped[31] & 0x7f) | 0x40;
    unpack25519(x, point);
    for (i = 0; i < 16; ++i) {
        b[i] = x[i];
        d[i] = a[i] = c[i] = 0;
    }
    a[0] = d[0] = 1;
    for (i = 254; i >= 0; --i) {
        bit = (clamped[i >> 3] >> (i & 7)) & 1;
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