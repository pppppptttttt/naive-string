/*
 * Expectations: approximately same result if SSO_BUFFER_SIZE are equal,
 * faster string operations if SSO_BUFFER_SIZE greater than from libstdc++
 * for longer strings while they can fit to sso buffer.
 */

#include <benchmark/benchmark.h>
#include <iostream>

#ifdef TEST_STL_STRING
#include <string>
using String = std::string;
#else
#include "naive_string.hpp"
#endif

namespace {

struct Dummy {
    Dummy() {
#ifdef TEST_STL_STRING
        std::cout << "STL string benchmarks" << std::endl;
#else
        std::cout << "Naive string benchmarks" << std::endl;
#endif
    }
} dummy;  // NOLINT

template <
    std::size_t MAX_LENGTH = 10,
    std::size_t CONCAT_LENGTH = 3,
    std::size_t ITERATIONS = 1'000'000>
void ConcatStringsBM(benchmark::State &state) {
    for (auto _ : state) {
        const String s1(CONCAT_LENGTH, '.');
        String s2;

        for (std::size_t i = 0; i < ITERATIONS; ++i) {
            if (s2.length() > MAX_LENGTH) {
                s2.clear();
            }
            benchmark::DoNotOptimize(s2 += s1);
        }
    }
}

}  // namespace

BENCHMARK(ConcatStringsBM<>);                   // Short strings
BENCHMARK(ConcatStringsBM<120, 20>);            // Longer strings
BENCHMARK(ConcatStringsBM<2'000'000, 10'000>);  // Long strings

BENCHMARK_MAIN();