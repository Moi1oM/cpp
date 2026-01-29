#include <iostream>
#include <chrono>
#include <thread>
#include "../common/Constants.hpp"
#include "../common/Protocol.hpp"
#include "../common/LatencyMeasure.hpp"
#include "../network/MulticastSender.hpp"

using namespace orderbook;

void printUsage(const char* prog) {
    std::cout << "Usage: " << prog << " <message_count> [rate_per_sec]" << std::endl;
    std::cout << "  message_count: Number of messages to send" << std::endl;
    std::cout << "  rate_per_sec: Messages per second (default: no rate limit)" << std::endl;
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        printUsage(argv[0]);
        return 1;
    }

    size_t messageCount = std::stoull(argv[1]);
    size_t ratePerSec = (argc >= 3) ? std::stoull(argv[2]) : 0;

    std::cout << "=== Multicast Test Sender ===" << std::endl;
    std::cout << "Multicast Group: " << MULTICAST_GROUP << std::endl;
    std::cout << "Port: " << MULTICAST_PORT << std::endl;
    std::cout << "Messages: " << messageCount << std::endl;
    if (ratePerSec > 0) {
        std::cout << "Rate: " << ratePerSec << " msg/sec" << std::endl;
    } else {
        std::cout << "Rate: Unlimited" << std::endl;
    }
    std::cout << std::endl;

    MulticastSender sender;
    sender.init();

    auto startTime = std::chrono::high_resolution_clock::now();
    auto intervalNs = (ratePerSec > 0) ? (1'000'000'000 / ratePerSec) : 0;

    for (size_t i = 0; i < messageCount; ++i) {
        sender.sendRandomAdd();

        if (intervalNs > 0) {
            // Rate limiting
            auto targetTime = startTime + std::chrono::nanoseconds(intervalNs * (i + 1));
            auto now = std::chrono::high_resolution_clock::now();
            if (now < targetTime) {
                std::this_thread::sleep_until(targetTime);
            }
        }

        // Progress indicator
        if ((i + 1) % 100000 == 0) {
            std::cout << "Sent " << (i + 1) << " messages..." << std::endl;
        }
    }

    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(
        endTime - startTime).count();

    std::cout << std::endl;
    std::cout << "=== Complete ===" << std::endl;
    std::cout << "Total sent: " << messageCount << " messages" << std::endl;
    std::cout << "Duration: " << duration << " ms" << std::endl;
    std::cout << "Actual rate: " << (messageCount * 1000 / duration) << " msg/sec" << std::endl;

    return 0;
}
