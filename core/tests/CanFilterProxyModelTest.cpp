#include "core/CanFilterProxyModel.h"

#include "TestFrames.h"

#include <QAbstractItemModelTester>
#include <QtTest>

namespace {

CanFilter parseOrEmpty(QString const & text) {
    auto const filter = parseCanFilter(text);
    if (!filter) {
        QTest::qFail(qPrintable(filter.error()), __FILE__, __LINE__);
        return {};
    }
    return *filter;
}

QStringList column(QAbstractItemModel const & model, int column) {
    QStringList values;
    for (int row = 0; row < model.rowCount(); ++row) {
        values.append(model.index(row, column).data().toString());
    }
    return values;
}

} // namespace

class CanFilterProxyModelTest : public QObject {
    Q_OBJECT

  private slots:
    void modelContract();
    void showsEverythingWithoutFilter();
    void appendedBatchesRespectFilter();
    void changingFilterUpdatesRows();
    void clearingFilterRevealsFramesReceivedWhileFiltering();
    void keepsSourceFrameNumbers();
    void rowCapTrimsHiddenAndShownRows();
    void visibleFramesFollowFilter();
};

void CanFilterProxyModelTest::modelContract() {
    CanTraceModel trace(5);
    CanFilterProxyModel proxy(&trace);
    QAbstractItemModelTester tester(&proxy, QAbstractItemModelTester::FailureReportingMode::QtTest);

    proxy.setFilter(parseOrEmpty("1:F"));
    trace.appendFrames(makeFrames({0x1, 0x2, 0x11, 0x1}));
    trace.appendFrames(makeFrames({0x1, 0x1, 0x1, 0x1, 0x1, 0x3}));
    proxy.setFilter(parseOrEmpty("!1"));
    trace.clear();
}

void CanFilterProxyModelTest::showsEverythingWithoutFilter() {
    CanTraceModel trace;
    CanFilterProxyModel proxy(&trace);

    trace.appendFrames(makeFrames({0x1, 0x2, 0x3}));

    QCOMPARE(proxy.rowCount(), 3);
}

void CanFilterProxyModelTest::appendedBatchesRespectFilter() {
    CanTraceModel trace;
    CanFilterProxyModel proxy(&trace);
    proxy.setFilter(parseOrEmpty("100:700"));

    trace.appendFrames(makeFrames({0x100, 0x200, 0x1FF}));
    trace.appendFrames(makeFrames({0x300, 0x101}));

    QCOMPARE(column(proxy, CanTraceModel::Id), QStringList({"100", "1FF", "101"}));
}

void CanFilterProxyModelTest::changingFilterUpdatesRows() {
    CanTraceModel trace;
    CanFilterProxyModel proxy(&trace);
    trace.appendFrames(makeFrames({0x1, 0x2, 0x3}));

    proxy.setFilter(parseOrEmpty("2"));
    QCOMPARE(column(proxy, CanTraceModel::Id), QStringList({"002"}));

    proxy.setFilter(parseOrEmpty("!2"));
    QCOMPARE(column(proxy, CanTraceModel::Id), QStringList({"001", "003"}));

    proxy.setFilter({});
    QCOMPARE(proxy.rowCount(), 3);
}

void CanFilterProxyModelTest::clearingFilterRevealsFramesReceivedWhileFiltering() {
    CanTraceModel trace;
    CanFilterProxyModel proxy(&trace);
    proxy.setFilter(parseOrEmpty("2"));

    trace.appendFrames(makeFrames({0x1, 0x2, 0x3}));
    trace.appendFrames(makeFrames({0x4, 0x2}));
    QCOMPARE(proxy.rowCount(), 2);

    proxy.setFilter({});

    QCOMPARE(proxy.rowCount(), 5);
}

void CanFilterProxyModelTest::keepsSourceFrameNumbers() {
    CanTraceModel trace;
    CanFilterProxyModel proxy(&trace);
    proxy.setFilter(parseOrEmpty("2"));

    trace.appendFrames(makeFrames({0x1, 0x2, 0x3, 0x2}));

    QCOMPARE(column(proxy, CanTraceModel::Number), QStringList({"2", "4"}));
}

void CanFilterProxyModelTest::rowCapTrimsHiddenAndShownRows() {
    CanTraceModel trace(3);
    CanFilterProxyModel proxy(&trace);
    proxy.setFilter(parseOrEmpty("2"));

    trace.appendFrames(makeFrames({0x2, 0x1, 0x2}));
    trace.appendFrames(makeFrames({0x1, 0x1}));

    QCOMPARE(column(proxy, CanTraceModel::Number), QStringList({"3"}));

    trace.appendFrames(makeFrames({0x2}));
    QCOMPARE(column(proxy, CanTraceModel::Number), QStringList({"6"}));

    trace.setMaxRows(1);
    QCOMPARE(column(proxy, CanTraceModel::Number), QStringList({"6"}));
}

void CanFilterProxyModelTest::visibleFramesFollowFilter() {
    CanTraceModel trace;
    CanFilterProxyModel proxy(&trace);
    trace.appendFrames(makeFrames({0x1, 0x2, 0x3, 0x2}));
    proxy.setFilter(parseOrEmpty("2"));

    auto const frames = proxy.visibleFrames();

    QCOMPARE(frames.size(), size_t{2});
    QCOMPARE(frames.at(0).id, 0x2u);
    QCOMPARE(frames.at(1).id, 0x2u);
}

QTEST_GUILESS_MAIN(CanFilterProxyModelTest)
#include "CanFilterProxyModelTest.moc"
