#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif

#include "mgjson.h"

#include <cmath>
#include <limits>

#include <gtest/gtest.h>

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

#define CONVERSION_TEST(n,t,v) \
    TEST(ConvertionTest, n) \
    { \
        t value = v; \
        mgjson json(value); \
        EXPECT_EQ(json.to<t>(), value); \
        EXPECT_EQ(static_cast<t>(json), value); \
    }

CONVERSION_TEST(bool_t, bool, true)
CONVERSION_TEST(bool_f, bool, false)
CONVERSION_TEST(char, char, 1)
CONVERSION_TEST(uchar, unsigned char, 1)
CONVERSION_TEST(short, short, 1)
CONVERSION_TEST(ushort, unsigned short, 1)
CONVERSION_TEST(int, int, 1)
CONVERSION_TEST(uint, unsigned int, 1)
CONVERSION_TEST(long, long, 1)
CONVERSION_TEST(ulong, unsigned long, 1)
CONVERSION_TEST(size_t, size_t, 1)
CONVERSION_TEST(intptr_t, intptr_t, 1)
CONVERSION_TEST(uintptr_t, uintptr_t, 1)

TEST(ConvertionTest, string)
{
    const char* value = "Test string";
    mgjson json(value);
    EXPECT_STREQ(json.to<const char*>(), value);
    EXPECT_STREQ(static_cast<const char*>(json), value);
}

TEST(ConvertionTest, std_string)
{
    std::string value = "Test string";
    mgjson json(value);
    EXPECT_EQ(json.to<std::string>(), value);
}

