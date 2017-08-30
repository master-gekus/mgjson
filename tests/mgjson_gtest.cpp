#include <cmath>
#include <limits>

#include <gtest/gtest.h>

#include "mgjson.h"

::std::ostream& operator<<(::std::ostream& os, const mgjson::json_type& type)
{
    switch (type) {
#define _case(a) case mgjson::a: os << #a; break;
    _case(Null);
    _case(Bool);
    _case(Integer);
    _case(Double);
    _case(String);
    _case(Array);
    _case(Object);
    _case(Undefined);
#undef _case
    default: os << "<invalid>"; break;
    }
    return os;
}

class TypedConstructorTest : public ::testing::TestWithParam<mgjson::json_type>
{
};

TEST_P(TypedConstructorTest, ConstructFromType)
{
    mgjson::json_type type = GetParam();
    mgjson json1(type);
    EXPECT_EQ(json1.type(), type);

    mgjson json2(json1);
    EXPECT_EQ(json2.type(), type);

    mgjson json3;
    json3 = json1;
    EXPECT_EQ(json3.type(), type);
}

INSTANTIATE_TEST_CASE_P(, TypedConstructorTest, ::testing::Values(
    mgjson::Null,
    mgjson::Bool,
    mgjson::Integer,
    mgjson::Double,
    mgjson::String,
    mgjson::Array,
    mgjson::Object,
    mgjson::Undefined
));

#define CONSTRUCT_FROM_VALUE_TEST(name, value, expected_type, checker, getter) \
TEST(ConstructorFromValue, name) \
{ \
    mgjson json1(value); \
    EXPECT_EQ(json1.type(), mgjson::expected_type); \
    EXPECT_TRUE(json1.checker()); \
    EXPECT_EQ(json1.getter(), value); \
    \
    mgjson json2(json1); \
    EXPECT_EQ(json2.type(), mgjson::expected_type); \
    EXPECT_TRUE(json2.checker()); \
    EXPECT_EQ(json2.getter(), value); \
}

#define CONSTRUCT_FROM_INT_VALUE_TESTS(tname, vtype, getter) \
    CONSTRUCT_FROM_VALUE_TEST(tname##Value, ((vtype)1), Integer, is_integer, getter) \
    CONSTRUCT_FROM_VALUE_TEST(tname##Min, std::numeric_limits<vtype>::min(), Integer, is_integer, getter) \
    CONSTRUCT_FROM_VALUE_TEST(tname##Max, std::numeric_limits<vtype>::max(), Integer, is_integer, getter)

#define CONSTRUCT_FROM_FLT_VALUE_TESTS(tname, vtype, getter) \
    CONSTRUCT_FROM_VALUE_TEST(tname##Pi, ((vtype)std::acos(((vtype)(-1.0)))), Double, is_double, getter) \
    CONSTRUCT_FROM_VALUE_TEST(tname##Min, std::numeric_limits<vtype>::min(), Double, is_double, getter) \
    CONSTRUCT_FROM_VALUE_TEST(tname##Max, std::numeric_limits<vtype>::max(), Double, is_double, getter)

CONSTRUCT_FROM_VALUE_TEST(BoolTrue, true, Bool, is_bool, to_bool)
CONSTRUCT_FROM_VALUE_TEST(BoolFalse, false, Bool, is_bool, to_bool)

CONSTRUCT_FROM_INT_VALUE_TESTS(Char, char, to_char)
CONSTRUCT_FROM_INT_VALUE_TESTS(UChar, unsigned char, to_uchar)
CONSTRUCT_FROM_INT_VALUE_TESTS(Short, short, to_short)
CONSTRUCT_FROM_INT_VALUE_TESTS(UShort, unsigned short, to_ushort)
CONSTRUCT_FROM_INT_VALUE_TESTS(Int, int, to_int)
CONSTRUCT_FROM_INT_VALUE_TESTS(UInt, unsigned int, to_uint)
CONSTRUCT_FROM_INT_VALUE_TESTS(Long, long, to_long)
CONSTRUCT_FROM_INT_VALUE_TESTS(ULong, unsigned long, to_ulong)
CONSTRUCT_FROM_INT_VALUE_TESTS(LongLong, long long, to_longlong)
CONSTRUCT_FROM_INT_VALUE_TESTS(ULongLong, unsigned long long, to_ulonglong)

CONSTRUCT_FROM_FLT_VALUE_TESTS(Float, float, to_float)
CONSTRUCT_FROM_FLT_VALUE_TESTS(Double, double, to_double)
CONSTRUCT_FROM_FLT_VALUE_TESTS(LongDouble, long double, to_longdouble)
