#include "psi/tools/BigInteger.h"

#include <sstream>

namespace psi::tools {

BigInteger::BigInteger()
    : BigInteger(0u)
{
}

BigInteger::BigInteger(uint64_t v)
    : m_value({v})
{
}

BigInteger::BigInteger(const std::vector<uint64_t> &v)
{
    size_t endIndex = 0;
    for (size_t k = v.size(); k > 0; --k) {
        if (v[k - 1] != 0) {
            endIndex = k - 1;
            break;
        }
    }

    m_value = std::vector<uint64_t>(v.begin(), v.begin() + endIndex + 1);
}

BigInteger BigInteger::operator+(const BigInteger &i) const
{
    auto ai = *this;
    const auto bi = i;
    for (size_t k = 0; k < bi.m_value.size(); ++k) {
        ai.add(bi.m_value[k], k);
    }
    return ai;
}

BigInteger BigInteger::operator+(uint64_t i) const
{
    auto ai = *this;
    ai.add(i, 0);
    return ai;
}

BigInteger &BigInteger::operator+=(const BigInteger &i)
{
    const auto bi = i;
    for (size_t k = 0; k < bi.m_value.size(); ++k) {
        add(bi.m_value[k], k);
    }
    return *this;
}

BigInteger &BigInteger::operator+=(uint64_t i)
{
    add(i, 0);
    return *this;
}

BigInteger BigInteger::operator-(const BigInteger &i) const
{
    auto ai = *this;
    const auto bi = i;
    for (size_t k = 0; k < bi.m_value.size(); ++k) {
        ai.substract(bi.m_value[k], k);
    }
    return ai;
}

BigInteger BigInteger::operator-(uint64_t i) const
{
    auto ai = *this;
    ai.substract(i, 0);
    return ai;
}

BigInteger &BigInteger::operator-=(const BigInteger &i)
{
    const auto bi = i;
    for (size_t k = 0; k < bi.m_value.size(); ++k) {
        substract(bi.m_value[k], k);
    }
    return *this;
}

BigInteger &BigInteger::operator-=(uint64_t i)
{
    substract(i, 0);
    return *this;
}

std::string BigInteger::toString() const
{
    std::ostringstream os;

    for (size_t k = m_value.size(); k > 0; --k) {
        os << std::to_string(m_value[k - 1]) << " ";
    }

    return os.str();
}

void BigInteger::add(uint64_t i, uint64_t index)
{
    while (m_value.size() - 1 < index) {
        m_value.emplace_back(0u);
    }
    const uint64_t newValue = m_value[index] + i;
    bool isCarry = newValue < m_value[index] && newValue < i;

    if (!isCarry) {
        m_value[index] = newValue;
        return;
    }

    m_value[index] = newValue;
    if (m_value.size() == index + 1) {
        m_value.emplace_back(1u);
        return;
    }

    for (uint64_t k = index + 1; k < m_value.size(); ++k) {
        if (m_value[k] == 0xffffffffffffffff) {
            if (k == m_value.size() - 1) {
                m_value.emplace_back(0u);
                m_value[k] = 0u;
            }
        } else {
            ++m_value[k];
            break;
        }
    }
}

void BigInteger::substract(uint64_t i, uint64_t index)
{
    const uint64_t newValue = m_value[index] - i;
    bool isCarry = m_value[index] < i;

    m_value[index] = newValue;
    if (isCarry) {
        for (uint64_t k = index + 1; k < m_value.size(); ++k) {
            if (m_value[k] == 0) {
                --m_value[k];
            } else {
                --m_value[k];
                break;
            }
        }
    }

    size_t endIndex = 0;
    for (size_t k = m_value.size(); k > 0; --k) {
        if (m_value[k - 1] != 0) {
            endIndex = k - 1;
            m_value = std::vector<uint64_t>(m_value.begin(), m_value.begin() + endIndex + 1);
            break;
        }
    }
}

} // namespace psi::tools