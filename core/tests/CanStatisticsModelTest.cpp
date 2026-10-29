#include "TestFrames.hpp"

#include "core/CanStatisticsModel.hpp"

#include <QAbstractItemModelTester>
#include <QtTest>

namespace {

QString cell(CanStatisticsModel const & model, int row, CanStatisticsModel::Column column) {
    return model.index(row, column).data().toString();
}

} // namespace

class CanStatisticsModelTest : public QObject {
    Q_OBJECT

  private slots:
    void modelContract();
    void rowsAppearPerId();
    void batchEmitsOneDataChanged();
    void displayStrings();
    void sortRoleIsNumeric();
    void resetRemovesRows();
};

void CanStatisticsModelTest::modelContract() {
    CanStatisticsModel model;
    QAbstractItemModelTester tester(&model, QAbstractItemModelTester::FailureReportingMode::QtTest);

    model.addFrames({makeFrame(0x300, false, 0.0), makeFrame(0x100, false, 0.1), makeFrame(0x100, true, 0.2)});
    model.advanceTo(5.0);
    model.reset();
}

void CanStatisticsModelTest::rowsAppearPerId() {
    CanStatisticsModel model;
    QSignalSpy inserted(&model, &QAbstractItemModel::rowsInserted);

    model.addFrames(makeFrames({0x200, 0x100, 0x200}));

    QCOMPARE(model.rowCount(), 2);
    QCOMPARE(inserted.count(), 2);
    QCOMPARE(cell(model, 0, CanStatisticsModel::Id), QString("100"));
    QCOMPARE(cell(model, 1, CanStatisticsModel::Count), QString("2"));
}

void CanStatisticsModelTest::batchEmitsOneDataChanged() {
    CanStatisticsModel model;
    model.addFrames(makeFrames({0x1, 0x2}));
    QSignalSpy changed(&model, &QAbstractItemModel::dataChanged);

    model.addFrames(makeFrames({0x1, 0x2, 0x1, 0x2}));

    QCOMPARE(changed.count(), 1);
}

void CanStatisticsModelTest::displayStrings() {
    CanFrame first = makeFrame(0x123, false, 1.0);
    first.dlc = 2;
    first.data = {0xDE, 0xAD};
    CanFrame second = first;
    second.timestamp = 1.010;

    CanStatisticsModel model;
    model.addFrames({first, second});

    QCOMPARE(cell(model, 0, CanStatisticsModel::Type), QString("STD"));
    QCOMPARE(cell(model, 0, CanStatisticsModel::CycleMean), QString("10.000"));
    QCOMPARE(cell(model, 0, CanStatisticsModel::Data), QString("DE AD"));
}

void CanStatisticsModelTest::sortRoleIsNumeric() {
    CanStatisticsModel model;
    model.addFrames({makeFrame(0x1, false, 0.0)});

    auto const cycle = model.index(0, CanStatisticsModel::CycleMean).data(CanStatisticsModel::kSortRole);
    QVERIFY(!cycle.isValid());
    QCOMPARE(cell(model, 0, CanStatisticsModel::CycleMean), QString("-"));

    auto const count = model.index(0, CanStatisticsModel::Count).data(CanStatisticsModel::kSortRole);
    QCOMPARE(count.toULongLong(), qulonglong{1});
}

void CanStatisticsModelTest::resetRemovesRows() {
    CanStatisticsModel model;
    model.addFrames(makeFrames({0x1, 0x2}));
    model.reset();

    QCOMPARE(model.rowCount(), 0);
    QCOMPARE(model.statistics().totalFrames(), uint64_t{0});
}

QTEST_GUILESS_MAIN(CanStatisticsModelTest)
#include "CanStatisticsModelTest.moc"
