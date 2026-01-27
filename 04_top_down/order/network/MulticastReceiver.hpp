#pragma once
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>
#include <stdexcept>
#include <functional>
#include <atomic>
#include "../common/Types.hpp"
#include "../common/Constants.hpp"
#include "../common/LatencyMeasure.hpp"

namespace orderbook {

class MulticastReceiver {
public:
    using PacketCallback = std::function<void(const char*, size_t, Timestamp)>;

    MulticastReceiver(const char* multicastGroup = MULTICAST_GROUP,
                      int port = MULTICAST_PORT)
        : multicastGroup_(multicastGroup)
        , port_(port)
        , sockfd_(-1)
        , running_(false) {}

    ~MulticastReceiver() {
        stop();
        if (sockfd_ != -1) {
            close(sockfd_);
        }
    }

    // Initialize socket and join multicast group
    void init() {
        // Create UDP socket
        sockfd_ = socket(AF_INET, SOCK_DGRAM, 0);
        if (sockfd_ == -1) {
            throw std::runtime_error("Failed to create socket");
        }

        // Allow multiple sockets to use the same port
        int reuse = 1;
        if (setsockopt(sockfd_, SOL_SOCKET, SO_REUSEADDR,
                       &reuse, sizeof(reuse)) < 0) {
            throw std::runtime_error("setsockopt SO_REUSEADDR failed");
        }

        // macOS requires SO_REUSEPORT for multiple binds
#ifdef SO_REUSEPORT
        if (setsockopt(sockfd_, SOL_SOCKET, SO_REUSEPORT,
                       &reuse, sizeof(reuse)) < 0) {
            throw std::runtime_error("setsockopt SO_REUSEPORT failed");
        }
#endif

        // Bind to the port
        sockaddr_in localAddr{};
        localAddr.sin_family = AF_INET;
        localAddr.sin_port = htons(port_);
        localAddr.sin_addr.s_addr = INADDR_ANY;

        if (bind(sockfd_, reinterpret_cast<sockaddr*>(&localAddr),
                 sizeof(localAddr)) < 0) {
            throw std::runtime_error("Bind failed");
        }

        // Join multicast group
        ip_mreq mreq{};
        mreq.imr_multiaddr.s_addr = inet_addr(multicastGroup_);
        mreq.imr_interface.s_addr = INADDR_ANY;

        if (setsockopt(sockfd_, IPPROTO_IP, IP_ADD_MEMBERSHIP,
                       &mreq, sizeof(mreq)) < 0) {
            throw std::runtime_error("Failed to join multicast group");
        }
    }

    // Blocking receive - captures timestamp immediately after syscall
    ssize_t receive(char* buffer, size_t maxLen, Timestamp& recvTimestamp) {
        ssize_t bytes = recvfrom(sockfd_, buffer, maxLen, 0, nullptr, nullptr);

        // Capture timestamp IMMEDIATELY after syscall returns
        // This is critical for accurate latency measurement
        recvTimestamp = getCurrentNanos();

        return bytes;
    }

    // Run receive loop with callback
    void run(PacketCallback callback) {
        char buffer[MAX_PACKET_SIZE];
        running_.store(true, std::memory_order_release);

        while (running_.load(std::memory_order_acquire)) {
            Timestamp recvTs;
            ssize_t bytes = receive(buffer, sizeof(buffer), recvTs);

            if (bytes > 0) {
                callback(buffer, static_cast<size_t>(bytes), recvTs);
            }
        }
    }

    void stop() {
        running_.store(false, std::memory_order_release);
    }

    bool isRunning() const {
        return running_.load(std::memory_order_acquire);
    }

    int getSocket() const { return sockfd_; }

private:
    const char* multicastGroup_;
    int port_;
    int sockfd_;
    std::atomic<bool> running_;
};

}  // namespace orderbook
