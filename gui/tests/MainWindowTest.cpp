#include "MainWindow.h"

#include <QPointer>
#include <QPushButton>
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
};

void MainWindowTest::destroyingConnectedWindowIsSafe() {
    auto window = std::make_unique<MainWindow>("vcan0");
    QPushButton * connectButton = findButton(*window, "Connect");
    QVERIFY(connectButton != nullptr);

    connectButton->click();
    if (connectButton->text() != "Disconnect") {
        QSKIP("vcan0 is not available (see docs/usage.md)");
    }

    QPointer<MainWindow> const guard(window.get());
    window.reset();

    QVERIFY(guard.isNull());
}

QTEST_MAIN(MainWindowTest)
#include "MainWindowTest.moc"
