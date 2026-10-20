#pragma once

#include "core/CanSource.h"

#include <QFile>
#include <QString>
#include <QTimer>

/// Outcome of reading a log file.
struct LogLoadSummary {
    qsizetype frames = 0;
    qsizetype skippedLines = 0;
    /// Description of the first skipped line (`line 12: ...`), empty if none.
    QString firstProblem;
};

/// Replays a `candump` log file as fast as it can be read, in chunks so the
/// event loop stays responsive. Timestamps are taken from the file; there is
/// no timing replay.
class LogFileSource final : public CanSource {
    Q_OBJECT

  public:
    explicit LogFileSource(QString const & path, QObject * parent = nullptr);

    QString path() const;

    bool start() override;
    void stop() override;
    bool isRunning() const override;

  signals:
    /// Emitted once the whole file has been read, before `runningChanged(false)`.
    /// Not emitted when the load is aborted with `stop()`.
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
