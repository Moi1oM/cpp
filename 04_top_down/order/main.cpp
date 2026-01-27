#include <iostream>
#include <thread>
#include <csignal>
#include <atomic>
#include "common/Constants.hpp"
#include "common/Protocol.hpp"
#include "common/LatencyMeasure.hpp"
#include "network/MulticastReceiver.hpp"
#include "v3_mempool/OptimizedHandler.hpp"

using namespace orderbook;

std::atomic<bool> running{true};

void signalHandler(int) {
    running.store(false, std::memory_order_release);
}

void printUsage(const char* prog) {
    std::cout << "Usage: " << prog << " [options]" << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << "  -g <group>  Multicast group (default: " << MULTICAST_GROUP << ")" << std::endl;
    std::cout << "  -p <port>   Port (default: " << MULTICAST_PORT << ")" << std::endl;
    std::cout << "  -h          Show this help" << std::endl;
}

int main(int argc, char* argv[]) {
    const char* multicastGroup = MULTICAST_GROUP;
    int port = MULTICAST_PORT;

    // Parse arguments
    for (int i = 1; i < argc; ++i) {
        if (std::string(argv[i]) == "-g" && i + 1 < argc) {
            multicastGroup = argv[++i];
        } else if (std::string(argv[i]) == "-p" && i + 1 < argc) {
            port = std::stoi(argv[++i]);
        } else if (std::string(argv[i]) == "-h") {
            printUsage(argv[0]);
            return 0;
        }
    }

    std::cout << "╔══════════════════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║           UDP MULTICAST ORDER BOOK RECEIVER                  ║" << std::endl;
    std::cout << "╠══════════════════════════════════════════════════════════════╣" << std::endl;
    std::cout << "║ Using V3: Lock-Free Queue + Object Pool                      ║" << std::endl;
    std::cout << "╚══════════════════════════════════════════════════════════════╝" << std::endl;
    std::cout << std::endl;
    std::cout << "Multicast Group: " << multicastGroup << std::endl;
    std::cout << "Port: " << port << std::endl;
    std::cout << "Press Ctrl+C to stop" << std::endl;
    std::cout << std::endl;

    // Setup signal handler
    std::signal(SIGINT, signalHandler);

    // Setup latency recorder
    LatencyRecorder latency;

    // Create and initialize components
    MulticastReceiver receiver(multicastGroup, port);
    v3::OptimizedHandler handler(latency);

    try {
        receiver.init();
        std::cout << "Joined multicast group successfully." << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Failed to initialize receiver: " << e.what() << std::endl;
        return 1;
    }

    // Start handler
    handler.startConsumer();

    // Statistics reporting thread
    std::thread statsThread([&]() {
        size_t lastCount = 0;
        while (running.load(std::memory_order_acquire)) {
            std::this_thread::sleep_for(std::chrono::seconds(5));

            size_t count = handler.getMessageCount();
            size_t delta = count - lastCount;
            lastCount = count;

            if (delta > 0) {
                std::cout << "\n--- Status Update ---" << std::endl;
                std::cout << "Messages processed: " << count << " (+" << delta << ")" << std::endl;
                std::cout << "Rate: " << (delta / 5) << " msg/sec" << std::endl;
                std::cout << "Orders in book: " << handler.getBook().orderCount() << std::endl;

                if (auto bid = handler.getBook().bestBid()) {
                    std::cout << "Best bid: " << bid->price / 100.0 << std::endl;
                }
                if (auto ask = handler.getBook().bestAsk()) {
                    std::cout << "Best ask: " << ask->price / 100.0 << std::endl;
                }
            }
        }
    });

    // Run receiver (blocking)
    receiver.run([&](const char* data, size_t len, Timestamp ts) {
        if (!running.load(std::memory_order_acquire)) {
            receiver.stop();
            return;
        }

        if (len >= sizeof(MarketDataMessage)) {
            const auto* msg = reinterpret_cast<const MarketDataMessage*>(data);
            handler.pushEvent(*msg, ts);
        }
    });

    // Cleanup
    handler.waitForDrain();
    running.store(false, std::memory_order_release);
    statsThread.join();

    // Print final statistics
    std::cout << "\n=== Final Statistics ===" << std::endl;
    latency.printStats("Processing Latency");

    std::cout << "\n=== Order Book Final State ===" << std::endl;
    std::cout << "Total orders: " << handler.getBook().orderCount() << std::endl;
    std::cout << "Bid levels: " << handler.getBook().bidLevelCount() << std::endl;
    std::cout << "Ask levels: " << handler.getBook().askLevelCount() << std::endl;

    return 0;
}
