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

template <std::size_t MAX_LENGTH = 10, std::size_t CONCAT_LENGTH = 3, std::size_t ITERATIONS = 1'000>
void ConcatStringsBM(benchmark::State &state) {
    for (auto _ : state) {
        const String s1(CONCAT_LENGTH, '.');
        String s2;

        for (std::size_t i = 0; i < ITERATIONS; ++i) {
            s2 += s1;
            if (s2.length() > MAX_LENGTH) {
                s2 = "";
            }
        }
    }
}

}  // namespace

BENCHMARK(ConcatStringsBM<10, 3, 1'000>);  // Short strings
BENCHMARK(ConcatStringsBM<120, 3, 1'000>);  // Longer strings
BENCHMARK(ConcatStringsBM<2'000'000, 10'000, 1'000>);  // Long strings

BENCHMARK_MAIN();