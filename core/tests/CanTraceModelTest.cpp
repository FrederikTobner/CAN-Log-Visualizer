#include "core/CanTraceModel.hpp"

#include <QAbstractItemModelTester>
#include <QtTest>

namespace {

CanFrame makeFrame(uint32_t id, double timestamp = 0.0) {
    CanFrame frame;
    frame.id = id;
    frame.timestamp = timestamp;
    return frame;
}

QList<CanFrame> makeFrames(uint32_t firstId, int count) {
    QList<CanFrame> frames;
    for (int i = 0; i < count; ++i) {
        frames.append(makeFrame(firstId + static_cast<uint32_t>(i)));
    }
    return frames;
}

QString cell(CanTraceModel const & model, int row, CanTraceModel::Column column) {
    return model.index(row, column).data().toString();
}

} // namespace

class CanTraceModelTest : public QObject {
    Q_OBJECT

  private slots:
    void modelContract();
    void appendInsertsOneBatch();
    void capDropsOldestAndKeepsNumbering();
    void batchLargerThanCapKeepsNewest();
    void timeIsRelativeToFirstFrame();
    void displayStrings();
    void frameAtReturnsRowFrame();
    void loweringMaxRowsDropsOldest();
    void raisingMaxRowsKeepsRows();
};

void CanTraceModelTest::modelContract() {
    CanTraceModel model(5);
    QAbstractItemModelTester tester(&model, QAbstractItemModelTester::FailureReportingMode::QtTest);

    model.appendFrames(makeFrames(1, 3));
    model.appendFrames(makeFrames(10, 4));
    model.appendFrames(makeFrames(20, 10));
    QCOMPARE(model.rowCount(), 5);

    model.clear();
    QCOMPARE(model.rowCount(), 0);
}

void CanTraceModelTest::appendInsertsOneBatch() {
    CanTraceModel model;
    QSignalSpy inserted(&model, &QAbstractItemModel::rowsInserted);

    model.appendFrames(makeFrames(1, 3));

    QCOMPARE(model.rowCount(), 3);
    QCOMPARE(inserted.count(), 1);
    QCOMPARE(inserted.first().at(1).toInt(), 0);
    QCOMPARE(inserted.first().at(2).toInt(), 2);
}

void CanTraceModelTest::capDropsOldestAndKeepsNumbering() {
    CanTraceModel model(3);
    QSignalSpy removed(&model, &QAbstractItemModel::rowsRemoved);

    model.appendFrames(makeFrames(1, 3));
    model.appendFrames(makeFrames(4, 2));

    QCOMPARE(model.rowCount(), 3);
    QCOMPARE(removed.count(), 1);
    QCOMPARE(cell(model, 0, CanTraceModel::Number), QString("3"));
    QCOMPARE(cell(model, 0, CanTraceModel::Id), QString("003"));
    QCOMPARE(cell(model, 2, CanTraceModel::Number), QString("5"));
    QCOMPARE(cell(model, 2, CanTraceModel::Id), QString("005"));
}

void CanTraceModelTest::batchLargerThanCapKeepsNewest() {
    CanTraceModel model(3);
    model.appendFrames(makeFrames(1, 2));
    model.appendFrames(makeFrames(10, 5));

    QCOMPARE(model.rowCount(), 3);
    // Frames 1..7 were received in total; the newest three are kept.
    QCOMPARE(cell(model, 0, CanTraceModel::Number), QString("5"));
    QCOMPARE(cell(model, 0, CanTraceModel::Id), QString("00C"));
    QCOMPARE(cell(model, 2, CanTraceModel::Number), QString("7"));
    QCOMPARE(cell(model, 2, CanTraceModel::Id), QString("00E"));
}

void CanTraceModelTest::timeIsRelativeToFirstFrame() {
    CanTraceModel model;
    model.appendFrames({makeFrame(1, 100.0), makeFrame(2, 100.25)});

    QCOMPARE(cell(model, 0, CanTraceModel::Time), QString("0.000000"));
    QCOMPARE(cell(model, 1, CanTraceModel::Time), QString("0.250000"));

    model.clear();
    model.appendFrames({makeFrame(3, 200.0)});
    QCOMPARE(cell(model, 0, CanTraceModel::Time), QString("0.000000"));
    QCOMPARE(cell(model, 0, CanTraceModel::Number), QString("1"));
}

void CanTraceModelTest::displayStrings() {
    CanFrame standard = makeFrame(0x123);
    standard.dlc = 2;
    standard.data = {0xDE, 0xAD};

    CanFrame extendedRemote = makeFrame(0x1ABCDE);
    extendedRemote.extended = true;
    extendedRemote.remote = true;
    extendedRemote.dlc = 3;

    CanTraceModel model;
    model.appendFrames({standard, extendedRemote});

    QCOMPARE(cell(model, 0, CanTraceModel::Id), QString("123"));
    QCOMPARE(cell(model, 0, CanTraceModel::Type), QString("STD"));
    QCOMPARE(cell(model, 0, CanTraceModel::Dlc), QString("2"));
    QCOMPARE(cell(model, 0, CanTraceModel::Data), QString("DE AD"));

    QCOMPARE(cell(model, 1, CanTraceModel::Id), QString("001ABCDE"));
    QCOMPARE(cell(model, 1, CanTraceModel::Type), QString("EXT RTR"));
    QCOMPARE(cell(model, 1, CanTraceModel::Dlc), QString("3"));
    QCOMPARE(cell(model, 1, CanTraceModel::Data), QString(""));
}

void CanTraceModelTest::frameAtReturnsRowFrame() {
    CanTraceModel model;
    model.appendFrames(makeFrames(7, 3));

    QCOMPARE(model.frameAt(0).id, 7u);
    QCOMPARE(model.frameAt(2).id, 9u);
}

void CanTraceModelTest::loweringMaxRowsDropsOldest() {
    CanTraceModel model;
    model.appendFrames(makeFrames(1, 5));
    QSignalSpy removed(&model, &QAbstractItemModel::rowsRemoved);

    model.setMaxRows(2);

    QCOMPARE(removed.count(), 1);
    QCOMPARE(model.rowCount(), 2);
    QCOMPARE(cell(model, 0, CanTraceModel::Number), QString("4"));
    QCOMPARE(cell(model, 1, CanTraceModel::Number), QString("5"));
}

void CanTraceModelTest::raisingMaxRowsKeepsRows() {
    CanTraceModel model(2);
    model.appendFrames(makeFrames(1, 2));
    QSignalSpy removed(&model, &QAbstractItemModel::rowsRemoved);

    model.setMaxRows(10);
    model.appendFrames(makeFrames(3, 4));

    QCOMPARE(removed.count(), 0);
    QCOMPARE(model.rowCount(), 6);
}

QTEST_GUILESS_MAIN(CanTraceModelTest)
#include "CanTraceModelTest.moc"
