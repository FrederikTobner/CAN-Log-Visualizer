#include "core/SlidingWindow.hpp"

#include <QtTest>

class SlidingWindowTest : public QObject {
    Q_OBJECT

  private slots:
    void sumsSamplesInsideWindow();
    void oldSamplesFallOut();
    void advanceDropsSamplesWithoutNewOnes();
    void lateSampleIsIgnored();
    void clearForgetsPosition();
};

void SlidingWindowTest::sumsSamplesInsideWindow() {
    SlidingWindow window(1.0);
    window.add(10.0, 3);
    window.add(10.5, 4);

    QCOMPARE(window.count(), size_t{2});
    QCOMPARE(window.weight(), uint64_t{7});
}

void SlidingWindowTest::oldSamplesFallOut() {
    SlidingWindow window(1.0);
    window.add(10.0);
    window.add(10.5);
    window.add(11.0);

    // (10.0, 11.0]: the sample at exactly 10.0 is gone.
    QCOMPARE(window.count(), size_t{2});
}

void SlidingWindowTest::advanceDropsSamplesWithoutNewOnes() {
    SlidingWindow window(1.0);
    window.add(10.0);
    window.advanceTo(12.0);
    QCOMPARE(window.count(), size_t{0});

    window.advanceTo(5.0);
    window.add(10.5);
    QCOMPARE(window.count(), size_t{0});
}

void SlidingWindowTest::lateSampleIsIgnored() {
    SlidingWindow window(1.0);
    window.add(10.0);
    window.add(5.0);
    QCOMPARE(window.count(), size_t{1});
}

void SlidingWindowTest::clearForgetsPosition() {
    SlidingWindow window(1.0);
    window.add(100.0);
    window.clear();
    window.add(1.0);
    QCOMPARE(window.count(), size_t{1});
    QCOMPARE(window.weight(), uint64_t{1});
}

QTEST_GUILESS_MAIN(SlidingWindowTest)
#include "SlidingWindowTest.moc"
