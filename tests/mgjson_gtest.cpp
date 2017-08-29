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
