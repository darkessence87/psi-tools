#include "psi/tools/BigInteger.h"

#include <iostream>
#include <stack>

using namespace psi::tools;

BigInteger fib_stack(uint64_t n);

BigInteger fib_stack(uint64_t n)
{
    if (n < 2) {
        return n < 1 ? BigInteger(0) : BigInteger(1);
    }

    BigInteger result(0);

    std::stack<std::pair<BigInteger, BigInteger>> st;
    st.push({BigInteger(0), BigInteger(1)});
    uint64_t k = 2;
    while (!st.empty()) {
        auto p = st.top();
        st.pop();

        result = p.first + p.second;
        if (k < n) {
            st.push({p.second, result});
        }

        std::cout << "[" << k << "] " << result.toString() << "= " << p.first.toString() << "+ "
                  << p.second.toString() << std::endl;

        ++k;
    }

    return result;
}

int main()
{
    std::cout << fib_stack(300).toString() << std::endl;
}
