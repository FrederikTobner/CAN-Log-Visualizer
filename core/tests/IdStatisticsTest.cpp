#include "TestFrames.hpp"

#include "core/IdStatistics.hpp"

#include <QtTest>

class IdStatisticsTest : public QObject {
    Q_OBJECT

  private slots:
    void firstFrameHasNoCycleTime();
    void cycleTimeTracksLastMinMaxMean();
    void rateUsesWindow();
    void rateDecaysWhenAdvanced();
    void keepsLastFrame();
    void outOfOrderFrameDoesNotChangeCycle();
    void keysDistinguishFrameType();
};

void IdStatisticsTest::firstFrameHasNoCycleTime() {
    IdStatistics statistics(1.0);
    statistics.add(makeFrame(0x123, false, 5.0));

    QCOMPARE(statistics.count(), uint64_t{1});
    QVERIFY(!statistics.cycleTime());
}

void IdStatisticsTest::cycleTimeTracksLastMinMaxMean() {
    IdStatistics statistics(1.0);
    for (double const time : {0.0, 0.5, 0.75, 1.75}) {
        statistics.add(makeFrame(0x123, false, time));
    }

    auto const cycle = statistics.cycleTime();
    if (!cycle) {
        QFAIL("no cycle time");
    }
    CycleTime const value = *cycle;
    QCOMPARE(value.last, 1.0);
    QCOMPARE(value.min, 0.25);
    QCOMPARE(value.max, 1.0);
    QCOMPARE(value.mean, 1.75 / 3.0);
}

void IdStatisticsTest::rateUsesWindow() {
    IdStatistics statistics(2.0);
    for (int i = 0; i < 10; ++i) {
        statistics.add(makeFrame(0x123, false, 0.1 * i));
    }
    QCOMPARE(statistics.rate(), 5.0);
}

void IdStatisticsTest::rateDecaysWhenAdvanced() {
    IdStatistics statistics(1.0);
    statistics.add(makeFrame(0x123, false, 1.0));
    statistics.advanceTo(3.0);

    QCOMPARE(statistics.rate(), 0.0);
    QCOMPARE(statistics.count(), uint64_t{1});
}

void IdStatisticsTest::keepsLastFrame() {
    CanFrame second = makeFrame(0x123, false, 2.0);
    second.dlc = 1;
    second.data = {0x42};

    IdStatistics statistics(1.0);
    statistics.add(makeFrame(0x123, false, 1.0));
    statistics.add(second);

    QCOMPARE(statistics.lastFrame().data[0], uint8_t{0x42});
}

void IdStatisticsTest::outOfOrderFrameDoesNotChangeCycle() {
    IdStatistics statistics(1.0);
    statistics.add(makeFrame(0x123, false, 2.0));
    statistics.add(makeFrame(0x123, false, 1.0));

    QCOMPARE(statistics.count(), uint64_t{2});
    QVERIFY(!statistics.cycleTime());
}

void IdStatisticsTest::keysDistinguishFrameType() {
    QVERIFY(keyOf(makeFrame(0x123, false)) != keyOf(makeFrame(0x123, true)));
    QVERIFY(keyOf(makeFrame(0x123, true)) < keyOf(makeFrame(0x124, false)));
}

QTEST_GUILESS_MAIN(IdStatisticsTest)
#include "IdStatisticsTest.moc"
