#pragma once
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>
#include <stdexcept>
#include <random>
#include "../common/Types.hpp"
#include "../common/Constants.hpp"
#include "../common/Protocol.hpp"
#include "../common/LatencyMeasure.hpp"

namespace orderbook {

class MulticastSender {
public:
    MulticastSender(const char* multicastGroup = MULTICAST_GROUP,
                    int port = MULTICAST_PORT)
        : multicastGroup_(multicastGroup)
        , port_(port)
        , sockfd_(-1)
        , sequence_(0)
        , nextOrderId_(1)
        , rng_(std::random_device{}()) {}

    ~MulticastSender() {
        if (sockfd_ != -1) {
            close(sockfd_);
        }
    }

    void init() {
        // Create UDP socket
        sockfd_ = socket(AF_INET, SOCK_DGRAM, 0);
        if (sockfd_ == -1) {
            throw std::runtime_error("Failed to create socket");
        }

        // Set TTL for multicast (default 1 = local network only)
        unsigned char ttl = 1;
        if (setsockopt(sockfd_, IPPROTO_IP, IP_MULTICAST_TTL,
                       &ttl, sizeof(ttl)) < 0) {
            throw std::runtime_error("setsockopt IP_MULTICAST_TTL failed");
        }

        // Setup destination address
        destAddr_.sin_family = AF_INET;
        destAddr_.sin_port = htons(port_);
        destAddr_.sin_addr.s_addr = inet_addr(multicastGroup_);
    }

    // Send a single message
    ssize_t send(const MarketDataMessage& msg) {
        return sendto(sockfd_, &msg, sizeof(msg), 0,
                      reinterpret_cast<sockaddr*>(&destAddr_),
                      sizeof(destAddr_));
    }

    // Generate and send a random Add order
    void sendRandomAdd() {
        std::uniform_int_distribution<int> priceDist(9900, 10100);
        std::uniform_int_distribution<int> qtyDist(1, 1000);
        std::uniform_int_distribution<int> sideDist(0, 1);

        Price price = priceDist(rng_) * 100;  // Fixed point
        Quantity qty = qtyDist(rng_);
        Side side = sideDist(rng_) == 0 ? Side::Buy : Side::Sell;

        auto msg = makeAddMessage(
            sequence_++,
            nextOrderId_++,
            price,
            qty,
            side,
            getCurrentNanos()
        );

        send(msg);
        activeOrders_.push_back(msg.orderId);
    }

    // Generate and send a random Modify for existing order
    void sendRandomModify() {
        if (activeOrders_.empty()) return;

        std::uniform_int_distribution<size_t> idxDist(0, activeOrders_.size() - 1);
        std::uniform_int_distribution<int> qtyDist(1, 500);

        OrderId id = activeOrders_[idxDist(rng_)];

        auto msg = makeModifyMessage(
            sequence_++,
            id,
            qtyDist(rng_),
            getCurrentNanos()
        );

        send(msg);
    }

    // Generate and send a random Delete for existing order
    void sendRandomDelete() {
        if (activeOrders_.empty()) return;

        std::uniform_int_distribution<size_t> idxDist(0, activeOrders_.size() - 1);
        size_t idx = idxDist(rng_);
        OrderId id = activeOrders_[idx];

        auto msg = makeDeleteMessage(
            sequence_++,
            id,
            getCurrentNanos()
        );

        send(msg);

        // Remove from active orders
        activeOrders_.erase(activeOrders_.begin() + idx);
    }

    // Send a burst of random messages
    // Distribution: 70% Add, 20% Modify, 10% Delete
    void sendBurst(size_t count) {
        std::uniform_int_distribution<int> typeDist(0, 99);

        for (size_t i = 0; i < count; ++i) {
            int r = typeDist(rng_);
            if (r < 70) {
                sendRandomAdd();
            } else if (r < 90) {
                sendRandomModify();
            } else {
                sendRandomDelete();
            }
        }
    }

    SequenceNumber getSequence() const { return sequence_; }
    size_t getActiveOrderCount() const { return activeOrders_.size(); }

private:
    const char* multicastGroup_;
    int port_;
    int sockfd_;
    sockaddr_in destAddr_{};
    SequenceNumber sequence_;
    OrderId nextOrderId_;
    std::mt19937 rng_;
    std::vector<OrderId> activeOrders_;
};

}  // namespace orderbook
