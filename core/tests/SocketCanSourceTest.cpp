#include "core/SocketCanSource.h"

#include <QtTest>

#include <cerrno>
#include <cstring>

#include <net/if.h>
#include <sys/socket.h>
#include <unistd.h>

#include <linux/can.h>
#include <linux/can/raw.h>

namespace {

/// Sends one classic frame through a separate raw socket.
bool sendFrame(char const * interface, canid_t id, QByteArray const & data) {
    int const fd = ::socket(PF_CAN, SOCK_RAW | SOCK_CLOEXEC, CAN_RAW);
    if (fd < 0) {
        return false;
    }

    sockaddr_can address{};
    address.can_family = AF_CAN;
    address.can_ifindex = static_cast<int>(::if_nametoindex(interface));

    can_frame frame{};
    frame.can_id = id;
    frame.len = static_cast<__u8>(data.size());
    std::memcpy(frame.data, data.constData(), static_cast<size_t>(data.size()));

    bool const ok = ::bind(fd, reinterpret_cast<sockaddr const *>(&address), sizeof(address)) == 0 &&
                    ::write(fd, &frame, sizeof(frame)) == static_cast<ssize_t>(sizeof(frame));
    ::close(fd);
    return ok;
}

} // namespace

class SocketCanSourceTest : public QObject {
    Q_OBJECT

  private slots:
    void startFailsForMissingInterface();
    void stopWithoutStartIsNoOp();
    void receivesFramesFromVcan();
};

void SocketCanSourceTest::startFailsForMissingInterface() {
    SocketCanSource source("canlog-missing");
    QSignalSpy errors(&source, &CanSource::errorOccurred);
    QSignalSpy running(&source, &CanSource::runningChanged);

    QVERIFY(!source.start());
    QVERIFY(!source.isRunning());
    QCOMPARE(errors.count(), 1);
    QCOMPARE(running.count(), 0);
}

void SocketCanSourceTest::stopWithoutStartIsNoOp() {
    SocketCanSource source;
    QSignalSpy running(&source, &CanSource::runningChanged);

    source.stop();

    QVERIFY(!source.isRunning());
    QCOMPARE(running.count(), 0);
}

void SocketCanSourceTest::receivesFramesFromVcan() {
    SocketCanSource source("vcan0");
    QSignalSpy frames(&source, &CanSource::framesReceived);
    if (!source.start()) {
        QSKIP("vcan0 is not available (see docs/usage.md)");
    }

    QByteArray const payload = QByteArray::fromHex("deadbeef");
    QVERIFY(sendFrame("vcan0", 0x123, payload));
    QVERIFY(frames.wait(2000));

    auto const received = frames.takeFirst().at(0).value<QList<CanFrame>>();
    QCOMPARE(received.size(), 1);
    CanFrame const & frame = received.first();
    QCOMPARE(frame.id, 0x123u);
    QVERIFY(!frame.extended);
    QVERIFY(!frame.remote);
    QCOMPARE(static_cast<int>(frame.dlc), 4);
    QCOMPARE(QByteArray(reinterpret_cast<char const *>(frame.data.data()), 4), payload);

    double const now = static_cast<double>(QDateTime::currentMSecsSinceEpoch()) / 1000.0;
    QVERIFY(std::abs(frame.timestamp - now) < 5.0);

    source.stop();
    QVERIFY(!source.isRunning());
}

QTEST_GUILESS_MAIN(SocketCanSourceTest)
#include "SocketCanSourceTest.moc"
