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

TEST(ConstructorFromValue, ConstChar)
{
    static const char test_string[]="Test string.";

    mgjson json1(test_string);
    EXPECT_EQ(json1.type(), mgjson::String);
    EXPECT_TRUE(json1.is_string());
    EXPECT_STREQ(json1.to_str(), test_string);

    mgjson json2(json1);
    EXPECT_EQ(json2.type(), mgjson::String);
    EXPECT_TRUE(json2.is_string());
    EXPECT_STREQ(json2.to_str(), test_string);
}

TEST(ConstructorFromValue, StdString)
{
    static const std::string test_string("Test string.");

    mgjson json1(test_string);
    EXPECT_EQ(json1.type(), mgjson::String);
    EXPECT_TRUE(json1.is_string());
    EXPECT_EQ(json1.to_string(), test_string);

    mgjson json2(json1);
    EXPECT_EQ(json2.type(), mgjson::String);
    EXPECT_TRUE(json2.is_string());
    EXPECT_EQ(json2.to_string(), test_string);
}

TEST(ConstructorFromValue, StdStringWithZeros)
{
    static const char test_string_data[]="Data before zero\0Data after zero";
    static const std::string test_string(test_string_data, sizeof(test_string_data) - 1);
    ASSERT_NE(std::string(test_string_data), test_string);

    mgjson json1(test_string);
    EXPECT_EQ(json1.type(), mgjson::String);
    EXPECT_TRUE(json1.is_string());
    EXPECT_EQ(json1.to_string(), test_string);

    mgjson json2(json1);
    EXPECT_EQ(json2.type(), mgjson::String);
    EXPECT_TRUE(json2.is_string());
    EXPECT_EQ(json2.to_string(), test_string);
    EXPECT_EQ(json2.to_string().size(), sizeof(test_string_data) - 1);
}

struct StringValueCastParam
{
    std::string str_;
    bool b_value_;
    unsigned long long i_value_;
    long double d_value;
};

::std::ostream& operator<<(::std::ostream& os, const StringValueCastParam& p)
{
    os << "\"" << p.str_ << "\"";
    return os;
}

class StringValueCastTest : public ::testing::TestWithParam<StringValueCastParam>
{
};

TEST_P(StringValueCastTest, ConstructFromString)
{
    const StringValueCastParam& p = GetParam();
    mgjson j(p.str_);
    EXPECT_EQ(j.type(), mgjson::String);

    EXPECT_EQ(j.to_string(), p.str_);
    EXPECT_EQ(j.to_bool(), p.b_value_);
    EXPECT_EQ(j.to_ulonglong(), p.i_value_);
    EXPECT_EQ(j.to_longdouble(), p.d_value);
}

#define _test(s,b,i,d) \
    StringValueCastParam{std::string(s, sizeof(s) - 1), b, \
                         static_cast<unsigned long long>(i), \
                         static_cast<long double>(d)}

#ifdef _MSC_VER
#define _long_double_tests \
    _test("1E300", true, std::numeric_limits<unsigned long long>::max(), 1E300L), \
    _test("-1E300", true, std::numeric_limits<long long>::min(), -1E300L)
#else
#define _long_double_tests \
    _test("1E1000", true, std::numeric_limits<unsigned long long>::max(), 1E1000L), \
    _test("-1E1000", true, std::numeric_limits<long long>::min(), -1E1000L)
#endif

INSTANTIATE_TEST_CASE_P(, StringValueCastTest, ::testing::Values(
    _test("Not a value", false, 0, 0),
    _test("0", false, 0, 0),
    _test("Off", false, 0, 0),
    _test("False", false, 0, 0),
    _test("On", true, 1, 1),
    _test("True", true, 1, 1),
    _test("1", true, 1, 1),
    _test("-1", true, -1, -1),
    _test("0x10", true, 16, 16),
    _test("1E100", true, std::numeric_limits<unsigned long long>::max(), 1E100L),
    _test("-1E100", true, std::numeric_limits<long long>::min(), -1E100L),
    _long_double_tests,
    _test(".123456789012345678901234567890", false, 0, .123456789012345678901234567890L),
    _test("-.123456789012345678901234567890", false, 0, -.123456789012345678901234567890L),
    _test(".123456789012345678901234567890E100", true, std::numeric_limits<unsigned long long>::max(), .123456789012345678901234567890E100L),
    _test(".123456789012345678901234567890E-100", false, 0, .123456789012345678901234567890E-100L),
    _test("12345678901234.5678901234567890", true, 12345678901234, 12345678901234.5678901234567890L),
    _test("-12345678901234.5678901234567890", true, -12345678901234, -12345678901234.5678901234567890L)
));
#undef _test

