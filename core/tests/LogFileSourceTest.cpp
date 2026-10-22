#include "core/LogFileSource.h"

#include <QSignalSpy>
#include <QTemporaryFile>
#include <QtTest>

Q_DECLARE_METATYPE(LogLoadSummary)

class LogFileSourceTest : public QObject {
    Q_OBJECT

  private slots:
    void loadsValidFramesAndSkipsOthers();
    void emptyFileFinishesImmediately();
    void largeFileIsDeliveredInSeveralBatches();
    void missingFileFailsToStart();
    void stopAbortsWithoutFinished();
};

namespace {

QByteArray frameLine(int number) {
    return QByteArray("(%1.000000) vcan0 123#AA\n").replace("%1", QByteArray::number(number));
}

/// Keeps the file alive for the test's duration.
struct LogFixture {
    explicit LogFixture(QByteArray const & content) {
        if (!file.open()) {
            qFatal("cannot create temporary log file");
        }
        file.write(content);
        file.close();
    }

    QTemporaryFile file;
    QString path() const {
        return file.fileName();
    }
};

qsizetype totalFrames(QSignalSpy const & batches) {
    qsizetype total = 0;
    for (auto const & arguments : batches) {
        total += arguments.at(0).value<QList<CanFrame>>().size();
    }
    return total;
}

} // namespace

void LogFileSourceTest::loadsValidFramesAndSkipsOthers() {
    LogFixture const log("# comment\n"
                         "(1.0) vcan0 123#AA\n"
                         "\n"
                         "garbage\n"
                         "(2.0) vcan0 123##1AA\n"
                         "(3.0) vcan0 12345678#BBCC\n");
    LogFileSource source(log.path());
    QSignalSpy batches(&source, &CanSource::framesReceived);
    QSignalSpy finished(&source, &LogFileSource::finished);
    QSignalSpy running(&source, &CanSource::runningChanged);

    QVERIFY(source.start());
    QVERIFY(source.isRunning());
    QTRY_COMPARE(finished.count(), 1);

    QCOMPARE(totalFrames(batches), 2);
    auto const summary = finished.first().at(0).value<LogLoadSummary>();
    QCOMPARE(summary.frames, 2);
    QCOMPARE(summary.skippedLines, 2);
    QVERIFY2(summary.firstProblem.startsWith("line 4:"), qPrintable(summary.firstProblem));

    QVERIFY(!source.isRunning());
    QCOMPARE(running.count(), 2);
    QCOMPARE(running.at(0).at(0).toBool(), true);
    QCOMPARE(running.at(1).at(0).toBool(), false);
}

void LogFileSourceTest::emptyFileFinishesImmediately() {
    LogFixture const log("");
    LogFileSource source(log.path());
    QSignalSpy batches(&source, &CanSource::framesReceived);
    QSignalSpy finished(&source, &LogFileSource::finished);

    QVERIFY(source.start());
    QTRY_COMPARE(finished.count(), 1);

    QCOMPARE(batches.count(), 0);
    QCOMPARE(finished.first().at(0).value<LogLoadSummary>().frames, 0);
}

void LogFileSourceTest::largeFileIsDeliveredInSeveralBatches() {
    QByteArray content;
    constexpr int kLines = 5000;
    for (int i = 0; i < kLines; ++i) {
        content += frameLine(i);
    }
    LogFixture const log(content);
    LogFileSource source(log.path());
    QSignalSpy batches(&source, &CanSource::framesReceived);
    QSignalSpy finished(&source, &LogFileSource::finished);

    QVERIFY(source.start());
    QTRY_COMPARE(finished.count(), 1);

    QVERIFY(batches.count() > 1);
    QCOMPARE(totalFrames(batches), kLines);
    QCOMPARE(finished.first().at(0).value<LogLoadSummary>().frames, kLines);
}

void LogFileSourceTest::missingFileFailsToStart() {
    LogFileSource source("/nonexistent/trace.log");
    QSignalSpy errors(&source, &CanSource::errorOccurred);
    QSignalSpy running(&source, &CanSource::runningChanged);

    QVERIFY(!source.start());

    QVERIFY(!source.isRunning());
    QCOMPARE(errors.count(), 1);
    QVERIFY(errors.first().at(0).toString().contains("/nonexistent/trace.log"));
    QCOMPARE(running.count(), 0);
}

void LogFileSourceTest::stopAbortsWithoutFinished() {
    QByteArray content;
    for (int i = 0; i < 5000; ++i) {
        content += frameLine(i);
    }
    LogFixture const log(content);
    LogFileSource source(log.path());
    QSignalSpy batches(&source, &CanSource::framesReceived);
    QSignalSpy finished(&source, &LogFileSource::finished);
    QSignalSpy running(&source, &CanSource::runningChanged);

    QVERIFY(source.start());
    QTRY_VERIFY(batches.count() >= 1);
    source.stop();
    qsizetype const framesAtStop = totalFrames(batches);

    QTest::qWait(50);
    QCOMPARE(totalFrames(batches), framesAtStop);
    QCOMPARE(finished.count(), 0);
    QVERIFY(!source.isRunning());
    QCOMPARE(running.last().at(0).toBool(), false);
}

QTEST_GUILESS_MAIN(LogFileSourceTest)
#include "LogFileSourceTest.moc"
