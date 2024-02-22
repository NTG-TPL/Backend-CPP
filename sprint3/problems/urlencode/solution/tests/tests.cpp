#include <gtest/gtest.h>

#include "../src/urlencode.h"

using namespace std::literals;

TEST(UrlEncodeTestSuite, OrdinaryCharsAreNotEncoded) {
    using namespace std::literals;
    EXPECT_EQ(UrlEncode(""sv), ""s);
    EXPECT_EQ(UrlEncode("hello"sv), "hello"s);
    EXPECT_EQ(UrlEncode("Hello World !"sv), "Hello%20World%20%21"s);
    EXPECT_EQ(UrlEncode("Hello+World+%3E+how+are+you%3F"sv), "Hello%2BWorld%2B%253E%2Bhow%2Bare%2Byou%253F"s);
    EXPECT_EQ(UrlEncode("abc*"sv), "abc%2A"s);
}
