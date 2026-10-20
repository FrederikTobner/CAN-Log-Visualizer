#pragma once

#include "core/CanFrame.h"

#include <QAbstractTableModel>
#include <QList>

#include <deque>
#include <optional>

/// Table model showing received frames, oldest first. Keeps at most
/// `maxRows` frames; older ones are dropped but keep their frame numbers.
class CanTraceModel : public QAbstractTableModel {
    Q_OBJECT

  public:
    enum Column {
        Number,
        Time,
        Id,
        Type,
        Dlc,
        Data,
        ColumnCount
    };

    explicit CanTraceModel(qsizetype maxRows = 100000, QObject * parent = nullptr);

    int rowCount(QModelIndex const & parent = {}) const override;
    int columnCount(QModelIndex const & parent = {}) const override;
    QVariant data(QModelIndex const & index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    /// The frame shown in `row`.
    CanFrame const & frameAt(int row) const;

    void appendFrames(QList<CanFrame> const & frames);
    /// Changes the row cap; lowering it drops the oldest rows right away.
    void setMaxRows(qsizetype maxRows);
    void clear();

  private:
    void removeOldest(qsizetype count);

    qsizetype m_maxRows;
    std::deque<CanFrame> m_frames;
    /// Frame number of `m_frames.front()`.
    qulonglong m_firstNumber = 1;
    /// Timestamp of the first frame since the last clear; times are shown relative to it.
    std::optional<double> m_timeOrigin;
};
