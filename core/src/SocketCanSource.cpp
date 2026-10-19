#include "core/SocketCanSource.h"

#include "core/KernelFrame.h"

#include <QSocketNotifier>

#include <array>
#include <cerrno>
#include <chrono>
#include <cstring>
#include <ctime>
#include <system_error>

#include <net/if.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include <linux/can.h>
#include <linux/can/raw.h>

namespace {

/// Upper bound per notifier wake-up so a saturated bus cannot starve the UI.
constexpr int kMaxFramesPerDrain = 1024;

QString errnoText() {
    return QString::fromStdString(std::generic_category().message(errno));
}

double nowSeconds() {
    auto const sinceEpoch = std::chrono::system_clock::now().time_since_epoch();
    return std::chrono::duration<double>(sinceEpoch).count();
}

} // namespace

SocketCanSource::SocketCanSource(QString interfaceName, QObject * parent)
    : CanSource(parent), m_interfaceName(std::move(interfaceName)) {
}

SocketCanSource::~SocketCanSource() {
    stop();
}

QString SocketCanSource::interfaceName() const {
    return m_interfaceName;
}

void SocketCanSource::setInterfaceName(QString name) {
    m_interfaceName = std::move(name);
}

bool SocketCanSource::isRunning() const {
    return m_fd >= 0;
}

bool SocketCanSource::start() {
    if (isRunning()) {
        return true;
    }

    int const fd = ::socket(PF_CAN, SOCK_RAW | SOCK_NONBLOCK | SOCK_CLOEXEC, CAN_RAW);
    if (fd < 0) {
        emit errorOccurred(QString("Cannot create CAN socket: %1").arg(errnoText()));
        return false;
    }

    unsigned int const ifindex = ::if_nametoindex(m_interfaceName.toLocal8Bit().constData());
    if (ifindex == 0) {
        ::close(fd);
        emit errorOccurred(QString("CAN interface '%1' not found").arg(m_interfaceName));
        return false;
    }

    // Kernel receive timestamps, delivered as SCM_TIMESTAMPNS control messages.
    int const enable = 1;
    if (::setsockopt(fd, SOL_SOCKET, SO_TIMESTAMPNS, &enable, sizeof(enable)) < 0) {
        QString const reason = errnoText();
        ::close(fd);
        emit errorOccurred(QString("Cannot enable timestamps: %1").arg(reason));
        return false;
    }

    sockaddr_can address{};
    address.can_family = AF_CAN;
    address.can_ifindex = static_cast<int>(ifindex);
    if (::bind(fd, reinterpret_cast<sockaddr const *>(&address), sizeof(address)) < 0) {
        QString const reason = errnoText();
        ::close(fd);
        emit errorOccurred(QString("Cannot bind to '%1': %2").arg(m_interfaceName, reason));
        return false;
    }

    m_fd = fd;
    m_notifier = new QSocketNotifier(m_fd, QSocketNotifier::Read, this);
    connect(m_notifier, &QSocketNotifier::activated, this, [this] { drainSocket(); });

    emit runningChanged(true);
    return true;
}

void SocketCanSource::stop() {
    if (!isRunning()) {
        return;
    }

    m_notifier->setEnabled(false);
    m_notifier->deleteLater();
    m_notifier = nullptr;
    ::close(m_fd);
    m_fd = -1;

    emit runningChanged(false);
}

void SocketCanSource::fail(QString const & message) {
    stop();
    emit errorOccurred(message);
}

void SocketCanSource::drainSocket() {
    QList<CanFrame> frames;
    QString failure;

    while (frames.size() < kMaxFramesPerDrain) {
        can_frame raw{};
        alignas(cmsghdr) std::array<char, CMSG_SPACE(sizeof(timespec))> control{};

        iovec buffer{.iov_base = &raw, .iov_len = sizeof(raw)};
        msghdr message{};
        message.msg_iov = &buffer;
        message.msg_iovlen = 1;
        message.msg_control = control.data();
        message.msg_controllen = control.size();

        ssize_t const received = ::recvmsg(m_fd, &message, 0);
        if (received < 0) {
            if (errno == EINTR) {
                continue;
            }
            if (errno != EAGAIN && errno != EWOULDBLOCK) {
                failure = QString("Receive failed: %1").arg(errnoText());
            }
            break;
        }

        // Only classic CAN frames are enabled on this socket.
        if (static_cast<size_t>(received) != CAN_MTU) {
            continue;
        }

        double timestamp = nowSeconds();
        for (cmsghdr * header = CMSG_FIRSTHDR(&message); header != nullptr; header = CMSG_NXTHDR(&message, header)) {
            if (header->cmsg_level == SOL_SOCKET && header->cmsg_type == SCM_TIMESTAMPNS) {
                timespec stamp{};
                std::memcpy(&stamp, CMSG_DATA(header), sizeof(stamp));
                timestamp = static_cast<double>(stamp.tv_sec) + (static_cast<double>(stamp.tv_nsec) * 1e-9);
            }
        }

        frames.append(frameFromKernel(raw, timestamp));
    }

    if (!frames.isEmpty()) {
        emit framesReceived(frames);
    }
    if (!failure.isEmpty()) {
        fail(failure);
    }
}
