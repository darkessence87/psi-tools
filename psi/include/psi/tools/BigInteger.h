#pragma once

#include <stdint.h>
#include <string>
#include <vector>

namespace psi::tools {

class BigInteger
{
public:
    BigInteger();
    explicit BigInteger(uint64_t v);
    explicit BigInteger(const std::vector<uint64_t> &v);

    BigInteger operator+(const BigInteger &i) const;
    BigInteger operator+(uint64_t i) const;
    BigInteger &operator+=(const BigInteger &i);
    BigInteger &operator+=(uint64_t i);

    BigInteger operator-(const BigInteger &i) const;
    BigInteger operator-(uint64_t i) const;
    BigInteger &operator-=(const BigInteger &i);
    BigInteger &operator-=(uint64_t i);

    std::string toString() const;

// private:
    void add(uint64_t i, uint64_t index);
    void substract(uint64_t i, uint64_t index);

private:
    // lower index = lower bits
    std::vector<uint64_t> m_value;
};

} // namespace psi::tools