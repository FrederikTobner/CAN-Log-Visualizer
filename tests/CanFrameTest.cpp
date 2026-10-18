#include "core/CanFrame.h"
#include "core/KernelFrame.h"

#include <QtTest>

#include <algorithm>
#include <cstring>

class CanFrameTest : public QObject {
    Q_OBJECT

  private slots:
    void frameFromKernel_data();
    void frameFromKernel();
    void formatId_data();
    void formatId();
    void formatData_data();
    void formatData();
};

void CanFrameTest::frameFromKernel_data() {
    QTest::addColumn<quint32>("canId");
    QTest::addColumn<int>("len");
    QTest::addColumn<QByteArray>("bytes");
    QTest::addColumn<quint32>("id");
    QTest::addColumn<bool>("extended");
    QTest::addColumn<bool>("remote");
    QTest::addColumn<int>("dlc");

    QTest::newRow("standard") << 0x123u << 4 << QByteArray::fromHex("deadbeef") << 0x123u << false << false << 4;
    QTest::newRow("extended") << (0x12345678u | CAN_EFF_FLAG) << 1 << QByteArray::fromHex("11") << 0x12345678u << true
                              << false << 1;
    QTest::newRow("remote") << (0x123u | CAN_RTR_FLAG) << 0 << QByteArray() << 0x123u << false << true << 0;
    QTest::newRow("dlc zero") << 0x7FFu << 0 << QByteArray() << 0x7FFu << false << false << 0;
    QTest::newRow("length clamped to 8") << 0x100u << 15 << QByteArray::fromHex("0102030405060708") << 0x100u << false
                                         << false << 8;
}

void CanFrameTest::frameFromKernel() {
    QFETCH(quint32, canId);
    QFETCH(int, len);
    QFETCH(QByteArray, bytes);
    QFETCH(quint32, id);
    QFETCH(bool, extended);
    QFETCH(bool, remote);
    QFETCH(int, dlc);

    can_frame raw{};
    raw.can_id = canId;
    raw.len = static_cast<__u8>(len);
    std::memcpy(raw.data, bytes.constData(), std::min<size_t>(static_cast<size_t>(bytes.size()), sizeof(raw.data)));

    CanFrame const frame = ::frameFromKernel(raw, 1.5);

    QCOMPARE(frame.id, id);
    QCOMPARE(frame.extended, extended);
    QCOMPARE(frame.remote, remote);
    QCOMPARE(static_cast<int>(frame.dlc), dlc);
    QCOMPARE(frame.timestamp, 1.5);
    QCOMPARE(QByteArray(reinterpret_cast<char const *>(frame.data.data()), dlc), bytes.left(dlc));
}

void CanFrameTest::formatId_data() {
    QTest::addColumn<quint32>("id");
    QTest::addColumn<bool>("extended");
    QTest::addColumn<QString>("expected");

    QTest::newRow("standard") << 0x123u << false << "123";
    QTest::newRow("standard leading zeros") << 0x5u << false << "005";
    QTest::newRow("standard uppercase") << 0xABCu << false << "ABC";
    QTest::newRow("standard max") << 0x7FFu << false << "7FF";
    QTest::newRow("extended") << 0x12345678u << true << "12345678";
    QTest::newRow("extended leading zeros") << 0x1u << true << "00000001";
}

void CanFrameTest::formatId() {
    QFETCH(quint32, id);
    QFETCH(bool, extended);
    QFETCH(QString, expected);

    CanFrame frame;
    frame.id = id;
    frame.extended = extended;
    QCOMPARE(::formatId(frame), expected);
}

void CanFrameTest::formatData_data() {
    QTest::addColumn<QByteArray>("bytes");
    QTest::addColumn<int>("dlc");
    QTest::addColumn<bool>("remote");
    QTest::addColumn<QString>("expected");

    QTest::newRow("four bytes") << QByteArray::fromHex("deadbeef") << 4 << false << "DE AD BE EF";
    QTest::newRow("single byte padded") << QByteArray::fromHex("0a") << 1 << false << "0A";
    QTest::newRow("empty") << QByteArray() << 0 << false << "";
    QTest::newRow("only dlc bytes shown") << QByteArray::fromHex("0102030405060708") << 2 << false << "01 02";
    QTest::newRow("remote shows nothing") << QByteArray() << 4 << true << "";
}

void CanFrameTest::formatData() {
    QFETCH(QByteArray, bytes);
    QFETCH(int, dlc);
    QFETCH(bool, remote);
    QFETCH(QString, expected);

    CanFrame frame;
    frame.dlc = static_cast<uint8_t>(dlc);
    frame.remote = remote;
    std::memcpy(frame.data.data(), bytes.constData(), static_cast<size_t>(bytes.size()));
    QCOMPARE(::formatData(frame), expected);
}

QTEST_GUILESS_MAIN(CanFrameTest)
#include "CanFrameTest.moc"
