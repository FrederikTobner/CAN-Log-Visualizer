#include "core/LogFormat.h"

#include "TestFrames.h"

#include <QSaveFile>
#include <QTemporaryDir>
#include <QtTest>

#include <vector>

Q_DECLARE_METATYPE(CanFrame)

class LogFormatTest : public QObject {
    Q_OBJECT

  private slots:
    void ignoresBlankAndCommentLines_data();
    void ignoresBlankAndCommentLines();
    void parsesFrames_data();
    void parsesFrames();
    void rejectsMalformedLines_data();
    void rejectsMalformedLines();
    void formatsFrames_data();
    void formatsFrames();
    void roundTrip_data();
    void roundTrip();
    void savesFile();
    void failedSaveLeavesNoFile();
};

namespace {

CanFrame parseOrFail(std::string_view line) {
    auto const frame = parseLogLine(line);
    if (!frame) {
        QTest::qFail(qPrintable(frame.error()), __FILE__, __LINE__);
        return {};
    }
    return *frame;
}

} // namespace

void LogFormatTest::ignoresBlankAndCommentLines_data() {
    QTest::addColumn<QByteArray>("line");
    QTest::addColumn<bool>("ignored");

    QTest::newRow("empty") << QByteArray("") << true;
    QTest::newRow("newline") << QByteArray("\n") << true;
    QTest::newRow("spaces") << QByteArray("   \r\n") << true;
    QTest::newRow("comment") << QByteArray("# recorded on vcan0") << true;
    QTest::newRow("frame") << QByteArray("(1.0) vcan0 123#00") << false;
}

void LogFormatTest::ignoresBlankAndCommentLines() {
    QFETCH(QByteArray, line);
    QFETCH(bool, ignored);

    QCOMPARE(isBlankOrComment({line.constData(), static_cast<size_t>(line.size())}), ignored);
}

void LogFormatTest::parsesFrames_data() {
    QTest::addColumn<QByteArray>("line");
    QTest::addColumn<uint32_t>("id");
    QTest::addColumn<bool>("extended");
    QTest::addColumn<bool>("remote");
    QTest::addColumn<int>("dlc");
    QTest::addColumn<QString>("data");

    QTest::newRow("standard") << QByteArray("(1792411168.454309) vcan0 123#DEADBEEF") << 0x123u << false << false << 4
                              << "DE AD BE EF";
    QTest::newRow("extended") << QByteArray("(1792411168.457194) vcan0 12345678#11") << 0x12345678u << true << false
                              << 1 << "11";
    QTest::newRow("remote") << QByteArray("(1792411168.459155) vcan0 123#R") << 0x123u << false << true << 0 << "";
    QTest::newRow("remote with dlc") << QByteArray("(1792411168.461658) vcan0 123#R3") << 0x123u << false << true << 3
                                     << "";
    QTest::newRow("no data") << QByteArray("(1792411168.463150) vcan0 7FF#") << 0x7FFu << false << false << 0 << "";
    QTest::newRow("full data") << QByteArray("(1.5) can0 001#0102030405060708") << 0x1u << false << false << 8
                               << "01 02 03 04 05 06 07 08";
    QTest::newRow("lowercase hex") << QByteArray("(1.5) can0 1ab#ff") << 0x1ABu << false << false << 1 << "FF";
    QTest::newRow("line ending") << QByteArray("(1.5) can0 123#AA\r\n") << 0x123u << false << false << 1 << "AA";
    QTest::newRow("extra spaces") << QByteArray("  (1.5)   can0   123#AA ") << 0x123u << false << false << 1 << "AA";
}

void LogFormatTest::parsesFrames() {
    QFETCH(QByteArray, line);
    QFETCH(uint32_t, id);
    QFETCH(bool, extended);
    QFETCH(bool, remote);
    QFETCH(int, dlc);
    QFETCH(QString, data);

    CanFrame const frame = parseOrFail({line.constData(), static_cast<size_t>(line.size())});

    QCOMPARE(frame.id, id);
    QCOMPARE(frame.extended, extended);
    QCOMPARE(frame.remote, remote);
    QCOMPARE(static_cast<int>(frame.dlc), dlc);
    QCOMPARE(formatData(frame), data);
}

