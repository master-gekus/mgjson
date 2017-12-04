#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif

#include "GJson.h"

#include <ctime>
#include <cmath>
#include <limits>

#include <QtTest>

Q_DECLARE_METATYPE(GJson::Type)
Q_DECLARE_METATYPE(GJsonParseError::ParseError)
Q_DECLARE_METATYPE(long double)

namespace QTest
{
    template <>
    char* toString<long double>(const long double& value)
    {
        char buf[std::numeric_limits<long double>::digits10 + 10];
#ifdef _MSC_VER
        sprintf_s(buf, sizeof(buf), "%.*Lg",
                  std::numeric_limits<long double>::digits10 + 2, value);
#else
        sprintf(buf, "%.*Lg",
                std::numeric_limits<long double>::digits10 + 2, value);
#endif
        return qstrdup(buf);
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

    bool almost_equal(const long double& x, const long double& y)
    {
        return (std::abs(x-y) < (std::numeric_limits<long double>::epsilon() * std::abs(x+y)))
                || (std::abs(x-y) < std::numeric_limits<long double>::min());
    }

    template<>
    inline bool qCompare(const long double& t1, const long double& t2, const char *actual,
                         const char *expected, const char *file, int line)
    {
        return compare_helper(almost_equal(t1, t2), "Compared values are not the same",
                              toString(t1), toString(t2), actual, expected, file, line);
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

#define CONVERSION_TEST(n,t,v) \
    void ConvertionTest_##n() \
    { \
        t value = v; \
        GJson json(value); \
        QCOMPARE(json.to<t>(), value); \
        QCOMPARE(static_cast<t>(json), value); \
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
    CONVERSION_TEST(string, const  char*, "Test string")

    void ConvertionTest_std_string()
    {
        std::string value = "Test string";
        GJson json(value);
        QCOMPARE(json.to<std::string>(), value);
    }

    void ConvertionTest_QString()
    {
        QString value = QStringLiteral("Test string");
        GJson json(value);
        QCOMPARE(json.to<QString>(), value);
    }

    void ConvertionTest_QByteArray()
    {
        QByteArray value = QByteArrayLiteral("Test string");
        GJson json(value);
        QCOMPARE(json.to<QByteArray>(), value);
    }

    void ConvertionTest_QByteArray_ref()
    {
        QByteArray value = QByteArrayLiteral("Test string");
        GJson json(value);
        QCOMPARE(json.to<const QByteArray&>(), value);
        QCOMPARE(static_cast<const QByteArray&>(json), value);
    }

// StringValueCast
    void StringValueCast_data()
    {
        QTest::addColumn<QByteArray>("string");
        QTest::addColumn<bool>("bvalue");
        QTest::addColumn<unsigned long long>("ivalue");
        QTest::addColumn<long double>("dvalue");

#define _test(s,b,i,d) \
        QTest::newRow(s) << QByteArrayLiteral(s) << b \
            << static_cast<unsigned long long>(i) \
            << static_cast<long double>(d)

        _test("Not a value", false, 0, 0);
        _test("0", false, 0, 0);
        _test("Off", false, 0, 0);
        _test("False", false, 0, 0);
        _test("On", true, 1, 1);
        _test("True", true, 1, 1);
        _test("1", true, 1, 1);
        _test("-1", true, -1, -1);
        _test("0x10", true, 16, 16);
        _test("1E100", true, std::numeric_limits<unsigned long long>::max(), 1E100L);
        _test("-1E100", true, std::numeric_limits<long long>::min(), -1E100L);
#ifdef _MSC_VER
        // In MSVC "long double" is the same as "double"
        _test("1E300", true, std::numeric_limits<unsigned long long>::max(), 1E300L);
        _test("-1E300", true, std::numeric_limits<long long>::min(), -1E300L);
#else
        _test("1E1000", true, std::numeric_limits<unsigned long long>::max(), 1E1000L);
        _test("-1E1000", true, std::numeric_limits<long long>::min(), -1E1000L);
#endif
        _test(".123456789012345678901234567890", false, 0, .123456789012345678901234567890L);
        _test("-.123456789012345678901234567890", false, 0, -.123456789012345678901234567890L);
        _test(".123456789012345678901234567890E100", true, std::numeric_limits<unsigned long long>::max(), .123456789012345678901234567890E100L);
        _test(".123456789012345678901234567890E-100", false, 0, .123456789012345678901234567890E-100L);
        _test("12345678901234.5678901234567890", true, 12345678901234, 12345678901234.5678901234567890L);
        _test("-12345678901234.5678901234567890", true, -12345678901234, -12345678901234.5678901234567890L);
#undef _test
    }

    void StringValueCast()
    {
        QFETCH(QByteArray, string);

        GJson json(string);
        QVERIFY(json.isString());
        QCOMPARE(json.toByteArray(), string);
        QTEST(json.toBool(), "bvalue");
        QTEST(json.toULongLong(), "ivalue");
        QTEST(json.toLongDouble(), "dvalue");
    }

    void CountAndResizeSimple_data()
    {
        QTest::addColumn<GJson>("json");
#define _test(a) QTest::newRow(#a) << GJson(GJson::a)
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

    void CountAndResizeSimple()
    {
        QFETCH(GJson, json);
        QCOMPARE(json.count(), 0);

        json.resize(0);
        QCOMPARE(json.count(), 0);

        json.resize(10);
        QCOMPARE(json.count(), 10);

        json.resize(5);
        QCOMPARE(json.count(), 5);

        json.resize(0);
        QCOMPARE(json.count(), 0);
    }

    void ArrayAt()
    {
        GJson json;
        json.resize(10);
        QCOMPARE(json.count(), 10);

        GJson json2;
        json2 = json;
        QCOMPARE(json2.count(), 10);

        for (int i = 0; i < json.count(); i++) {
            GJson j1 = json.at(i);
            QCOMPARE(j1.type(), GJson::Null);

            GJson& j2 = json2.at(i);
            QCOMPARE(j2.type(), GJson::Null);

            j2 = 1;
            QCOMPARE(j2.type(), GJson::Integer);
            QCOMPARE(j2.to<int>(), 1);

            GJson j3 = json.at(i);
            QCOMPARE(j3.type(), GJson::Null);

            GJson& j4 = json2.at(i);
            QCOMPARE(j4.type(), GJson::Integer);
            QCOMPARE(j4.to<int>(), 1);
        }

        QVERIFY(static_cast<const GJson&>(json).at(11).isNull());
        QVERIFY_EXCEPTION_THROWN(json2.at(11) = 1, std::out_of_range);
    }

    void ArrayAtAutoGrow()
    {
        GJson json;
        QCOMPARE(json.count(), 0);
        QVERIFY(static_cast<const GJson&>(json).at(0).isNull());

        json.at(0) = 1;
        QCOMPARE(json.count(), 1);
        QCOMPARE(json.at(0).type(), GJson::Integer);
        QCOMPARE(json.at(0).to<int>(), 1);
        QVERIFY_EXCEPTION_THROWN(json.at(2) = 1, std::out_of_range);

        json.at(1) = 2;
        QCOMPARE(json.count(), 2);
        QCOMPARE(json.at(1).type(), GJson::Integer);
        QCOMPARE(json.at(1).to<int>(), 2);
        QVERIFY_EXCEPTION_THROWN(json.at(3) = 1, std::out_of_range);

        json.at(0) = "Test string";
        QCOMPARE(json.count(), 2);
        QCOMPARE(json.at(0).type(), GJson::String);
        QCOMPARE(json.at(0).to<const char*>(), "Test string");
        QCOMPARE(json.at(1).type(), GJson::Integer);
        QCOMPARE(json.at(1).to<int>(), 2);
    }

    void ArrayAtException_data()
    {
        QTest::addColumn<GJson>("json");
        QTest::addColumn<bool>("must_throw");
#define _test(a,t) QTest::newRow(#a) << GJson(GJson::a) << t
        _test(Null, false);
        _test(Bool, true);
        _test(Integer, true);
        _test(Double, true);
        _test(String, true);
        _test(Array, false);
        _test(Object, true);
        _test(Undefined, false);
#undef _test
    }

    void ArrayAtException()
    {
        QFETCH(GJson, json);
        QFETCH(bool, must_throw);

        if (must_throw) {
            QVERIFY_EXCEPTION_THROWN(json.at(0) = 1, std::invalid_argument);
        } else {
            json.at(0) = 1;
            QCOMPARE(json.count(), 1);
            QCOMPARE(json.at(0).type(), GJson::Integer);
            QCOMPARE(json.at(0).to<int>(), 1);
        }
    }

    void ObjectAt()
    {
        GJson json;
        QCOMPARE(json.count(), 0);

        for (int i = 0; i < 10; i++) {
            char key1[10];
            sprintf(key1, "Key %d", i);
            std::string key2(key1);
            const GJson& cjson = json;
            QVERIFY(cjson.at(key1).isNull());
            QVERIFY(cjson.at(key2).isNull());
            json.at(key1) = i;
            QCOMPARE(json.count(), i + 1);
            QVERIFY(cjson.at(key1).isInteger());
            QVERIFY(cjson.at(key2).isInteger());
            QCOMPARE(cjson.at(key1).to<int>(), i);
            QCOMPARE(cjson.at(key2).to<int>(), i);
        }
        QCOMPARE(json.count(), 10);

        GJson json2;
        json2 = json;
        for (int i = 10; i < 20; i++) {
            char key1[10];
            sprintf(key1, "Key %d", i);
            std::string key2(key1);
            const GJson& cjson = json2;
            QVERIFY(cjson.at(key1).isNull());
            QVERIFY(cjson.at(key2).isNull());
            json2.at(key1) = key2;
            QCOMPARE(json2.count(), i + 1);
            QVERIFY(cjson.at(key1).isString());
            QVERIFY(cjson.at(key2).isString());
            QCOMPARE(cjson.at(key1).to<const char *>(), key1);
            QCOMPARE(cjson.at(key2).to<const char *>(), key1);
        }
        QCOMPARE(json.count(), 10);
        QCOMPARE(json2.count(), 20);

        for (int i = 0; i < 10; i++) {
            char key1[10];
            sprintf(key1, "Key %d", i);
            std::string key2(key1);
            const GJson& cjson = json;
            QVERIFY(!cjson.at(key1).isNull());
            QVERIFY(!cjson.at(key2).isNull());
            json.at(key1) = i + 10;
            QVERIFY(cjson.at(key1).isInteger());
            QVERIFY(cjson.at(key2).isInteger());
            QCOMPARE(cjson.at(key1).to<int>(), i + 10);
            QCOMPARE(cjson.at(key2).to<int>(), i + 10);
        }
        QCOMPARE(json.count(), 10);
        QCOMPARE(json2.count(), 20);

        for (int i = 0; i < 10; i++) {
            char key1[10];
            sprintf(key1, "Key %d", i);
            GJson& element = json2.at(key1);
            QVERIFY(element.isInteger());
            QCOMPARE(element.to<int>(), i);
            element = key1;
            QVERIFY(static_cast<const GJson&>(json2).at(key1).isString());
            QCOMPARE(static_cast<const GJson&>(json2).at(key1).to<const char *>(), key1);
        }
        QCOMPARE(json.count(), 10);
        QCOMPARE(json2.count(), 20);

        for (int i = 0; i < 10; i++) {
            char key1[10];
            sprintf(key1, "Key %d", i);
            GJson element = json2.at(key1);
            QVERIFY(element.isString());
            QCOMPARE(element.to<const char*>(), key1);
            element = i + 100;
            QVERIFY(element.isInteger());
            QCOMPARE(element.to<int>(), i + 100);
            QVERIFY(json2.at(key1).isString());
            QCOMPARE(json2.at(key1).to<const char *>(), key1);
            QVERIFY(static_cast<const GJson&>(json2).at(key1).isString());
            QCOMPARE(static_cast<const GJson&>(json2).at(key1).to<const char *>(), key1);
        }
        QCOMPARE(json.count(), 10);
        QCOMPARE(json2.count(), 20);

        // Qt special
        for (int i = 0; i < 20; i++) {
            QString key1 = QStringLiteral("Key %1").arg(i);
            QByteArray key2 = QByteArrayLiteral("Key ") + QByteArray::number(i);
            QCOMPARE(json2.at(key1).to<QString>(), key1);
            QCOMPARE(json2.at(key2).to<QByteArray>(), key2);
            const GJson& cjson = json2;
            QCOMPARE(cjson.at(key1).to<QString>(), key1);
            QCOMPARE(cjson.at(key2).to<QByteArray>(), key2);
        }
    }

    void ObjectAtEmptyKey()
    {
        GJson json;
        json["Test"] = 1;
        QVERIFY(json.isObject());
        QVERIFY(json["Test"].isInteger());
        QCOMPARE(json["Test"].toInt(), 1);
        QVERIFY_EXCEPTION_THROWN(json[nullptr] = 1, std::out_of_range);
        QVERIFY_EXCEPTION_THROWN(json[""] = 1, std::out_of_range);

        // Qt special
        QVERIFY_EXCEPTION_THROWN(json[QByteArray()] = 1, std::out_of_range);
        QVERIFY_EXCEPTION_THROWN(json[QString()] = 1, std::out_of_range);
    }

    void ObjectAtException_data()
    {
        QTest::addColumn<GJson>("json");
        QTest::addColumn<bool>("must_throw");
#define _test(a,t) QTest::newRow(#a) << GJson(GJson::a) << t
        _test(Null, false);
        _test(Bool, true);
        _test(Integer, true);
        _test(Double, true);
        _test(String, true);
        _test(Array, true);
        _test(Object, false);
        _test(Undefined, false);
#undef _test
    }

    void ObjectAtException()
    {
        QFETCH(GJson, json);
        QFETCH(bool, must_throw);
        QCOMPARE(json.count(), 0);

        if (must_throw) {
            QVERIFY_EXCEPTION_THROWN(json.at("key") = 1, std::invalid_argument);
        } else {
            json.at("key") = 1;
            QCOMPARE(json.count(), 1);
            QCOMPARE(json.at("key").type(), GJson::Integer);
            QCOMPARE(json.at("key").to<int>(), 1);
        }
    }

    void Keys_data()
    {
        QTest::addColumn<QByteArrayList>("src");
        QTest::addColumn<QByteArrayList>("dst");
        QTest::newRow("Empty") << QByteArrayList{} << QByteArrayList{};
        QTest::newRow("One key") << QByteArrayList{"Key"} << QByteArrayList{};
        QTest::newRow("Two same") << QByteArrayList{"Key", "Key"} << QByteArrayList{};
        QTest::newRow("Two straight") << QByteArrayList{"Key 0", "Key 1"} << QByteArrayList{"Key 0", "Key 1"};
        QTest::newRow("Two reverse") << QByteArrayList{"Key 1", "Key 0"} << QByteArrayList{"Key 0", "Key 1"};
        QTest::newRow("Three repeated 1") << QByteArrayList{"Key 0", "Key 1", "Key 0"} << QByteArrayList{"Key 0", "Key 1"};
        QTest::newRow("Three repeated 2") << QByteArrayList{"Key 1", "Key 0", "Key 1"} << QByteArrayList{"Key 0", "Key 1"};
        QTest::newRow("Many") << QByteArrayList{"Key 0", "Key 1", "Key 0", "Key 2", "Key 4", "Key 3", "Key 2"} << QByteArrayList{};
    }

    void Keys()
    {
        QFETCH(QByteArrayList, src);
        QFETCH(QByteArrayList, dst);

        if (dst.isEmpty() && (!src.isEmpty())) {
            std::set<QByteArray> dst_set;
            for (const auto& key : src) {
                dst_set.insert(key);
            }
            for (const auto& key : dst_set) {
                dst.push_back(key);
            }
        }

        GJson json;
        int value = 1;
        for (const auto& key : src) {
            json[key] = value++;
        }

        QCOMPARE(json.keys(), dst);
    }

    void Append()
    {
        GJson json;
        QVERIFY(json.isNull());

        for (int i = 0; i < 10; i++) {
            GJson &j1 = json.append();
            QVERIFY(json.isArray());
            QCOMPARE(json.count(), i * 2 + 1);
            QVERIFY(j1.isNull());
            QVERIFY(json[i * 2].isNull());
            j1 = i;
            QVERIFY(json[i * 2].isInteger());
            QCOMPARE(json[i * 2].to<int>(), i);
            json[i * 2] = "Test string";
            QVERIFY(j1.isString());
            QCOMPARE(j1.to<const char*>(), "Test string");

            GJson &j2 = json.append("Test string 2");
            QCOMPARE(json.count(), i * 2 + 2);
            QVERIFY(j2.isString());
            QVERIFY(json[i * 2 + 1].isString());
            QCOMPARE(j2.to<const char*>(), "Test string 2");
            QCOMPARE(json[i * 2 + 1].to<const char*>(), "Test string 2");

            j2 = i * 10;
            QVERIFY(json[i * 2 + 1].isInteger());
            QCOMPARE(json[i * 2 + 1].to<int>(), i * 10);
            json[i * 2 + 1] = "Test string";
            QVERIFY(j2.isString());
            QCOMPARE(j2.to<const char*>(), "Test string");
        }
    }

    void AppendException_data()
    {
        ArrayAtException_data();
    }

    void AppendException()
    {
        QFETCH(GJson, json);
        QFETCH(bool, must_throw);
        QCOMPARE(json.count(), 0);

        if (must_throw) {
            QVERIFY_EXCEPTION_THROWN(json.append(1), std::invalid_argument);
        } else {
            json.append(1);
            QCOMPARE(json.count(), 1);
            QVERIFY(json[0].isInteger());
            QCOMPARE(json[0].to<int>(), 1);
        }
    }

    void Prepend()
    {
        GJson json;
        QVERIFY(json.isNull());

        for (int i = 0; i < 10; i++) {
            GJson &j1 = json.prepend();
            QVERIFY(json.isArray());
            QCOMPARE(json.count(), i * 2 + 1);
            QVERIFY(j1.isNull());
            QVERIFY(json[0].isNull());
            j1 = i;
            QVERIFY(json[0].isInteger());
            QCOMPARE(json[0].to<int>(), i);
            json[0] = "Test string";
            QVERIFY(j1.isString());
            QCOMPARE(j1.to<const char*>(), "Test string");

            GJson &j2 = json.prepend("Test string 2");
            QCOMPARE(json.count(), i * 2 + 2);
            QVERIFY(j2.isString());
            QVERIFY(json[0].isString());
            QCOMPARE(j2.to<const char*>(), "Test string 2");
            QCOMPARE(json[0].to<const char*>(), "Test string 2");

            j2 = i * 10;
            QVERIFY(json[0].isInteger());
            QCOMPARE(json[0].to<int>(), i * 10);
            json[0] = "Test string";
            QVERIFY(j2.isString());
            QCOMPARE(j2.to<const char*>(), "Test string");
        }
    }

    void PrependException_data()
    {
        ArrayAtException_data();
    }

    void PrependException()
    {
        QFETCH(GJson, json);
        QFETCH(bool, must_throw);
        QCOMPARE(json.count(), 0);

        if (must_throw) {
            QVERIFY_EXCEPTION_THROWN(json.prepend(1), std::invalid_argument);
        } else {
            json.prepend(1);
            QCOMPARE(json.count(), 1);
            QVERIFY(json[0].isInteger());
            QCOMPARE(json[0].to<int>(), 1);
        }
    }

    void ArrayRemoveAt()
    {
        GJson json;
        QVERIFY(json.isNull());

        QVERIFY(json.append(1).isInteger());
        QVERIFY(json.append(2).isInteger());
        QVERIFY(json.append("String 1").isString());
        QVERIFY(json.append("String 2").isString());
        QVERIFY(json.append(1.1).isDouble());
        QVERIFY(json.append(2.2).isDouble());
        QCOMPARE(json.count(), 6);

        json.removeAt(2);
        QCOMPARE(json.count(), 5);
        QVERIFY(json[0].isInteger());
        QCOMPARE(json[0].to<int>(), 1);
        QVERIFY(json[1].isInteger());
        QCOMPARE(json[1].to<int>(), 2);
        QVERIFY(json[2].isString());
        QCOMPARE(json[2].to<const char*>(), "String 2");
        QVERIFY(json[3].isDouble());
        QCOMPARE(json[3].to<double>(), 1.1);
        QVERIFY(json[4].isDouble());
        QCOMPARE(json[4].to<double>(), 2.2);

        json.removeAt(3);
        QCOMPARE(json.count(), 4);
        QVERIFY(json[0].isInteger());
        QCOMPARE(json[0].to<int>(), 1);
        QVERIFY(json[1].isInteger());
        QCOMPARE(json[1].to<int>(), 2);
        QVERIFY(json[2].isString());
        QCOMPARE(json[2].to<const char*>(), "String 2");
        QVERIFY(json[3].isDouble());
        QCOMPARE(json[3].to<double>(), 2.2);

        json.removeAt(1);
        QCOMPARE(json.count(), 3);
        QVERIFY(json[0].isInteger());
        QCOMPARE(json[0].to<int>(), 1);
        QVERIFY(json[1].isString());
        QCOMPARE(json[1].to<const char*>(), "String 2");
        QVERIFY(json[2].isDouble());
        QCOMPARE(json[2].to<double>(), 2.2);

        json.removeAt(2);
        QCOMPARE(json.count(), 2);
        json.removeAt(1);
        QCOMPARE(json.count(), 1);
        json.removeAt(0);
        QCOMPARE(json.count(), 0);
    }

    void ObjectRemoveAt()
    {
        static const QByteArray keys[] = {
            "Key 1", "Key 2", "Key 3", "Key 4", "Key 5", "Key 6"
        };

        GJson json;
        const GJson& cjson = json;
        QVERIFY(json.isNull());

        json[keys[5]] = 2.2;
        json[keys[4]] = 1.1;
        json[keys[0]] = 1;
        json[keys[1]] = 2;
        json[keys[2]] = "String 1";
        json[keys[3]] = "String 2";
        QCOMPARE(json.count(), 6);

        json.removeAt(keys[2]);
        QCOMPARE(cjson.count(), 5);
        QVERIFY(cjson[keys[0]].isInteger());
        QCOMPARE(cjson[keys[0]].to<int>(), 1);
        QVERIFY(cjson[keys[1]].isInteger());
        QCOMPARE(cjson[keys[1]].to<int>(), 2);
        QVERIFY(cjson[keys[2]].isNull());
        QVERIFY(cjson[keys[3]].isString());
        QCOMPARE(cjson[keys[3]].to<const char*>(), "String 2");
        QVERIFY(cjson[keys[4]].isDouble());
        QCOMPARE(cjson[keys[4]].to<double>(), 1.1);
        QVERIFY(cjson[keys[5]].isDouble());
        QCOMPARE(cjson[keys[5]].to<double>(), 2.2);

        json.removeAt(keys[4]);
        QCOMPARE(cjson.count(), 4);
        QVERIFY(cjson[keys[0]].isInteger());
        QCOMPARE(cjson[keys[0]].to<int>(), 1);
        QVERIFY(cjson[keys[1]].isInteger());
        QCOMPARE(cjson[keys[1]].to<int>(), 2);
        QVERIFY(cjson[keys[2]].isNull());
        QVERIFY(cjson[keys[3]].isString());
        QCOMPARE(cjson[keys[3]].to<const char*>(), "String 2");
        QVERIFY(cjson[keys[4]].isNull());
        QVERIFY(cjson[keys[5]].isDouble());
        QCOMPARE(cjson[keys[5]].to<double>(), 2.2);

        json.removeAt(keys[1]);
        QCOMPARE(cjson.count(), 3);
        QVERIFY(cjson[keys[0]].isInteger());
        QCOMPARE(cjson[keys[0]].to<int>(), 1);
        QVERIFY(cjson[keys[1]].isNull());
        QVERIFY(cjson[keys[2]].isNull());
        QVERIFY(cjson[keys[3]].isString());
        QCOMPARE(cjson[keys[3]].to<const char*>(), "String 2");
        QVERIFY(cjson[keys[4]].isNull());
        QVERIFY(cjson[keys[5]].isDouble());
        QCOMPARE(cjson[keys[5]].to<double>(), 2.2);

        json.removeAt(keys[4]);
        QCOMPARE(cjson.count(), 3);
        QVERIFY(cjson[keys[0]].isInteger());
        QCOMPARE(cjson[keys[0]].to<int>(), 1);
        QVERIFY(cjson[keys[1]].isNull());
        QVERIFY(cjson[keys[2]].isNull());
        QVERIFY(cjson[keys[3]].isString());
        QCOMPARE(cjson[keys[3]].to<const char*>(), "String 2");
        QVERIFY(cjson[keys[4]].isNull());
        QVERIFY(cjson[keys[5]].isDouble());
        QCOMPARE(cjson[keys[5]].to<double>(), 2.2);

        json.removeAt("Nonexistent key");
        QCOMPARE(cjson.count(), 3);
        QVERIFY(cjson[keys[0]].isInteger());
        QCOMPARE(cjson[keys[0]].to<int>(), 1);
        QVERIFY(cjson[keys[1]].isNull());
        QVERIFY(cjson[keys[2]].isNull());
        QVERIFY(cjson[keys[3]].isString());
        QCOMPARE(cjson[keys[3]].to<const char*>(), "String 2");
        QVERIFY(cjson[keys[4]].isNull());
        QVERIFY(cjson[keys[5]].isDouble());
        QCOMPARE(cjson[keys[5]].to<double>(), 2.2);

        json.removeAt(keys[0]);
        QCOMPARE(cjson.count(), 2);
        json.removeAt(keys[5]);
        QCOMPARE(cjson.count(), 1);
        json.removeAt(keys[3]);
        QCOMPARE(cjson.count(), 0);
    }

    void ArrayTakeAt()
    {
        GJson json, json1;
        QVERIFY(json.isNull());

        QVERIFY(json.append(1).isInteger());
        QVERIFY(json.append(2).isInteger());
        QVERIFY(json.append("String 1").isString());
        QVERIFY(json.append("String 2").isString());
        QVERIFY(json.append(1.1).isDouble());
        QVERIFY(json.append(2.2).isDouble());
        QCOMPARE(json.count(), 6);

        json1 = json.takeAt(2);
        QVERIFY(json1.isString());
        QCOMPARE(json1.to<const char*>(), "String 1");
        QCOMPARE(json.count(), 5);
        QVERIFY(json[0].isInteger());
        QCOMPARE(json[0].to<int>(), 1);
        QVERIFY(json[1].isInteger());
        QCOMPARE(json[1].to<int>(), 2);
        QVERIFY(json[2].isString());
        QCOMPARE(json[2].to<const char*>(), "String 2");
        QVERIFY(json[3].isDouble());
        QCOMPARE(json[3].to<double>(), 1.1);
        QVERIFY(json[4].isDouble());
        QCOMPARE(json[4].to<double>(), 2.2);

        json1 = json.takeAt(3);
        QVERIFY(json1.isDouble());
        QCOMPARE(json1.to<double>(), 1.1);
        QCOMPARE(json.count(), 4);
        QVERIFY(json[0].isInteger());
        QCOMPARE(json[0].to<int>(), 1);
        QVERIFY(json[1].isInteger());
        QCOMPARE(json[1].to<int>(), 2);
        QVERIFY(json[2].isString());
        QCOMPARE(json[2].to<const char*>(), "String 2");
        QVERIFY(json[3].isDouble());
        QCOMPARE(json[3].to<double>(), 2.2);

        json1 = json.takeAt(1);
        QVERIFY(json1.isInteger());
        QCOMPARE(json1.to<int>(), 2);
        QCOMPARE(json.count(), 3);
        QVERIFY(json[0].isInteger());
        QCOMPARE(json[0].to<int>(), 1);
        QVERIFY(json[1].isString());
        QCOMPARE(json[1].to<const char*>(), "String 2");
        QVERIFY(json[2].isDouble());
        QCOMPARE(json[2].to<double>(), 2.2);

        json1 = json.takeAt(2);
        QCOMPARE(json.count(), 2);
        json1 = json.takeAt(1);
        QCOMPARE(json.count(), 1);
        json1 = json.takeAt(0);
        QCOMPARE(json.count(), 0);
    }

    void ObjectTakeAt()
    {
        static QString keys[] = {
            "Key 1", "Key 2", "Key 3", "Key 4", "Key 5", "Key 6"
        };

        GJson json, json1;
        const GJson& cjson = json;
        QVERIFY(json.isNull());

        json[keys[5]] = 2.2;
        json[keys[4]] = 1.1;
        json[keys[0]] = 1;
        json[keys[1]] = 2;
        json[keys[2]] = "String 1";
        json[keys[3]] = "String 2";
        QCOMPARE(json.count(), 6);

        json1 = json.takeAt(keys[2]);
        QVERIFY(json1.isString());
        QCOMPARE(json1.to<const char*>(), "String 1");
        QCOMPARE(cjson.count(), 5);
        QVERIFY(cjson[keys[0]].isInteger());
        QCOMPARE(cjson[keys[0]].to<int>(), 1);
        QVERIFY(cjson[keys[1]].isInteger());
        QCOMPARE(cjson[keys[1]].to<int>(), 2);
        QVERIFY(cjson[keys[2]].isNull());
        QVERIFY(cjson[keys[3]].isString());
        QCOMPARE(cjson[keys[3]].to<const char*>(), "String 2");
        QVERIFY(cjson[keys[4]].isDouble());
        QCOMPARE(cjson[keys[4]].to<double>(), 1.1);
        QVERIFY(cjson[keys[5]].isDouble());
        QCOMPARE(cjson[keys[5]].to<double>(), 2.2);

        json1 = json.takeAt(keys[4]);
        QVERIFY(json1.isDouble());
        QCOMPARE(json1.to<double>(), 1.1);
        QCOMPARE(cjson.count(), 4);
        QVERIFY(cjson[keys[0]].isInteger());
        QCOMPARE(cjson[keys[0]].to<int>(), 1);
        QVERIFY(cjson[keys[1]].isInteger());
        QCOMPARE(cjson[keys[1]].to<int>(), 2);
        QVERIFY(cjson[keys[2]].isNull());
        QVERIFY(cjson[keys[3]].isString());
        QCOMPARE(cjson[keys[3]].to<const char*>(), "String 2");
        QVERIFY(cjson[keys[4]].isNull());
        QVERIFY(cjson[keys[5]].isDouble());
        QCOMPARE(cjson[keys[5]].to<double>(), 2.2);

        json1 = json.takeAt(keys[1]);
        QVERIFY(json1.isInteger());
        QCOMPARE(json1.to<int>(), 2);
        QCOMPARE(cjson.count(), 3);
        QVERIFY(cjson[keys[0]].isInteger());
        QCOMPARE(cjson[keys[0]].to<int>(), 1);
        QVERIFY(cjson[keys[1]].isNull());
        QVERIFY(cjson[keys[2]].isNull());
        QVERIFY(cjson[keys[3]].isString());
        QCOMPARE(cjson[keys[3]].to<const char*>(), "String 2");
        QVERIFY(cjson[keys[4]].isNull());
        QVERIFY(cjson[keys[5]].isDouble());
        QCOMPARE(cjson[keys[5]].to<double>(), 2.2);

        json1 = json.takeAt(keys[4]);
        QVERIFY(json1.isNull());
        QCOMPARE(cjson.count(), 3);
        QVERIFY(cjson[keys[0]].isInteger());
        QCOMPARE(cjson[keys[0]].to<int>(), 1);
        QVERIFY(cjson[keys[1]].isNull());
        QVERIFY(cjson[keys[2]].isNull());
        QVERIFY(cjson[keys[3]].isString());
        QCOMPARE(cjson[keys[3]].to<const char*>(), "String 2");
        QVERIFY(cjson[keys[4]].isNull());
        QVERIFY(cjson[keys[5]].isDouble());
        QCOMPARE(cjson[keys[5]].to<double>(), 2.2);

        json1 = json.takeAt("Nonexistent key");
        QVERIFY(json1.isNull());
        QCOMPARE(cjson.count(), 3);
        QVERIFY(cjson[keys[0]].isInteger());
        QCOMPARE(cjson[keys[0]].to<int>(), 1);
        QVERIFY(cjson[keys[1]].isNull());
        QVERIFY(cjson[keys[2]].isNull());
        QVERIFY(cjson[keys[3]].isString());
        QCOMPARE(cjson[keys[3]].to<const char*>(), "String 2");
        QVERIFY(cjson[keys[4]].isNull());
        QVERIFY(cjson[keys[5]].isDouble());
        QCOMPARE(cjson[keys[5]].to<double>(), 2.2);

        json1 = json.takeAt(keys[0]);
        QVERIFY(json1.isInteger());
        QCOMPARE(json1.to<int>(), 1);
        QCOMPARE(cjson.count(), 2);
        json1 = json.takeAt(keys[5]);
        QVERIFY(json1.isDouble());
        QCOMPARE(json1.to<double>(), 2.2);
        QCOMPARE(cjson.count(), 1);
        json1 = json.takeAt(keys[3]);
        QVERIFY(json1.isString());
        QCOMPARE(json1.to<const char*>(), "String 2");
        QCOMPARE(cjson.count(), 0);
    }
};

QTEST_APPLESS_MAIN(GJsonTest)

#include "mgjson_qtest.moc"
