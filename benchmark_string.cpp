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

void ShortStringBM(benchmark::State &state) {
    for (auto _ : state) {
        const String s1(3, '.');
        String s2;

        constexpr std::size_t ITERATIONS = 1e3;
        constexpr std::size_t MAX_LENGTH = 10;

        for (std::size_t i = 0; i < ITERATIONS; ++i) {
            s2 += s1;
            if (s2.length() > MAX_LENGTH) {
                s2 = "";
            }
        }
    }
}

void LongerStringBM(benchmark::State &state) {
    for (auto _ : state) {
        const String s1(3, '.');
        String s2;

        constexpr std::size_t ITERATIONS = 1e3;
        constexpr std::size_t MAX_LENGTH = 120;

        for (std::size_t i = 0; i < ITERATIONS; ++i) {
            s2 += s1;
            if (s2.length() > MAX_LENGTH) {
                s2 = "";
            }
        }
    }
}

void LongStringBM(benchmark::State &state) {
    for (auto _ : state) {
        const String s1(10'000, '.');
        String s2;

        constexpr std::size_t ITERATIONS = 1e3;
        constexpr std::size_t MAX_LENGTH = 2e6;

        for (std::size_t i = 0; i < ITERATIONS; ++i) {
            s2 += s1;
            if (s2.length() > MAX_LENGTH) {
                s2 = "";
            }
        }
    }
}

}  // namespace

BENCHMARK(ShortStringBM);
BENCHMARK(LongerStringBM);
BENCHMARK(LongStringBM);

BENCHMARK_MAIN();