TEST(ConvertionTest, std_string_ref)
{
    std::string value = "Test string";
    mgjson json(value);
    EXPECT_EQ(json.to<const std::string&>(), value);
    EXPECT_EQ(static_cast<const std::string&>(json), value);
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

struct test_exception_param
{
    mgjson::json_type type;
    bool must_throw;
};

static constexpr const test_exception_param ArrayExeption_params[] = {
    {mgjson::Null, false},
    {mgjson::Bool, true},
    {mgjson::Integer, true},
    {mgjson::Double, true},
    {mgjson::String, true},
    {mgjson::Array, false},
    {mgjson::Object, true},
    {mgjson::Undefined, false},
};

static constexpr const test_exception_param ObjectException_params[] = {
    {mgjson::Null, false},
    {mgjson::Bool, true},
    {mgjson::Integer, true},
    {mgjson::Double, true},
    {mgjson::String, true},
    {mgjson::Array, true},
    {mgjson::Object, false},
    {mgjson::Undefined, false},
};

class ArrayAt : public ::testing::TestWithParam<test_exception_param>
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

INSTANTIATE_TEST_CASE_P(, ArrayAt, ::testing::ValuesIn(ArrayExeption_params));

TEST_P(ArrayAt, Exceptions)
{
    test_exception_param param = GetParam();
    mgjson json(param.type);

    if (param.must_throw) {
        EXPECT_THROW(json.at(static_cast<size_t>(0)) = 1, std::invalid_argument);
    } else {
        json.at(static_cast<size_t>(0)) = 1;
        EXPECT_EQ(json.count(), 1U);
        EXPECT_EQ(json.at(static_cast<size_t>(0)).type(), mgjson::Integer);
        EXPECT_EQ(json.at(static_cast<size_t>(0)).to<int>(), 1);
    }
}

class ObjectAt : public ::testing::TestWithParam<test_exception_param>
{
};

TEST_F(ObjectAt, Simple)
{
    mgjson json;
    EXPECT_EQ(json.count(), 0u);

    for (size_t i = 0; i < 10; i++) {
        char key1[10];
        sprintf(key1, "Key %d", static_cast<int>(i));
        std::string key2(key1);
        const mgjson& cjson = json;
        EXPECT_TRUE(cjson.at(key1).is_null());
        EXPECT_TRUE(cjson.at(key2).is_null());
        json.at(key1) = i;
        EXPECT_EQ(json.count(), i + 1U);
        EXPECT_TRUE(cjson.at(key1).is_integer());
        EXPECT_TRUE(cjson.at(key2).is_integer());
        EXPECT_EQ(cjson.at(key1).to<int>(), static_cast<int>(i));
        EXPECT_EQ(cjson.at(key2).to<int>(), static_cast<int>(i));
    }
    EXPECT_EQ(json.count(), 10U);

    mgjson json2;
    json2 = json;
    for (size_t i = 10; i < 20; i++) {
        char key1[10];
        sprintf(key1, "Key %d", static_cast<int>(i));
        std::string key2(key1);
        const mgjson& cjson = json2;
        EXPECT_TRUE(cjson.at(key1).is_null());
        EXPECT_TRUE(cjson.at(key2).is_null());
        json2.at(key1) = key2;
        EXPECT_EQ(json2.count(), i + 1);
        EXPECT_TRUE(cjson.at(key1).is_string());
        EXPECT_TRUE(cjson.at(key2).is_string());
        EXPECT_STREQ(cjson.at(key1).to<const char *>(), key1);
        EXPECT_STREQ(cjson.at(key2).to<const char *>(), key1);
    }
    EXPECT_EQ(json.count(), 10U);
    EXPECT_EQ(json2.count(), 20U);

    for (size_t i = 0; i < 10; i++) {
        char key1[10];
        sprintf(key1, "Key %d", static_cast<int>(i));
        std::string key2(key1);
        const mgjson& cjson = json;
        EXPECT_FALSE(cjson.at(key1).is_null());
        EXPECT_FALSE(cjson.at(key2).is_null());
        json.at(key1) = i + 10;
        EXPECT_TRUE(cjson.at(key1).is_integer());
        EXPECT_TRUE(cjson.at(key2).is_integer());
        EXPECT_EQ(cjson.at(key1).to<int>(), static_cast<int>(i + 10));
        EXPECT_EQ(cjson.at(key2).to<int>(), static_cast<int>(i + 10));
    }
    EXPECT_EQ(json.count(), 10U);
    EXPECT_EQ(json2.count(), 20U);

    for (size_t i = 0; i < 10; i++) {
        char key1[10];
        sprintf(key1, "Key %d", static_cast<int>(i));
        mgjson& element = json2.at(key1);
        EXPECT_TRUE(element.is_integer());
        EXPECT_EQ(element.to<int>(), static_cast<int>(i));
        element = key1;
        EXPECT_TRUE(static_cast<const mgjson&>(json2).at(key1).is_string());
        EXPECT_STREQ(static_cast<const mgjson&>(json2).at(key1).to<const char *>(), key1);
    }
    EXPECT_EQ(json.count(), 10U);
    EXPECT_EQ(json2.count(), 20U);

    for (size_t i = 0; i < 10; i++) {
        char key1[10];
        sprintf(key1, "Key %d", static_cast<int>(i));
        mgjson element = json2.at(key1);
        EXPECT_TRUE(element.is_string());
        EXPECT_STREQ(element.to<const char*>(), key1);
        element = i + 100;
        EXPECT_TRUE(element.is_integer());
        EXPECT_EQ(element.to<int>(), static_cast<int>(i + 100));
        EXPECT_TRUE(json2.at(key1).is_string());
        EXPECT_STREQ(json2.at(key1).to<const char *>(), key1);
        EXPECT_TRUE(static_cast<const mgjson&>(json2).at(key1).is_string());
        EXPECT_STREQ(static_cast<const mgjson&>(json2).at(key1).to<const char *>(), key1);
    }
    EXPECT_EQ(json.count(), 10U);
    EXPECT_EQ(json2.count(), 20U);
}

TEST_F(ObjectAt, EmptyKey)
{
    mgjson json;
    json["Test"] = 1;
    EXPECT_TRUE(json.is_object());
    EXPECT_EQ(json.count(), 1U);
    EXPECT_TRUE(json["Test"].is_integer());
    EXPECT_EQ(json["Test"].to_int(), 1);
    EXPECT_THROW(json[nullptr] = 1, std::out_of_range);
    EXPECT_THROW(json[""] = 1, std::out_of_range);
}

INSTANTIATE_TEST_CASE_P(, ObjectAt, ::testing::ValuesIn(ObjectException_params));

TEST_P(ObjectAt, Exceptions)
{
    test_exception_param param = GetParam();
    mgjson json(param.type);
    EXPECT_EQ(json.count(), 0U);

    if (param.must_throw) {
        EXPECT_THROW(json.at("key") = 1, std::invalid_argument);
    } else {
        json.at("key") = 1;
        EXPECT_EQ(json.count(), 1U);
        EXPECT_EQ(json.at("key").type(), mgjson::Integer);
        EXPECT_EQ(json.at("key").to<int>(), 1);
    }
}

struct Keys_param
{
    std::vector<std::string> src;
    std::vector<std::string> dst;
};

std::ostream& operator << (std::ostream& os, const Keys_param& param)
{
    os << testing::PrintToString(param.src);
    return os;
}

class Keys : public ::testing::TestWithParam<Keys_param>
{
};

static const Keys_param Keys_params[] ={
    {{}, {}},
    {{"Key"}, {}},
    {{"Key", "Key"}, {}},
    {{"Key 0", "Key 1"}, {"Key 0", "Key 1"}},
    {{"Key 1", "Key 0"}, {"Key 0", "Key 1"}},
    {{"Key 0", "Key 1", "Key 0"}, {"Key 0", "Key 1"}},
    {{"Key 1", "Key 0", "Key 1"}, {"Key 0", "Key 1"}},
    {{"Key 0", "Key 1", "Key 0", "Key 2", "Key 4", "Key 3", "Key 2"}, {}},
};

INSTANTIATE_TEST_CASE_P(, Keys, ::testing::ValuesIn(Keys_params));

TEST_P(Keys, Keys)
{
    Keys_param param = GetParam();
    std::vector<std::string> dst;
    if (param.dst.empty()) {
        std::set<std::string> dst_set;
        for (const auto& key : param.src) {
            dst_set.insert(key);
        }
        for (const auto& key : dst_set) {
            dst.push_back(key);
        }
    } else {
        dst = param.dst;
    }

    mgjson json;
    int value = 1;
    for (const auto& key : param.src) {
        json[key] = value++;
    }
    EXPECT_EQ(json.keys(), dst);
}

class PushBack : public ::testing::TestWithParam<test_exception_param>
{
};

TEST_F(PushBack, Simple)
{
    mgjson json;
    EXPECT_TRUE(json.is_null());

    for (size_t i = 0; i < 10; i++) {
        mgjson &j1 = json.push_back();
        EXPECT_TRUE(json.is_array());
        EXPECT_EQ(json.count(), static_cast<size_t>(i * 2 + 1));
        EXPECT_TRUE(j1.is_null());
        EXPECT_TRUE(json[i * 2].is_null());
        j1 = i;
        EXPECT_TRUE(json[i * 2].is_integer());
        EXPECT_EQ(json[i * 2].to<size_t>(), i);
        json[i * 2] = "Test string";
        EXPECT_TRUE(j1.is_string());
        EXPECT_STREQ(j1.to<const char*>(), "Test string");

        mgjson &j2 = json.push_back("Test string 2");
        EXPECT_EQ(json.count(), static_cast<size_t>(i * 2 + 2));
        EXPECT_TRUE(j2.is_string());
        EXPECT_TRUE(json[i * 2 + 1].is_string());
        EXPECT_STREQ(j2.to<const char*>(), "Test string 2");
        EXPECT_STREQ(json[i * 2 + 1].to<const char*>(), "Test string 2");

        j2 = i * 10;
        EXPECT_TRUE(json[i * 2 + 1].is_integer());
        EXPECT_EQ(json[i * 2 + 1].to<size_t>(), i * 10);
        json[i * 2 + 1] = "Test string";
        EXPECT_TRUE(j2.is_string());
        EXPECT_STREQ(j2.to<const char*>(), "Test string");
    }
}

INSTANTIATE_TEST_CASE_P(, PushBack, ::testing::ValuesIn(ArrayExeption_params));

TEST_P(PushBack, Exceptions)
{
    test_exception_param param = GetParam();
    mgjson json(param.type);

    if (param.must_throw) {
        EXPECT_THROW(json.push_back(1), std::invalid_argument);
    } else {
        json.push_back(1) = 1;
        EXPECT_EQ(json.count(), 1U);
        EXPECT_EQ(json.at(static_cast<size_t>(0)).type(), mgjson::Integer);
        EXPECT_EQ(json.at(static_cast<size_t>(0)).to<int>(), 1);
    }
}

class PushFront : public ::testing::TestWithParam<test_exception_param>
{
};

TEST_F(PushFront, Simple)
{
    mgjson json;
    EXPECT_TRUE(json.is_null());

    for (size_t i = 0; i < 10; i++) {
        mgjson &j1 = json.push_front();
        EXPECT_TRUE(json.is_array());
        EXPECT_EQ(json.count(), static_cast<size_t>(i * 2 + 1));
        EXPECT_TRUE(j1.is_null());
        EXPECT_TRUE(json[static_cast<size_t>(0)].is_null());
        j1 = i;
        EXPECT_TRUE(json[static_cast<size_t>(0)].is_integer());
        EXPECT_EQ(json[static_cast<size_t>(0)].to<size_t>(), i);
        json[static_cast<size_t>(0)] = "Test string";
        EXPECT_TRUE(j1.is_string());
        EXPECT_STREQ(j1.to<const char*>(), "Test string");

        mgjson &j2 = json.push_front("Test string 2");
        EXPECT_EQ(json.count(), static_cast<size_t>(i * 2 + 2));
        EXPECT_TRUE(j2.is_string());
        EXPECT_TRUE(json[static_cast<size_t>(0)].is_string());
        EXPECT_STREQ(j2.to<const char*>(), "Test string 2");
        EXPECT_STREQ(json[static_cast<size_t>(0)].to<const char*>(), "Test string 2");

        j2 = i * 10;
        EXPECT_TRUE(json[static_cast<size_t>(0)].is_integer());
        EXPECT_EQ(json[static_cast<size_t>(0)].to<size_t>(), i * 10);
        json[static_cast<size_t>(0)] = "Test string";
        EXPECT_TRUE(j2.is_string());
        EXPECT_STREQ(j2.to<const char*>(), "Test string");
    }
}

INSTANTIATE_TEST_CASE_P(, PushFront, ::testing::ValuesIn(ArrayExeption_params));

TEST_P(PushFront, Exceptions)
{
    test_exception_param param = GetParam();
    mgjson json(param.type);

    if (param.must_throw) {
        EXPECT_THROW(json.push_front(1), std::invalid_argument);
    } else {
        json.push_front(1) = 1;
        EXPECT_EQ(json.count(), 1U);
        EXPECT_EQ(json.at(static_cast<size_t>(0)).type(), mgjson::Integer);
        EXPECT_EQ(json.at(static_cast<size_t>(0)).to<int>(), 1);
    }
}

TEST(Remove, Array)
{
    mgjson json;
    EXPECT_TRUE(json.is_null());

    EXPECT_TRUE(json.push_back(1).is_integer());
    EXPECT_TRUE(json.push_back(2).is_integer());
    EXPECT_TRUE(json.push_back("String 1").is_string());
    EXPECT_TRUE(json.push_back("String 2").is_string());
    EXPECT_TRUE(json.push_back(1.1).is_double());
    EXPECT_TRUE(json.push_back(2.2).is_double());
    EXPECT_EQ(json.count(), 6U);

    json.remove(2);
    EXPECT_EQ(json.count(), 5U);
    EXPECT_TRUE(json[static_cast<size_t>(0)].is_integer());
    EXPECT_EQ(json[static_cast<size_t>(0)].to<int>(), 1);
    EXPECT_TRUE(json[1].is_integer());
    EXPECT_EQ(json[1].to<int>(), 2);
    EXPECT_TRUE(json[2].is_string());
    EXPECT_STREQ(json[2].to<const char*>(), "String 2");
    EXPECT_TRUE(json[3].is_double());
    EXPECT_EQ(json[3].to<double>(), 1.1);
    EXPECT_TRUE(json[4].is_double());
    EXPECT_EQ(json[4].to<double>(), 2.2);

    json.remove(3);
    EXPECT_EQ(json.count(), 4U);
    EXPECT_TRUE(json[static_cast<size_t>(0)].is_integer());
    EXPECT_EQ(json[static_cast<size_t>(0)].to<int>(), 1);
    EXPECT_TRUE(json[1].is_integer());
    EXPECT_EQ(json[1].to<int>(), 2);
    EXPECT_TRUE(json[2].is_string());
    EXPECT_STREQ(json[2].to<const char*>(), "String 2");
    EXPECT_TRUE(json[3].is_double());
    EXPECT_EQ(json[3].to<double>(), 2.2);

    json.remove(1);
    EXPECT_EQ(json.count(), 3U);
    EXPECT_TRUE(json[static_cast<size_t>(0)].is_integer());
    EXPECT_EQ(json[static_cast<size_t>(0)].to<int>(), 1);
    EXPECT_TRUE(json[1].is_string());
    EXPECT_STREQ(json[1].to<const char*>(), "String 2");
    EXPECT_TRUE(json[2].is_double());
    EXPECT_EQ(json[2].to<double>(), 2.2);

    json.remove(2);
    EXPECT_EQ(json.count(), 2U);
    json.remove(1);
    EXPECT_EQ(json.count(), 1U);
    json.remove(static_cast<size_t>(0));
    EXPECT_EQ(json.count(), 0U);
}

TEST(Remove, Object)
{
    static constexpr const char* keys[] = {
        "Key 1", "Key 2", "Key 3", "Key 4", "Key 5", "Key 6"
    };

    mgjson json;
    const mgjson& cjson = json;
    EXPECT_TRUE(json.is_null());

    json[keys[5]] = 2.2;
    json[keys[4]] = 1.1;
    json[keys[0]] = 1;
    json[keys[1]] = 2;
    json[keys[2]] = "String 1";
    json[keys[3]] = "String 2";
    EXPECT_EQ(json.count(), 6U);

    json.remove(keys[2]);
    EXPECT_EQ(cjson.count(), 5U);
    EXPECT_TRUE(cjson[keys[0]].is_integer());
    EXPECT_EQ(cjson[keys[0]].to<int>(), 1);
    EXPECT_TRUE(cjson[keys[1]].is_integer());
    EXPECT_EQ(cjson[keys[1]].to<int>(), 2);
    EXPECT_TRUE(cjson[keys[2]].is_null());
    EXPECT_TRUE(cjson[keys[3]].is_string());
    EXPECT_STREQ(cjson[keys[3]].to<const char*>(), "String 2");
    EXPECT_TRUE(cjson[keys[4]].is_double());
    EXPECT_EQ(cjson[keys[4]].to<double>(), 1.1);
    EXPECT_TRUE(cjson[keys[5]].is_double());
    EXPECT_EQ(cjson[keys[5]].to<double>(), 2.2);

    json.remove(keys[4]);
    EXPECT_EQ(cjson.count(), 4U);
    EXPECT_TRUE(cjson[keys[0]].is_integer());
    EXPECT_EQ(cjson[keys[0]].to<int>(), 1);
    EXPECT_TRUE(cjson[keys[1]].is_integer());
    EXPECT_EQ(cjson[keys[1]].to<int>(), 2);
    EXPECT_TRUE(cjson[keys[2]].is_null());
    EXPECT_TRUE(cjson[keys[3]].is_string());
    EXPECT_STREQ(cjson[keys[3]].to<const char*>(), "String 2");
    EXPECT_TRUE(cjson[keys[4]].is_null());
    EXPECT_TRUE(cjson[keys[5]].is_double());
    EXPECT_EQ(cjson[keys[5]].to<double>(), 2.2);

    json.remove(keys[1]);
    EXPECT_EQ(cjson.count(), 3U);
    EXPECT_TRUE(cjson[keys[0]].is_integer());
    EXPECT_EQ(cjson[keys[0]].to<int>(), 1);
    EXPECT_TRUE(cjson[keys[1]].is_null());
    EXPECT_TRUE(cjson[keys[2]].is_null());
    EXPECT_TRUE(cjson[keys[3]].is_string());
    EXPECT_STREQ(cjson[keys[3]].to<const char*>(), "String 2");
    EXPECT_TRUE(cjson[keys[4]].is_null());
    EXPECT_TRUE(cjson[keys[5]].is_double());
    EXPECT_EQ(cjson[keys[5]].to<double>(), 2.2);

    json.remove(keys[4]);
    EXPECT_EQ(cjson.count(), 3U);
    EXPECT_TRUE(cjson[keys[0]].is_integer());
    EXPECT_EQ(cjson[keys[0]].to<int>(), 1);
    EXPECT_TRUE(cjson[keys[1]].is_null());
    EXPECT_TRUE(cjson[keys[2]].is_null());
    EXPECT_TRUE(cjson[keys[3]].is_string());
    EXPECT_STREQ(cjson[keys[3]].to<const char*>(), "String 2");
    EXPECT_TRUE(cjson[keys[4]].is_null());
    EXPECT_TRUE(cjson[keys[5]].is_double());
    EXPECT_EQ(cjson[keys[5]].to<double>(), 2.2);

    json.remove("Nonexistent key");
    EXPECT_EQ(cjson.count(), 3U);
    EXPECT_TRUE(cjson[keys[0]].is_integer());
    EXPECT_EQ(cjson[keys[0]].to<int>(), 1);
    EXPECT_TRUE(cjson[keys[1]].is_null());
    EXPECT_TRUE(cjson[keys[2]].is_null());
    EXPECT_TRUE(cjson[keys[3]].is_string());
    EXPECT_STREQ(cjson[keys[3]].to<const char*>(), "String 2");
    EXPECT_TRUE(cjson[keys[4]].is_null());
    EXPECT_TRUE(cjson[keys[5]].is_double());
    EXPECT_EQ(cjson[keys[5]].to<double>(), 2.2);

    json.remove(keys[0]);
    EXPECT_EQ(cjson.count(), 2U);
    json.remove(keys[5]);
    EXPECT_EQ(cjson.count(), 1U);
    json.remove(keys[3]);
    EXPECT_EQ(cjson.count(), 0U);
}

TEST(Take, Array)
{
    mgjson json, json1;
    EXPECT_TRUE(json.is_null());

    EXPECT_TRUE(json.push_back(1).is_integer());
    EXPECT_TRUE(json.push_back(2).is_integer());
    EXPECT_TRUE(json.push_back("String 1").is_string());
    EXPECT_TRUE(json.push_back("String 2").is_string());
    EXPECT_TRUE(json.push_back(1.1).is_double());
    EXPECT_TRUE(json.push_back(2.2).is_double());
    EXPECT_EQ(json.count(), 6U);

    json1 = json.take(2);
    EXPECT_TRUE(json1.is_string());
    EXPECT_STREQ(json1.to<const char*>(), "String 1");
    EXPECT_EQ(json.count(), 5U);
    EXPECT_TRUE(json[static_cast<size_t>(0)].is_integer());
    EXPECT_EQ(json[static_cast<size_t>(0)].to<int>(), 1);
    EXPECT_TRUE(json[1].is_integer());
    EXPECT_EQ(json[1].to<int>(), 2);
    EXPECT_TRUE(json[2].is_string());
    EXPECT_STREQ(json[2].to<const char*>(), "String 2");
    EXPECT_TRUE(json[3].is_double());
    EXPECT_EQ(json[3].to<double>(), 1.1);
    EXPECT_TRUE(json[4].is_double());
    EXPECT_EQ(json[4].to<double>(), 2.2);

    json1 = json.take(3);
    EXPECT_TRUE(json1.is_double());
    EXPECT_EQ(json1.to<double>(), 1.1);
    EXPECT_EQ(json.count(), 4U);
    EXPECT_TRUE(json[static_cast<size_t>(0)].is_integer());
    EXPECT_EQ(json[static_cast<size_t>(0)].to<int>(), 1);
    EXPECT_TRUE(json[1].is_integer());
    EXPECT_EQ(json[1].to<int>(), 2);
    EXPECT_TRUE(json[2].is_string());
    EXPECT_STREQ(json[2].to<const char*>(), "String 2");
    EXPECT_TRUE(json[3].is_double());
    EXPECT_EQ(json[3].to<double>(), 2.2);

    json1 = json.take(1);
    EXPECT_TRUE(json1.is_integer());
    EXPECT_EQ(json1.to<int>(), 2);
    EXPECT_EQ(json.count(), 3U);
    EXPECT_TRUE(json[static_cast<size_t>(0)].is_integer());
    EXPECT_EQ(json[static_cast<size_t>(0)].to<int>(), 1);
    EXPECT_TRUE(json[1].is_string());
    EXPECT_STREQ(json[1].to<const char*>(), "String 2");
    EXPECT_TRUE(json[2].is_double());
    EXPECT_EQ(json[2].to<double>(), 2.2);

    json1 = json.take(2);
    EXPECT_TRUE(json1.is_double());
    EXPECT_EQ(json1.to<double>(), 2.2);
    EXPECT_EQ(json.count(), 2U);
    json1 = json.take(1);
    EXPECT_TRUE(json1.is_string());
    EXPECT_STREQ(json1.to<const char*>(), "String 2");
    EXPECT_EQ(json.count(), 1U);
    json1 = json.take(static_cast<size_t>(0));
    EXPECT_TRUE(json1.is_integer());
    EXPECT_EQ(json1.to<int>(), 1);
    EXPECT_EQ(json.count(), 0U);
}

TEST(Take, Object)
{
    static constexpr const char* keys[] = {
        "Key 1", "Key 2", "Key 3", "Key 4", "Key 5", "Key 6"
    };

    mgjson json, json1;
    const mgjson& cjson = json;
    EXPECT_TRUE(json.is_null());

    json[keys[5]] = 2.2;
    json[keys[4]] = 1.1;
    json[keys[0]] = 1;
    json[keys[1]] = 2;
    json[keys[2]] = "String 1";
    json[keys[3]] = "String 2";
    EXPECT_EQ(json.count(), 6U);

    json1 = json.take(keys[2]);
    EXPECT_TRUE(json1.is_string());
    EXPECT_STREQ(json1.to<const char*>(), "String 1");
    EXPECT_EQ(cjson.count(), 5U);
    EXPECT_TRUE(cjson[keys[0]].is_integer());
    EXPECT_EQ(cjson[keys[0]].to<int>(), 1);
    EXPECT_TRUE(cjson[keys[1]].is_integer());
    EXPECT_EQ(cjson[keys[1]].to<int>(), 2);
    EXPECT_TRUE(cjson[keys[2]].is_null());
    EXPECT_TRUE(cjson[keys[3]].is_string());
    EXPECT_STREQ(cjson[keys[3]].to<const char*>(), "String 2");
    EXPECT_TRUE(cjson[keys[4]].is_double());
    EXPECT_EQ(cjson[keys[4]].to<double>(), 1.1);
    EXPECT_TRUE(cjson[keys[5]].is_double());
    EXPECT_EQ(cjson[keys[5]].to<double>(), 2.2);

    json1 = json.take(keys[4]);
    EXPECT_TRUE(json1.is_double());
    EXPECT_EQ(json1.to<double>(), 1.1);
    EXPECT_EQ(cjson.count(), 4U);
    EXPECT_TRUE(cjson[keys[0]].is_integer());
    EXPECT_EQ(cjson[keys[0]].to<int>(), 1);
    EXPECT_TRUE(cjson[keys[1]].is_integer());
    EXPECT_EQ(cjson[keys[1]].to<int>(), 2);
    EXPECT_TRUE(cjson[keys[2]].is_null());
    EXPECT_TRUE(cjson[keys[3]].is_string());
    EXPECT_STREQ(cjson[keys[3]].to<const char*>(), "String 2");
    EXPECT_TRUE(cjson[keys[4]].is_null());
    EXPECT_TRUE(cjson[keys[5]].is_double());
    EXPECT_EQ(cjson[keys[5]].to<double>(), 2.2);

    json1 = json.take(keys[1]);
    EXPECT_TRUE(json1.is_integer());
    EXPECT_EQ(json1.to<int>(), 2);
    EXPECT_EQ(cjson.count(), 3U);
    EXPECT_TRUE(cjson[keys[0]].is_integer());
    EXPECT_EQ(cjson[keys[0]].to<int>(), 1);
    EXPECT_TRUE(cjson[keys[1]].is_null());
    EXPECT_TRUE(cjson[keys[2]].is_null());
    EXPECT_TRUE(cjson[keys[3]].is_string());
    EXPECT_STREQ(cjson[keys[3]].to<const char*>(), "String 2");
    EXPECT_TRUE(cjson[keys[4]].is_null());
    EXPECT_TRUE(cjson[keys[5]].is_double());
    EXPECT_EQ(cjson[keys[5]].to<double>(), 2.2);

    json1 = json.take(keys[4]);
    EXPECT_TRUE(json1.is_null());
    EXPECT_EQ(cjson.count(), 3U);
    EXPECT_TRUE(cjson[keys[0]].is_integer());
    EXPECT_EQ(cjson[keys[0]].to<int>(), 1);
    EXPECT_TRUE(cjson[keys[1]].is_null());
    EXPECT_TRUE(cjson[keys[2]].is_null());
    EXPECT_TRUE(cjson[keys[3]].is_string());
    EXPECT_STREQ(cjson[keys[3]].to<const char*>(), "String 2");
    EXPECT_TRUE(cjson[keys[4]].is_null());
    EXPECT_TRUE(cjson[keys[5]].is_double());
    EXPECT_EQ(cjson[keys[5]].to<double>(), 2.2);

    json1 = json.take("Nonexistent key");
    EXPECT_TRUE(json1.is_null());
    EXPECT_EQ(cjson.count(), 3U);
    EXPECT_TRUE(cjson[keys[0]].is_integer());
    EXPECT_EQ(cjson[keys[0]].to<int>(), 1);
    EXPECT_TRUE(cjson[keys[1]].is_null());
    EXPECT_TRUE(cjson[keys[2]].is_null());
    EXPECT_TRUE(cjson[keys[3]].is_string());
    EXPECT_STREQ(cjson[keys[3]].to<const char*>(), "String 2");
    EXPECT_TRUE(cjson[keys[4]].is_null());
    EXPECT_TRUE(cjson[keys[5]].is_double());
    EXPECT_EQ(cjson[keys[5]].to<double>(), 2.2);

    json1 = json.take(keys[0]);
    EXPECT_TRUE(json1.is_integer());
    EXPECT_EQ(json1.to<int>(), 1);
    EXPECT_EQ(cjson.count(), 2U);
    json1 = json.take(keys[5]);
    EXPECT_TRUE(json1.is_double());
    EXPECT_EQ(json1.to<double>(), 2.2);
    EXPECT_EQ(cjson.count(), 1U);
    json1 = json.take(keys[3]);
    EXPECT_TRUE(json1.is_string());
    EXPECT_STREQ(json1.to<const char*>(), "String 2");
    EXPECT_EQ(cjson.count(), 0U);
}
