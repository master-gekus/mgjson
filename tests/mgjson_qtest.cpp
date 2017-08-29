#include <ctime>
#include <limits>

#include <QtTest>

#include "GJson.h"

#define SIMPLE_TEST_COUNT 10

Q_DECLARE_METATYPE(GJson::Type)
Q_DECLARE_METATYPE(GJsonParseError::ParseError)

namespace QTest
{
    template <>
    char* toString<GJsonParseError::ParseError>(const GJsonParseError::ParseError& err)
    {
        return qstrdup(GJsonParseError::error_string(err));
    }

    template <>
    char* toString<GJson::Type>(const GJson::Type& type)
    {
        switch (type) {
#define _case(a) case GJson::a: return qstrdup(#a)
        _case(Null);
        _case(Bool);
        _case(Integer);
        _case(Double);
        _case(String);
        _case(Array);
        _case(Object);
        _case(Undefined);
#undef _case
        default:                return qstrdup("<invalid>");
        }
    }

    //  inline bool qCompare(const GJson& json, const char* value, const char *actual,
//                       const char *expected, const char *file, int line)
//  {
//    GJson value_json;
//    GJsonParseError error;
//    value_json = GJson::fromJson(value, &error);
//    if (!qCompare(error.error, GJsonParseError::NoError, "Parse", "Parse",
//                  file, line))
//      return false;

//    return qCompare(json.toJson(GJson::MinSize).constData(),
//                    value_json.toJson(GJson::MinSize).constData(), actual,
//                    expected, file, line);
//  }
}

class GJsonTest : public QObject
{
    Q_OBJECT

private slots:

    // ConstructorFromType
    void ConstructorFromType_data()
    {
        QTest::addColumn<GJson::Type>("type");

#define _test(a) QTest::newRow(#a) << GJson::a
        _test(Null);
        _test(Bool);
        _test(Integer);
        _test(Double);
        _test(String);
        _test(Array);
        _test(Object);
        _test(Undefined);
#undef _test
    }
    void ConstructorFromType()
    {
        QFETCH(GJson::Type, type);

        GJson json1(type);
        QCOMPARE(json1.type(), type);

        GJson json2(json1);
        QCOMPARE(json2.type(), type);

        GJson json3;
        json3 = json1;
        QCOMPARE(json3.type(), type);
    }

    // ConstructorFromValue
#define CONSTRUCT_FROM_VALUE_TEST(name, value, expected_type, checker) \
    void ConstructorFromValue_##name() \
    { \
        GJson json1(value); \
        QCOMPARE(json1.type(), GJson::expected_type); \
        QVERIFY(json1.checker()); \
    }

    CONSTRUCT_FROM_VALUE_TEST(BoolTrue, true, Bool, isBool)
    CONSTRUCT_FROM_VALUE_TEST(BoolFalse, false, Bool, isBool)
    CONSTRUCT_FROM_VALUE_TEST(Short1, 1, Integer, isInteger)
    CONSTRUCT_FROM_VALUE_TEST(ShortMax, std::numeric_limits<short>::max(), Integer, isInteger)
    CONSTRUCT_FROM_VALUE_TEST(ShortMin, std::numeric_limits<short>::min(), Integer, isInteger)
    CONSTRUCT_FROM_VALUE_TEST(UShort1, 1U, Integer, isInteger)
    CONSTRUCT_FROM_VALUE_TEST(UShortMax, std::numeric_limits<unsigned short>::max(), Integer, isInteger)
    CONSTRUCT_FROM_VALUE_TEST(UShortMin, std::numeric_limits<unsigned short>::min(), Integer, isInteger)
    CONSTRUCT_FROM_VALUE_TEST(Int1, 1, Integer, isInteger)
    CONSTRUCT_FROM_VALUE_TEST(IntMax, std::numeric_limits<int>::max(), Integer, isInteger)
    CONSTRUCT_FROM_VALUE_TEST(IntMin, std::numeric_limits<int>::min(), Integer, isInteger)
    CONSTRUCT_FROM_VALUE_TEST(UInt1, 1U, Integer, isInteger)
    CONSTRUCT_FROM_VALUE_TEST(UIntMax, std::numeric_limits<unsigned int>::max(), Integer, isInteger)
    CONSTRUCT_FROM_VALUE_TEST(UIntMin, std::numeric_limits<unsigned int>::min(), Integer, isInteger)
    CONSTRUCT_FROM_VALUE_TEST(Long1, 1L, Integer, isInteger)
    CONSTRUCT_FROM_VALUE_TEST(LongMax, std::numeric_limits<long>::max(), Integer, isInteger)
    CONSTRUCT_FROM_VALUE_TEST(LongMin, std::numeric_limits<long>::min(), Integer, isInteger)
    CONSTRUCT_FROM_VALUE_TEST(ULong1, 1UL, Integer, isInteger)
    CONSTRUCT_FROM_VALUE_TEST(ULongMax, std::numeric_limits<unsigned long>::max(), Integer, isInteger)
    CONSTRUCT_FROM_VALUE_TEST(ULongMin, std::numeric_limits<unsigned long>::min(), Integer, isInteger)
    CONSTRUCT_FROM_VALUE_TEST(LongLong1, 1LL, Integer, isInteger)
    CONSTRUCT_FROM_VALUE_TEST(LongLongMax, std::numeric_limits<long long>::max(), Integer, isInteger)
    CONSTRUCT_FROM_VALUE_TEST(LongLongMin, std::numeric_limits<long long>::min(), Integer, isInteger)
    CONSTRUCT_FROM_VALUE_TEST(ULongLong1, 1LL, Integer, isInteger)
    CONSTRUCT_FROM_VALUE_TEST(ULongLongMax, std::numeric_limits<long long>::max(), Integer, isInteger)
    CONSTRUCT_FROM_VALUE_TEST(ULongLongMin, std::numeric_limits<long long>::min(), Integer, isInteger)

};

QTEST_APPLESS_MAIN(GJsonTest)

#include "mgjson_qtest.moc"
