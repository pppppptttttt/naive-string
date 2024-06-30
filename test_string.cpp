#include <gtest/gtest.h>
#include <cassert>
#include <sstream>
#include <stdexcept>

// #define TEST_STL_STRING

#ifdef TEST_STL_STRING
#include <string>
using String = std::string;
#else
#include "naive_string.hpp"
#endif

struct Dummy {
    Dummy() {
#ifdef TEST_STL_STRING
        std::cout << "STL string tests" << std::endl;
#else
        std::cout << "Naive string tests" << std::endl;
#endif
    }
} dummy;  // NOLINT

TEST(String, Constructors) {
    String s;
    EXPECT_EQ(s.length(), 0);
    EXPECT_EQ(s.size(), 0);
    EXPECT_EQ(s, "");

    String s1(6, '*');
    const String s2 = "******";
    EXPECT_EQ(s1, s2);

    String s3 = std::move(s1);
    EXPECT_EQ(s3, s2);

    s1 = std::move(s3);
    EXPECT_EQ(s1, s2);

    s3 = s2;
    EXPECT_EQ(s3, s2);

    const String s4(100, '#');
    s1 = s4;
    EXPECT_EQ(s1, s4);

    String s5(s1);
    EXPECT_EQ(s1, s5);
    s5 = s2;
    EXPECT_EQ(s2, s5);
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

#ifndef TEST_STL_STRING
TEST(String, Capacity) {
    // TODO
}
#endif

TEST(String, Operators) {
    String s(6, '*');
    const String s2("******");

    EXPECT_TRUE(s == s2);
    EXPECT_TRUE(s2 == "******");
    EXPECT_TRUE("******" == s2);

    std::stringstream ss;
    ss << s;
    EXPECT_EQ(ss.str().c_str(), s2);

    s += s2 + s;
    EXPECT_EQ(s, String(18, '*'));

    String s3 = "Hello";
    String s4 = "World!";
    EXPECT_EQ(s3 + ", " + s4, "Hello, World!");
}