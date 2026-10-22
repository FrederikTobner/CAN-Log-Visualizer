#include "core/LogFileSource.h"

#include "core/LogFormat.h"

namespace {

constexpr int kLinesPerChunk = 2000;

} // namespace

LogFileSource::LogFileSource(QString const & path, QObject * parent) : CanSource(parent), m_file(path) {
    connect(&m_chunkTimer, &QTimer::timeout, this, &LogFileSource::readChunk);
}

QString LogFileSource::path() const {
    return m_file.fileName();
}

bool LogFileSource::isRunning() const {
    return m_file.isOpen();
}

bool LogFileSource::start() {
    if (isRunning()) {
        return true;
    }

    if (!m_file.open(QIODevice::ReadOnly)) {
        emit errorOccurred(QString("Cannot open '%1': %2").arg(m_file.fileName(), m_file.errorString()));
        return false;
    }

    m_summary = {};
    m_lineNumber = 0;
    m_chunkTimer.start(0);
    emit runningChanged(true);
    return true;
}

void LogFileSource::stop() {
    if (!isRunning()) {
        return;
    }

    close();
    emit runningChanged(false);
}

void LogFileSource::close() {
    m_chunkTimer.stop();
    m_file.close();
}

void LogFileSource::recordSkippedLine(QString const & reason) {
    ++m_summary.skippedLines;
    if (m_summary.firstProblem.isEmpty()) {
        m_summary.firstProblem = QString("line %1: %2").arg(m_lineNumber).arg(reason);
    }
}

void LogFileSource::readChunk() {
    QList<CanFrame> frames;

    for (int i = 0; i < kLinesPerChunk && !m_file.atEnd(); ++i) {
        QByteArray const line = m_file.readLine();
        ++m_lineNumber;

        std::string_view const text{line.constData(), static_cast<size_t>(line.size())};
        if (isBlankOrComment(text)) {
            continue;
        }

        if (auto const frame = parseLogLine(text)) {
            frames.append(*frame);
        } else {
            recordSkippedLine(frame.error());
        }
    }

    m_summary.frames += frames.size();
    if (!frames.isEmpty()) {
        emit framesReceived(frames);
    }

    // A receiver may have called stop() while handling the batch.
    if (isRunning() && m_file.atEnd()) {
        close();
        emit finished(m_summary);
        emit runningChanged(false);
    }
}
