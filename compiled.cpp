#include <iostream>
#include <string>
#include <fstream>
#include "Builtins.hpp"

int factorial(int n);
int fibonacci(int n);

int factorial(int n) {
    if ((n <= 1)) {
    return 1;

    }
    return (n * factorial((n - 1)));

}

int fibonacci(int n) {
    if ((n <= 1)) {
    return n;

    }
    auto a = 0;
    auto b = 1;
    auto fib = 0;
    auto j = 2;
    while ((j <= n)) {
    fib = (a + b);
    a = b;
    b = fib;
    j = (j + 1);

    }
    return fib;

}

int main() {
    std::cout << factorial(5) << std::endl;
    auto sum = 0;
    auto i = 1;
    while ((i <= 10)) {
    sum = (sum + i);
    i = (i + 1);

    }
    std::cout << sum << std::endl;
    std::cout << fibonacci(10) << std::endl;
    return 0;
}
