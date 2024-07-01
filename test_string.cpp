#include <gtest/gtest.h>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <utility>

// #define TEST_STL_STRING

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
        std::cout << "STL string tests" << std::endl;
#else
        std::cout << "Naive string tests" << std::endl;
#endif
    }
} dummy;  // NOLINT

}

TEST(String, Constructors) {
    // default constructor
    const String s;
    EXPECT_EQ(s.length(), 0);
    EXPECT_EQ(s.size(), 0);
    EXPECT_EQ(s, "");

    // small string from literal and count, char pair
    String s1(6, '*');
    const String s2 = "******";
    EXPECT_EQ(s1, s2);

    // move ctor from small string
    String s3 = std::move(s1);
    EXPECT_EQ(s3, s2);

    // move assignment from small string
    s1 = std::move(s3);
    EXPECT_EQ(s1, s2);

    // copy assignment from small string
    s3 = s2;
    EXPECT_EQ(s3, s2);

    // long string ctor
    const String s4(10'000, '#');

    // long string copy assignment
    s1 = s4;
    EXPECT_EQ(s1, s4);

    // long string copy constructor
    String s5(s1);
    EXPECT_EQ(s1, s5);

    // long string to short string assignment
    s5 = s2;
    EXPECT_EQ(s2, s5);

    // short string to long string assignment
    s5 = s4;
    EXPECT_EQ(s5, s4);

    // long string move ctor
    const String s6(std::move(s5));
    EXPECT_EQ(s6, s4);
}

TEST(String, ElementAccess) {
    String s1(6, '*');
    const String s2 = "******";

    EXPECT_TRUE(s1.length() == 6);
    EXPECT_TRUE(s2.length() == 6);

    const String s3 = "Hello, World!";

    EXPECT_EQ(s3[0], 'H');
    EXPECT_EQ(s3[4], 'o');
    EXPECT_EQ(s3.at(0), 'H');
    EXPECT_EQ(s3.at(4), 'o');
    EXPECT_THROW(static_cast<void>(s3.at(s3.size()));, std::out_of_range);
    EXPECT_EQ(s3.front(), 'H');

    s1 = s3;
    s1.front() = 'h';
    s1.at(7) = 'w';
    s1.back() = '.';
    EXPECT_EQ(s1, "hello, world.");
}

TEST(String, Operators) {
    String s(6, '*');
    const String s2("******");

    EXPECT_TRUE(s == s2);
    EXPECT_TRUE(s2 == "******");
    EXPECT_TRUE("******" == s2);

    std::stringstream ss;
    ss << s;
    EXPECT_EQ(ss.str().c_str(), s2);

    s += s2;
    s += s2;
    EXPECT_EQ(s, String(18, '*'));

    const String s3 = "Hello";
    const String s4 = "World!";
    EXPECT_EQ(s3 + ", " + s4, "Hello, World!");
}


#if 0
TEST(String, LongStringBenchmark) {
    const String s1(2e6, '.');
    String s2;

    constexpr std::size_t ITERATIONS = 1000;
    for (std::size_t i = 0; i < ITERATIONS; ++i) {
        s2 += s1;
    }
}
#endif