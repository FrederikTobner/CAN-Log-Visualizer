/*****************************************************************************
 * Copyright (C) 2026 by Frederik Tobner                                     *
 *                                                                           *
 * This file is part of the CAN Log Visualizer.                              *
 *                                                                           *
 * Permission to use, copy, modify, and distribute this software and its     *
 * documentation under the terms of the GNU Affero General Public License is *
 * hereby granted.                                                           *
 * No representations are made about the suitability of this software for    *
 * any purpose.                                                              *
 * It is provided "as is" without express or implied warranty.               *
 * See the <https://www.gnu.org/licenses/gpl-3.0.html>                       *
 * GNU General Public License version 3 or later.                            *
 * License for more details.                                                 *
 ****************************************************************************/

#include "StatisticsPanel.hpp"

#include "core/CanStatisticsModel.hpp"

#include <QComboBox>
#include <QFontDatabase>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QIntValidator>
#include <QLabel>
#include <QSortFilterProxyModel>
#include <QTableView>
#include <QTimer>
#include <QVBoxLayout>

#include <chrono>

namespace {

constexpr int kRefreshIntervalMs = 250;
constexpr int kDefaultBitrate = 500'000;
constexpr int kMinBitrate = 1000;
constexpr int kMaxBitrate = 10'000'000;

double nowSeconds() {
    auto const sinceEpoch = std::chrono::system_clock::now().time_since_epoch();
    return std::chrono::duration<double>(sinceEpoch).count();
}

} // namespace

StatisticsPanel::StatisticsPanel(CanStatisticsModel * model, QWidget * parent)
    : QWidget(parent), m_model(model), m_summary(new QLabel(this)), m_bitrate(new QComboBox(this)),
      m_timer(new QTimer(this)) {
    m_bitrate->setEditable(true);
    m_bitrate->setValidator(new QIntValidator(kMinBitrate, kMaxBitrate, m_bitrate));
    for (int const bitrate : {125'000, 250'000, 500'000, 1'000'000}) {
        m_bitrate->addItem(QString::number(bitrate));
    }
    m_bitrate->setCurrentText(QString::number(kDefaultBitrate));
    m_model->setBitrate(kDefaultBitrate);

    auto * proxy = new QSortFilterProxyModel(this);
    proxy->setSourceModel(m_model);
    proxy->setSortRole(CanStatisticsModel::kSortRole);

    auto * view = new QTableView(this);
    view->setModel(proxy);
    view->setFont(QFontDatabase::systemFont(QFontDatabase::FixedFont));
    view->setSortingEnabled(true);
    view->sortByColumn(CanStatisticsModel::Id, Qt::AscendingOrder);
    view->setSelectionBehavior(QAbstractItemView::SelectRows);
    view->setEditTriggers(QAbstractItemView::NoEditTriggers);
    view->verticalHeader()->setVisible(false);
    view->horizontalHeader()->setStretchLastSection(true);

    m_summary->setToolTip("Rate: frames per second over the last second.\n"
                          "Bus load: upper bound assuming worst-case bit stuffing at the selected bitrate.");
    m_bitrate->setToolTip("Nominal bus bitrate used to estimate the bus load");
    view->setToolTip("Cycle: time between consecutive frames of one ID. Click a header to sort.");

    auto * header = new QHBoxLayout;
    header->addWidget(m_summary, 1);
    header->addWidget(new QLabel("Bitrate [bit/s]:", this));
    header->addWidget(m_bitrate);

    auto * layout = new QVBoxLayout(this);
    layout->addLayout(header);
    layout->addWidget(view);

    connect(m_bitrate, &QComboBox::textActivated, this, &StatisticsPanel::applyBitrate);
    connect(m_timer, &QTimer::timeout, this, &StatisticsPanel::refresh);
    m_timer->start(kRefreshIntervalMs);
    refresh();
}

void StatisticsPanel::setLive(bool live) {
    m_live = live;
}

void StatisticsPanel::applyBitrate(QString const & text) {
    bool ok = false;
    int const bitrate = text.toInt(&ok);
    if (ok && bitrate >= kMinBitrate) {
        m_model->setBitrate(static_cast<unsigned>(bitrate));
        refresh();
    }
}

void StatisticsPanel::refresh() {
    if (m_live) {
        m_model->advanceTo(nowSeconds());
    }

    CanStatistics const & statistics = m_model->statistics();
    m_summary->setText(QString("Frames: %1   Rate: %2 Hz   Bus load: \u2264 %3 %")
                           .arg(statistics.totalFrames())
                           .arg(statistics.frameRate(), 0, 'f', 1)
                           .arg(statistics.busLoad() * 100.0, 0, 'f', 1));
}
