#include "TestFrames.hpp"

#include "core/BitTiming.hpp"
#include "core/CanStatistics.hpp"

#include <QtTest>

class CanStatisticsTest : public QObject {
    Q_OBJECT

  private slots:
    void countsTotalsBeyondWindow();
    void standardAndExtendedAreSeparateIds();
    void idsAreSortedByKey();
    void frameRateUsesWindow();
    void busLoadIsBitsOverCapacity();
    void bitrateChangesLoadOnly();
    void advanceToDecaysRates();
    void resetKeepsBitrate();
    void tracksLatestTimestamp();
};

void CanStatisticsTest::countsTotalsBeyondWindow() {
    CanStatistics statistics;
    for (int i = 0; i < 100; ++i) {
        statistics.add(makeFrame(0x1, false, 0.1 * i));
    }

    QCOMPARE(statistics.totalFrames(), uint64_t{100});
    QCOMPARE(statistics.ids().size(), size_t{1});
    QCOMPARE(statistics.ids()[0].statistics.count(), uint64_t{100});
}

void CanStatisticsTest::standardAndExtendedAreSeparateIds() {
    CanStatistics statistics;
    statistics.add(makeFrame(0x123, false));
    statistics.add(makeFrame(0x123, true));

    QCOMPARE(statistics.ids().size(), size_t{2});
    QVERIFY(statistics.contains({.id = 0x123, .extended = true}));
}

void CanStatisticsTest::idsAreSortedByKey() {
    CanStatistics statistics;
    for (uint32_t const id : {0x300u, 0x100u, 0x200u, 0x100u}) {
        statistics.add(makeFrame(id));
    }

    auto const ids = statistics.ids();
    QCOMPARE(ids.size(), size_t{3});
    QCOMPARE(ids[0].key.id, 0x100u);
    QCOMPARE(ids[1].key.id, 0x200u);
    QCOMPARE(ids[2].key.id, 0x300u);
}

void CanStatisticsTest::frameRateUsesWindow() {
    CanStatistics statistics(1.0);
    for (int i = 0; i < 40; ++i) {
        statistics.add(makeFrame(0x1, false, 0.05 * i));
    }
    // 20 frames per second, so 20 frames sit in the last second.
    QCOMPARE(statistics.frameRate(), 20.0);
}

void CanStatisticsTest::busLoadIsBitsOverCapacity() {
    CanStatistics statistics(1.0, 500'000);
    CanFrame const frame = makeFrame(0x1, false, 0.0);
    double const frameBits = maxFrameBits(frame);
    statistics.add(frame);

    QCOMPARE(statistics.busLoad(), frameBits / 500'000.0);
}

void CanStatisticsTest::bitrateChangesLoadOnly() {
    CanStatistics statistics(1.0, 500'000);
    statistics.add(makeFrame(0x1));
    double const load = statistics.busLoad();
    double const rate = statistics.frameRate();

    statistics.setBitrate(250'000);

    QCOMPARE(statistics.busLoad(), load * 2.0);
    QCOMPARE(statistics.frameRate(), rate);
}

void CanStatisticsTest::advanceToDecaysRates() {
    CanStatistics statistics;
    statistics.add(makeFrame(0x1, false, 1.0));
    statistics.advanceTo(5.0);

    QCOMPARE(statistics.frameRate(), 0.0);
    QCOMPARE(statistics.busLoad(), 0.0);
    QCOMPARE(statistics.ids()[0].statistics.rate(), 0.0);
    QCOMPARE(statistics.totalFrames(), uint64_t{1});
}

void CanStatisticsTest::resetKeepsBitrate() {
    CanStatistics statistics(1.0, 250'000);
    statistics.add(makeFrame(0x1));
    statistics.reset();

    QCOMPARE(statistics.totalFrames(), uint64_t{0});
    QVERIFY(statistics.ids().empty());
    QVERIFY(!statistics.latestTimestamp());
    QCOMPARE(statistics.bitrate(), 250'000u);
}

void CanStatisticsTest::tracksLatestTimestamp() {
    CanStatistics statistics;
    statistics.add(makeFrame(0x1, false, 3.0));
    statistics.add(makeFrame(0x1, false, 2.0));

    QCOMPARE(statistics.latestTimestamp().value_or(0.0), 3.0);
}

QTEST_GUILESS_MAIN(CanStatisticsTest)
#include "CanStatisticsTest.moc"
