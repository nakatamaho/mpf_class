#include "gmpxx_mkII.h"
#include <iostream>

int main() {
    gmp::mpf_class num1(1.5);
    gmp::mpf_class num2(2.5);
    gmp::mpf_class result = num1 + num2;
    std::cout << "Result of 1.5 + 2.5 = " << result << std::endl;
    return 0;
}