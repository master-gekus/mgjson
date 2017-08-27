#include <QString>
#include <QtTest>

#include <QSharedData>

class TestestTest : public QObject
{
    Q_OBJECT

public:
    TestestTest();

private slots:
    void testCase1();
};

TestestTest::TestestTest()
{
}

void TestestTest::testCase1()
{
    QVERIFY2(true, "Failure");
}

QTEST_APPLESS_MAIN(TestestTest)

#include "mgjson_qtest.moc"