void LogFormatTest::rejectsMalformedLines_data() {
    QTest::addColumn<QString>("line");
    QTest::addColumn<QString>("errorPart");

    QTest::newRow("empty") << "" << "expected";
    QTest::newRow("missing parts") << "(1.0) vcan0" << "expected";
    QTest::newRow("extra text") << "(1.0) vcan0 123#00 junk" << "expected";
    QTest::newRow("no hash") << "(1.0) vcan0 12300" << "no '#'";
    QTest::newRow("no parentheses") << "1.0 vcan0 123#00" << "timestamp";
    QTest::newRow("bad timestamp") << "(abc) vcan0 123#00" << "abc";
    QTest::newRow("timestamp with junk") << "(1.0x) vcan0 123#00" << "1.0x";
    QTest::newRow("non-finite timestamp") << "(nan) vcan0 123#00" << "nan";
    QTest::newRow("bad id length") << "(1.0) vcan0 1234#00" << "1234";
    QTest::newRow("bad id hex") << "(1.0) vcan0 12G#00" << "12G";
    QTest::newRow("error frame id") << "(1.0) vcan0 20000001#0000000000000000" << "20000001";
    QTest::newRow("odd digit count") << "(1.0) vcan0 123#ABC" << "ABC";
    QTest::newRow("too many bytes") << "(1.0) vcan0 123#112233445566778899" << "112233445566778899";
    QTest::newRow("bad data hex") << "(1.0) vcan0 123#GG" << "GG";
    QTest::newRow("raw dlc suffix") << "(1.0) vcan0 123#1122334455667788_9" << "1122334455667788_9";
    QTest::newRow("can fd") << "(1.0) vcan0 123##1AA" << "CAN FD";
    QTest::newRow("remote dlc too large") << "(1.0) vcan0 123#R9" << "length '9'";
    QTest::newRow("remote junk") << "(1.0) vcan0 123#RX" << "length 'X'";
    QTest::newRow("remote two digits") << "(1.0) vcan0 123#R12" << "length '12'";
}

void LogFormatTest::rejectsMalformedLines() {
    QFETCH(QString, line);
    QFETCH(QString, errorPart);

    std::string const text = line.toStdString();
    auto const frame = parseLogLine(text);
    QVERIFY(!frame);
    QVERIFY2(frame.error().contains(errorPart), qPrintable(frame.error()));
}

void LogFormatTest::formatsFrames_data() {
    QTest::addColumn<CanFrame>("frame");
    QTest::addColumn<QString>("line");

    CanFrame standard = makeFrame(0x123, false, 1792411168.454309);
    standard.dlc = 4;
    standard.data = {0xDE, 0xAD, 0xBE, 0xEF};
    QTest::newRow("standard") << standard << "(1792411168.454309) vcan0 123#DEADBEEF";

    CanFrame extended = makeFrame(0x12345678, true, 2.5);
    extended.dlc = 1;
    extended.data = {0x11};
    QTest::newRow("extended") << extended << "(2.500000) vcan0 12345678#11";

    CanFrame remote = makeFrame(0x123, false, 3.0);
    remote.remote = true;
    QTest::newRow("remote") << remote << "(3.000000) vcan0 123#R";

    remote.dlc = 3;
    QTest::newRow("remote with dlc") << remote << "(3.000000) vcan0 123#R3";

    QTest::newRow("no data") << makeFrame(0x7, false, 4.0) << "(4.000000) vcan0 007#";
}

void LogFormatTest::formatsFrames() {
    QFETCH(CanFrame, frame);
    QFETCH(QString, line);

    QCOMPARE(formatLogLine(frame, "vcan0"), line);
}

void LogFormatTest::roundTrip_data() {
    formatsFrames_data();
}

void LogFormatTest::roundTrip() {
    QFETCH(CanFrame, frame);

    CanFrame const parsed = parseOrFail(formatLogLine(frame, "vcan0").toStdString());

    QCOMPARE(parsed.id, frame.id);
    QCOMPARE(parsed.extended, frame.extended);
    QCOMPARE(parsed.remote, frame.remote);
    QCOMPARE(parsed.dlc, frame.dlc);
    QCOMPARE(parsed.data, frame.data);
    QCOMPARE(parsed.timestamp, frame.timestamp);
}

void LogFormatTest::savesFile() {
    QTemporaryDir const dir;
    QVERIFY(dir.isValid());
    QString const path = dir.filePath("trace.log");

    CanFrame first = makeFrame(0x123, false, 1.0);
    first.dlc = 1;
    first.data = {0xAA};
    std::vector<CanFrame> const frames{first, makeFrame(0x1, true, 2.0)};

    QVERIFY(saveLogFile(path, frames, "can0"));

    QFile file(path);
    QVERIFY(file.open(QIODevice::ReadOnly));
    QCOMPARE(file.readAll(), QByteArray("(1.000000) can0 123#AA\n(2.000000) can0 00000001#\n"));
}

void LogFormatTest::failedSaveLeavesNoFile() {
    QTemporaryDir const dir;
    QVERIFY(dir.isValid());
    QString const path = dir.filePath("missing/trace.log");

    auto const result = saveLogFile(path, std::vector{makeFrame(0x1)}, "can0");

    QVERIFY(!result);
    QVERIFY(!result.error().isEmpty());
    QVERIFY(!QFileInfo::exists(path));
}

QTEST_GUILESS_MAIN(LogFormatTest)
#include "LogFormatTest.moc"
