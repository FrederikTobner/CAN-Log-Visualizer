#include "core/Result.h"

#include <QtTest>

class ResultTest : public QObject {
    Q_OBJECT

  private slots:
    void holdsValue();
    void holdsError();
    void sameTypeForValueAndErrorIsUnambiguous();
};

void ResultTest::holdsValue() {
    Result<int> const result = 42;

    QVERIFY(result);
    QCOMPARE(*result, 42);
}

void ResultTest::holdsError() {
    Result<int> const result = Failure{"broken"};

    QVERIFY(!result);
    QCOMPARE(result.error(), QString("broken"));
}

void ResultTest::sameTypeForValueAndErrorIsUnambiguous() {
    Result<QString> const success = QString("value");
    Result<QString> const failure = Failure{"error"};

    QVERIFY(success);
    QCOMPARE(*success, QString("value"));
    QCOMPARE(success->size(), 5);
    QVERIFY(!failure);
    QCOMPARE(failure.error(), QString("error"));
}

QTEST_GUILESS_MAIN(ResultTest)
#include "ResultTest.moc"
