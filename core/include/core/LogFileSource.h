#pragma once

#include "core/CanSource.h"

#include <QFile>
#include <QString>
#include <QTimer>

/// @brief Outcome of reading a log file.
struct LogLoadSummary {
    qsizetype frames = 0;
    qsizetype skippedLines = 0;
    /// Description of the first skipped line (`line 12: ...`), empty if none.
    QString firstProblem;
};

/// @brief Replays a `candump` log file as fast as it can be read, in chunks so the
/// event loop stays responsive.
/// @details Timestamps are taken from the file; there is no timing replay.
class LogFileSource final : public CanSource {
    Q_OBJECT

  public:
    /// @brief Constructs a `LogFileSource` for the given file path.
    /// @param path The path to the log file.
    /// @param parent The parent QObject, if any.
    explicit LogFileSource(QString const & path, QObject * parent = nullptr);

    /// @brief Returns the path to the log file.
    /// @return The file path.
    QString path() const;

    /// @brief Starts reading the log file.
    /// @return `true` if the source started successfully, `false` otherwise.
    bool start() override;
    /// @brief Stops reading the log file.
    void stop() override;
    /// @brief Checks if the source is currently running.
    /// @return `true` if the source is running, `false` otherwise.
    bool isRunning() const override;

  signals:
    /// @brief Emitted once the whole file has been read, before `runningChanged(false)`.
    /// @details Not emitted when the load is aborted with `stop()`.
    void finished(LogLoadSummary summary);

  private:
    void readChunk();
    void recordSkippedLine(QString const & reason);
    void close();

    QFile m_file;
    QTimer m_chunkTimer;
    LogLoadSummary m_summary;
    qsizetype m_lineNumber = 0;
};