class CountAndResizeSimpleTest : public ::testing::TestWithParam<mgjson::json_type>
{
};

TEST_P(CountAndResizeSimpleTest, ConstructFromType)
{
    mgjson::json_type type = GetParam();
    mgjson json(type);
    EXPECT_EQ(json.type(), type);
    EXPECT_EQ(json.count(), 0U);

    json.resize(10);
    EXPECT_EQ(json.count(), 10U);

    json.resize(5);
    EXPECT_EQ(json.count(), 5U);

    json.resize(0);
    EXPECT_EQ(json.count(), 0U);
}

INSTANTIATE_TEST_CASE_P(, CountAndResizeSimpleTest, ::testing::Values(
    mgjson::Null,
    mgjson::Bool,
    mgjson::Integer,
    mgjson::Double,
    mgjson::String,
    mgjson::Array,
    mgjson::Object,
    mgjson::Undefined
));

struct ArrayAt_param
{
    mgjson::json_type type;
    bool must_throw;
};

class ArrayAt : public ::testing::TestWithParam<ArrayAt_param>
{
};

TEST_F(ArrayAt, Simple)
{
    mgjson json;
    json.resize(10);
    EXPECT_EQ(json.count(), 10U);

    mgjson json2;
    json2 = json;
    EXPECT_EQ(json2.count(), 10U);

    for (size_t i = 0; i < json.count(); i++) {
        mgjson j1 = json.at(i);
        EXPECT_EQ(j1.type(), mgjson::Null);

        mgjson& j2 = json2.at(i);
        EXPECT_EQ(j2.type(), mgjson::Null);

        j2 = 1;
        EXPECT_EQ(j2.type(), mgjson::Integer);
        EXPECT_EQ(j2.to<int>(), 1);

        mgjson j3 = json.at(i);
        EXPECT_EQ(j3.type(), mgjson::Null);

        mgjson& j4 = json2.at(i);
        EXPECT_EQ(j4.type(), mgjson::Integer);
        EXPECT_EQ(j4.to<int>(), 1);
    }

    EXPECT_TRUE(static_cast<const mgjson&>(json).at(11).is_null());
    EXPECT_THROW(json2.at(11) = 1, std::out_of_range);
}

TEST_F(ArrayAt, AutoGrow)
{
    mgjson json;
    EXPECT_EQ(json.count(), 0U);
    EXPECT_TRUE(static_cast<const mgjson&>(json).at(static_cast<size_t>(0)).is_null());

    json.at(static_cast<size_t>(0)) = 1;
    EXPECT_EQ(json.count(), 1U);
    EXPECT_EQ(json.at(static_cast<size_t>(0)).type(), mgjson::Integer);
    EXPECT_EQ(json.at(static_cast<size_t>(0)).to<int>(), 1);
    EXPECT_THROW(json.at(2) = 1, std::out_of_range);

    json.at(1) = 2;
    EXPECT_EQ(json.count(), 2U);
    EXPECT_EQ(json.at(1).type(), mgjson::Integer);
    EXPECT_EQ(json.at(1).to<int>(), 2);
    EXPECT_THROW(json.at(3) = 1, std::out_of_range);

    json.at(static_cast<size_t>(0)) = "Test string";
    EXPECT_EQ(json.count(), 2U);
    EXPECT_EQ(json.at(static_cast<size_t>(0)).type(), mgjson::String);
    EXPECT_STREQ(json.at(static_cast<size_t>(0)).to<const char*>(), "Test string");
    EXPECT_EQ(json.at(1).type(), mgjson::Integer);
    EXPECT_EQ(json.at(1).to<int>(), 2);
}

static constexpr const ArrayAt_param ArrayAt_params[] = {
    {mgjson::Null, false},
    {mgjson::Bool, true},
    {mgjson::Integer, true},
    {mgjson::Double, true},
    {mgjson::String, true},
    {mgjson::Array, false},
    {mgjson::Object, true},
    {mgjson::Undefined, false},
};

INSTANTIATE_TEST_CASE_P(, ArrayAt, ::testing::ValuesIn(ArrayAt_params));

TEST_P(ArrayAt, Exceptions)
{
    ArrayAt_param param = GetParam();
    mgjson json(param.type);

    if (param.must_throw) {
        EXPECT_THROW(json.at(static_cast<size_t>(0)) = 1, std::out_of_range);
    } else {
        json.at(static_cast<size_t>(0)) = 1;
        EXPECT_EQ(json.count(), 1U);
        EXPECT_EQ(json.at(static_cast<size_t>(0)).type(), mgjson::Integer);
        EXPECT_EQ(json.at(static_cast<size_t>(0)).to<int>(), 1);
    }
}
