#include <QtTest>

#include "GJson.h"

#define SIMPLE_TEST_COUNT 10

Q_DECLARE_METATYPE(GJsonParseError::ParseError)

namespace QTest
{
  template <>
  char* toString<GJsonParseError::ParseError>(const GJsonParseError::ParseError& err)
  {
    return qstrdup(GJsonParseError::error_string(err));
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
    void testCase1();
};

void GJsonTest::testCase1()
{
    QVERIFY2(true, "Failure");
}

QTEST_APPLESS_MAIN(GJsonTest)

#include "mgjson_qtest.moc"
