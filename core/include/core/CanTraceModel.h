#pragma once

#include "core/CanFrame.h"

#include <QAbstractTableModel>
#include <QList>

#include <deque>
#include <optional>

/// @brief Table model showing received frames. 
/// @details The oldest frame is displayed first. Keeps at most `maxRows` frames. 
/// Older ones are dropped but keep their frame numbers.
class CanTraceModel : public QAbstractTableModel {
    Q_OBJECT

  public:
    /// @brief Columns of the table model.
    enum Column {
        Number,
        Time,
        Id,
        Type,
        Dlc,
        Data,
        ColumnCount
    };

    /// @brief Constructs the trace model.
    /// @param maxRows Maximum number of rows to keep.
    /// @param parent The parent QObject, if any.
    explicit CanTraceModel(qsizetype maxRows = 100000, QObject * parent = nullptr);

    /// @brief Returns the number of rows in the model.
    /// @param parent The parent index, if any.
    /// @return The number of rows.
    int rowCount(QModelIndex const & parent = {}) const override;
    /// @brief Returns the number of columns in the model.
    /// @param parent The parent index, if any.
    /// @return The number of columns.
    int columnCount(QModelIndex const & parent = {}) const override;

    /// @brief Returns the data for the given index and role.
    /// @param index The model index.
    /// @param role The role for which data is requested.
    /// @return The data for the given index and role.
    QVariant data(QModelIndex const & index, int role = Qt::DisplayRole) const override;

    /// @brief Returns the header data for the given section, orientation, and role.
    /// @param section The section (row or column).
    /// @param orientation The orientation (horizontal or vertical).
    /// @param role The role for which header data is requested.
    /// @return The header data.
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    /// @brief Returns the frame shown in the given row.
    /// @param row The row index.
    /// @return The frame at the specified row.
    CanFrame const & frameAt(int row) const;

    /// @brief Appends new frames to the model.
    /// @param frames The frames to append.
    void appendFrames(QList<CanFrame> const & frames);

    /// @brief Changes the row cap. Lowering it drops the oldest rows right away.
    /// @param maxRows The new maximum number of rows.
    void setMaxRows(qsizetype maxRows);

    /// @brief Clears all frames from the model.
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
