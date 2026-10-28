#include "core/CanFilter.hpp"

#include "TestFrames.hpp"

#include <QtTest>

class CanFilterTest : public QObject {
    Q_OBJECT

  private slots:
    void blankTextGivesEmptyFilter();
    void parsesTerms_data();
    void parsesTerms();
    void rejectsInvalidText_data();
    void rejectsInvalidText();
    void emptyFilterMatchesEverything();
    void exactIdMatchesOnlyThatId();
    void maskComparesOnlyMaskedBits();
    void standardAndExtendedAreDistinct();
    void excludeRemovesMatches();
    void excludeWinsOverInclude();
};

void CanFilterTest::blankTextGivesEmptyFilter() {
    for (QString const & text : {QString(), QString("  "), QString(" , ,")}) {
        auto const filter = parseCanFilter(text);
        QVERIFY(filter);
        QVERIFY(filter->include.empty());
        QVERIFY(filter->exclude.empty());
    }
}

void CanFilterTest::parsesTerms_data() {
    QTest::addColumn<QString>("text");
    QTest::addColumn<uint32_t>("id");
    QTest::addColumn<uint32_t>("mask");
    QTest::addColumn<bool>("extended");
    QTest::addColumn<bool>("exclude");

    QTest::newRow("standard") << "123" << 0x123u << 0x7FFu << false << false;
    QTest::newRow("short standard") << "12" << 0x12u << 0x7FFu << false << false;
    QTest::newRow("lowercase") << "1ab" << 0x1ABu << 0x7FFu << false << false;
    QTest::newRow("mask") << "100:700" << 0x100u << 0x700u << false << false;
    QTest::newRow("extended") << "12345678" << 0x12345678u << 0x1FFFFFFFu << true << false;
    QTest::newRow("extended mask") << "12345678:FF" << 0x12345678u << 0xFFu << true << false;
    QTest::newRow("exclude") << "!123" << 0x123u << 0x7FFu << false << true;
    QTest::newRow("whitespace") << "  ! 123 : 7F0 " << 0x123u << 0x7F0u << false << true;
}

void CanFilterTest::parsesTerms() {
    QFETCH(QString, text);
    QFETCH(uint32_t, id);
    QFETCH(uint32_t, mask);
    QFETCH(bool, extended);
    QFETCH(bool, exclude);

    auto const filter = parseCanFilter(text);
    QVERIFY(filter);

    auto const & rules = exclude ? filter->exclude : filter->include;
    auto const & otherRules = exclude ? filter->include : filter->exclude;
    QCOMPARE(rules.size(), size_t{1});
    QVERIFY(otherRules.empty());
    QCOMPARE(rules.front().id, id);
    QCOMPARE(rules.front().mask, mask);
    QCOMPARE(rules.front().extended, extended);
}

void CanFilterTest::rejectsInvalidText_data() {
    QTest::addColumn<QString>("text");
    QTest::addColumn<QString>("errorPart");

    QTest::newRow("not hex") << "12G" << "12G";
    QTest::newRow("too long for standard") << "1234" << "1234";
    QTest::newRow("too long for extended") << "123456789" << "123456789";
    QTest::newRow("standard out of range") << "800" << "800";
    QTest::newRow("extended out of range") << "20000000" << "20000000";
    QTest::newRow("empty exclude") << "!" << "'!'";
    QTest::newRow("bad mask") << "123:XY" << "XY";
    QTest::newRow("mask out of range") << "123:800" << "800";
    QTest::newRow("prefix is rejected") << "0x12" << "0x12";
    QTest::newRow("second term is reported") << "123,12G" << "12G";
}

void CanFilterTest::rejectsInvalidText() {
    QFETCH(QString, text);
    QFETCH(QString, errorPart);

    auto const filter = parseCanFilter(text);
    QVERIFY(!filter);
    QVERIFY2(filter.error().contains(errorPart), qPrintable(filter.error()));
}

void CanFilterTest::emptyFilterMatchesEverything() {
    CanFilter const filter;
    QVERIFY(filter.matches(makeFrame(0x123)));
    QVERIFY(filter.matches(makeFrame(0x123, true)));
}

void CanFilterTest::exactIdMatchesOnlyThatId() {
    auto const filter = parseCanFilter("123");
    QVERIFY(filter);
    QVERIFY(filter->matches(makeFrame(0x123)));
    QVERIFY(!filter->matches(makeFrame(0x124)));
}

void CanFilterTest::maskComparesOnlyMaskedBits() {
    auto const filter = parseCanFilter("700:700");
    QVERIFY(filter);
    QVERIFY(filter->matches(makeFrame(0x700)));
    QVERIFY(filter->matches(makeFrame(0x7FF)));
    QVERIFY(!filter->matches(makeFrame(0x6FF)));
}

void CanFilterTest::standardAndExtendedAreDistinct() {
    auto const standard = parseCanFilter("123");
    auto const extended = parseCanFilter("00000123");
    QVERIFY(standard);
    QVERIFY(extended);

    QVERIFY(standard->matches(makeFrame(0x123)));
    QVERIFY(!standard->matches(makeFrame(0x123, true)));
    QVERIFY(extended->matches(makeFrame(0x123, true)));
    QVERIFY(!extended->matches(makeFrame(0x123)));
}

void CanFilterTest::excludeRemovesMatches() {
    auto const filter = parseCanFilter("!123");
    QVERIFY(filter);
    QVERIFY(!filter->matches(makeFrame(0x123)));
    QVERIFY(filter->matches(makeFrame(0x124)));
}

void CanFilterTest::excludeWinsOverInclude() {
    auto const filter = parseCanFilter("100:700, 200:700, !150");
    QVERIFY(filter);
    QVERIFY(filter->matches(makeFrame(0x123)));
    QVERIFY(filter->matches(makeFrame(0x250)));
    QVERIFY(!filter->matches(makeFrame(0x150)));
    QVERIFY(!filter->matches(makeFrame(0x300)));
}

QTEST_GUILESS_MAIN(CanFilterTest)
#include "CanFilterTest.moc"
