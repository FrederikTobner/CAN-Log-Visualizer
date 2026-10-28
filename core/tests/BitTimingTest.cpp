#include "core/BitTiming.hpp"

#include <QtTest>

class BitTimingTest : public QObject {
    Q_OBJECT

  private slots:
    void frameLengths_data();
    void frameLengths();
    void remoteFramesHaveNoDataBits();
};

void BitTimingTest::frameLengths_data() {
    QTest::addColumn<bool>("extended");
    QTest::addColumn<int>("dlc");
    QTest::addColumn<int>("bits");

    QTest::newRow("standard dlc 0") << false << 0 << 55;
    QTest::newRow("standard dlc 8") << false << 8 << 135;
    QTest::newRow("extended dlc 0") << true << 0 << 80;
    QTest::newRow("extended dlc 8") << true << 8 << 160;
}

void BitTimingTest::frameLengths() {
    QFETCH(bool, extended);
    QFETCH(int, dlc);
    QFETCH(int, bits);

    CanFrame frame;
    frame.extended = extended;
    frame.dlc = static_cast<uint8_t>(dlc);
    QCOMPARE(maxFrameBits(frame), bits);
}

void BitTimingTest::remoteFramesHaveNoDataBits() {
    CanFrame remote;
    remote.remote = true;
    remote.dlc = 8;
    CanFrame empty;
    QCOMPARE(maxFrameBits(remote), maxFrameBits(empty));
}

QTEST_GUILESS_MAIN(BitTimingTest)
#include "BitTimingTest.moc"
