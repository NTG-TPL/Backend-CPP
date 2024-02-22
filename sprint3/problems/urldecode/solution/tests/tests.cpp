#define BOOST_TEST_MODULE urlencode tests
#include <boost/test/unit_test.hpp>

#include "../src/urldecode.h"

BOOST_AUTO_TEST_CASE(UrlDecode_tests) {
    using namespace std::literals;
    BOOST_CHECK_EQUAL(UrlDecode(""sv), ""s);
    BOOST_CHECK_EQUAL(UrlDecode("Hello%20World !"sv), "Hello World !"s);
    BOOST_CHECK_EQUAL(UrlDecode("Hello%2BWorld%2B%253E%2Bhow%2Bare%2Byou%253F"sv), "Hello+World+%3E+how+are+you%3F"s);
    BOOST_CHECK_EQUAL(UrlDecode("%D0%90%D0%B1%D1%80%D0%B0%D0%BA%D0%B0%D0%B4%D0%B0%D0%B1%D1%80%D0%B0"sv), "Абракадабра"s);
    BOOST_CHECK_EQUAL(UrlDecode("Привет"sv), "Привет"s);
}