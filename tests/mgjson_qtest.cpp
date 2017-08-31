#include <ctime>
#include <cmath>
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
#define CONSTRUCT_FROM_VALUE_TEST(name, value, expected_type, checker, getter) \
    void ConstructorFromValue_##name() \
    { \
        GJson json1(value); \
        QCOMPARE(json1.type(), GJson::expected_type); \
        QVERIFY(json1.checker()); \
        QCOMPARE(json1.getter(), value); \
    \
        GJson json2(json1); \
        QCOMPARE(json2.type(), GJson::expected_type); \
        QVERIFY(json2.checker()); \
        QCOMPARE(json2.getter(), value); \
    }

#define CONSTRUCT_FROM_INT_VALUE_TESTS(type_name, type, value) \
    CONSTRUCT_FROM_VALUE_TEST(type_name##Value, ((type)value), Integer, isInteger, to##type_name) \
    CONSTRUCT_FROM_VALUE_TEST(type_name##Min, std::numeric_limits<type>::min(), Integer, isInteger, to##type_name) \
    CONSTRUCT_FROM_VALUE_TEST(type_name##Max, std::numeric_limits<type>::max(), Integer, isInteger, to##type_name)

#define CONSTRUCT_FROM_DBL_VALUE_TESTS(type_name, type) \
    CONSTRUCT_FROM_VALUE_TEST(type_name##Pi, ((type)std::acos(((type)(-1.0)))), Double, isDouble, to##type_name) \
    CONSTRUCT_FROM_VALUE_TEST(type_name##Min, std::numeric_limits<type>::min(), Double, isDouble, to##type_name) \
    CONSTRUCT_FROM_VALUE_TEST(type_name##Max, std::numeric_limits<type>::max(), Double, isDouble, to##type_name)

    CONSTRUCT_FROM_VALUE_TEST(BoolTrue, true, Bool, isBool, toBool)
    CONSTRUCT_FROM_VALUE_TEST(BoolFalse, false, Bool, isBool, toBool)
    CONSTRUCT_FROM_INT_VALUE_TESTS(Char, char, 1)
    CONSTRUCT_FROM_INT_VALUE_TESTS(UChar, unsigned char, 1)
    CONSTRUCT_FROM_INT_VALUE_TESTS(Short, short, 1)
    CONSTRUCT_FROM_INT_VALUE_TESTS(UShort, unsigned short, 1)
    CONSTRUCT_FROM_INT_VALUE_TESTS(Int, int, 1)
    CONSTRUCT_FROM_INT_VALUE_TESTS(UInt, unsigned int, 1)
    CONSTRUCT_FROM_INT_VALUE_TESTS(Long, long, 1)
    CONSTRUCT_FROM_INT_VALUE_TESTS(ULong, unsigned long, 1)
    CONSTRUCT_FROM_INT_VALUE_TESTS(LongLong, long long, 1)
    CONSTRUCT_FROM_INT_VALUE_TESTS(ULongLong, unsigned long long, 1)

    CONSTRUCT_FROM_DBL_VALUE_TESTS(Float, float)
    CONSTRUCT_FROM_DBL_VALUE_TESTS(Double, double)
    CONSTRUCT_FROM_DBL_VALUE_TESTS(LongDouble, long double)

    void ConstructorFromValue_ConstChar()
    {
        static const char test_string[]="Test string.";

        GJson json1(test_string);
        QCOMPARE(json1.type(), GJson::String);
        QVERIFY(json1.isString());
        QCOMPARE(json1.toStr(), test_string);

        GJson json2(json1);
        QCOMPARE(json2.type(), GJson::String);
        QVERIFY(json2.isString());
        QCOMPARE(json2.toStr(), test_string);
    }

    void ConstructorFromValue_StdString()
    {
        static const std::string test_string("Test string.");

        GJson json1(test_string);
        QCOMPARE(json1.type(), GJson::String);
        QVERIFY(json1.isString());
        QCOMPARE(json1.toStdString(), test_string);

        GJson json2(json1);
        QCOMPARE(json2.type(), GJson::String);
        QVERIFY(json2.isString());
        QCOMPARE(json2.toStdString(), test_string);
    }

    void ConstructorFromValue_StdStringWithZeros()
    {
        static const char test_string_data[]="Data before zero\0Data after zero";
        static const std::string test_string(test_string_data, sizeof(test_string_data) - 1);
        QVERIFY(std::string(test_string_data) != test_string);

        GJson json1(test_string);
        QCOMPARE(json1.type(), GJson::String);
        QVERIFY(json1.isString());
        QCOMPARE(json1.toStdString(), test_string);

        GJson json2(json1);
        QCOMPARE(json2.type(), GJson::String);
        QVERIFY(json2.isString());
        QCOMPARE(json2.toStdString(), test_string);
        QCOMPARE(json2.toStdString().size(), sizeof(test_string_data) - 1);
    }

    void ConstructorFromValue_QByteArray()
    {
        static const char test_string_data[]="Data before zero\0Data after zero";
        static const QByteArray test_string(test_string_data, sizeof(test_string_data) - 1);
        QVERIFY(QByteArray(test_string_data) != test_string);

        GJson json1(test_string);
        QCOMPARE(json1.type(), GJson::String);
        QVERIFY(json1.isString());
        QCOMPARE(json1.toByteArray(), test_string);

        GJson json2(json1);
        QCOMPARE(json2.type(), GJson::String);
        QVERIFY(json2.isString());
        QCOMPARE(json2.toByteArray(), test_string);
        QCOMPARE(json2.toByteArray().size(), static_cast<int>(sizeof(test_string_data) - 1));
    }

    void ConstructorFromValue_QString()
    {
        static const char test_string_data[]="Data before zero\0Data after zero";
        static const QString test_string(QString::fromUtf8(test_string_data,
                                                           sizeof(test_string_data) - 1));
        QVERIFY(QString::fromUtf8(test_string_data) != test_string);

        GJson json1(test_string);
        QCOMPARE(json1.type(), GJson::String);
        QVERIFY(json1.isString());
        QCOMPARE(json1.toString(), test_string);

        GJson json2(json1);
        QCOMPARE(json2.type(), GJson::String);
        QVERIFY(json2.isString());
        QCOMPARE(json2.toString(), test_string);
        QCOMPARE(json2.toString().size(), static_cast<int>(sizeof(test_string_data) - 1));
    }
};

QTEST_APPLESS_MAIN(GJsonTest)

#include "mgjson_qtest.moc"
