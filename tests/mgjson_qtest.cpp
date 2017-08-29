#include <ctime>

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

public:
    GJsonTest()
    {
        qsrand(time(0));
    }

private slots:

    // TestTypedConstructor
    void TestTypedConstructor_data()
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
    void TestTypedConstructor()
    {
        QFETCH(GJson::Type, type);
        GJson test_json(type);
        QCOMPARE(test_json.type(), type);
    }
};

QTEST_APPLESS_MAIN(GJsonTest)

#include "mgjson_qtest.moc"
