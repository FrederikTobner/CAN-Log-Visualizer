#include "MainWindow.hpp"

#include "core/CanStatisticsModel.hpp"

#include <QPointer>
#include <QPushButton>
#include <QTemporaryFile>
#include <QtTest>

#include <memory>

namespace {

QPushButton * findButton(MainWindow const & window, QString const & text) {
    for (auto * button : window.findChildren<QPushButton *>()) {
        if (button->text() == text) {
            return button;
        }
    }
    return nullptr;
}

} // namespace

class MainWindowTest : public QObject {
    Q_OBJECT

  private slots:
    void destroyingConnectedWindowIsSafe();
    void loadedFileFillsStatistics();
    void clearResetsStatistics();
};

void MainWindowTest::destroyingConnectedWindowIsSafe() {
    auto window = std::make_unique<MainWindow>("vcan0");
    QPushButton * connectButton = findButton(*window, "Connect");
    QVERIFY(connectButton != nullptr);

    connectButton->click();
    if (connectButton->text() != "Disconnect") {
        QFAIL("vcan0 is not available (see docs/manual-testing.md)");
    }

    QPointer<MainWindow> const guard(window.get());
    window.reset();

    QVERIFY(guard.isNull());
}

void MainWindowTest::loadedFileFillsStatistics() {
    QTemporaryFile file;
    QVERIFY(file.open());
    file.write("(1.000000) vcan0 123#AA\n(1.010000) vcan0 123#BB\n(1.020000) vcan0 456#CC\n");
    file.close();

    MainWindow window("vcan0");
    window.openLogFile(file.fileName());
    auto * statistics = window.findChild<CanStatisticsModel *>();
    QVERIFY(statistics != nullptr);

    QTRY_COMPARE(statistics->statistics().totalFrames(), uint64_t{3});
    QCOMPARE(statistics->rowCount(), 2);
}

void MainWindowTest::clearResetsStatistics() {
    QTemporaryFile file;
    QVERIFY(file.open());
    file.write("(1.000000) vcan0 123#AA\n");
    file.close();

    MainWindow window("vcan0");
    window.openLogFile(file.fileName());
    auto * statistics = window.findChild<CanStatisticsModel *>();
    QVERIFY(statistics != nullptr);
    QTRY_COMPARE(statistics->statistics().totalFrames(), uint64_t{1});

    QPushButton * clearButton = findButton(window, "Clear");
    QVERIFY(clearButton != nullptr);
    clearButton->click();

    QCOMPARE(statistics->statistics().totalFrames(), uint64_t{0});
    QCOMPARE(statistics->rowCount(), 0);
}

QTEST_MAIN(MainWindowTest)
#include "MainWindowTest.moc"
