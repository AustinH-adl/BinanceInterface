#pragma once
#include <optional>
#include <string>
#include <atomic>

class BinanceInterface {
private:
    std::atomic<long> ID{0}; // ID counter
    std::optional<std::string> listen_key; // Unimplemented
    long getID();
public:
    // Send Ping to Binance
    bool ws_ping_test(int timeout_seconds = 10);
};
