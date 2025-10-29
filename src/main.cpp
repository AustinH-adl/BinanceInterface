#include <iostream>
#include <string>
#include "env.hpp"
#include "binance_interface.hpp"

int main() {
    std::cout << "Websocket ping test to Binance API...\n";

    environment_variables::load();

    if (environment_variables::WS_URL.empty()) {
        std::cerr << "[ERROR] WS_URL is required in the .env\n";
        return 1;
    }
    if (environment_variables::API_URL.empty()) {
        std::cerr << "[ERROR] WS_URL is required in the .env\n";
        return 1;
    }

    BinanceInterface bi;
    bool ok = bi.ws_ping_test(8);
    if (!ok) {
        std::cerr << "[ERROR] websocket ping test failed or timed out\n";
        return 1;
    }

    std::cout << "websocket ping test succeeded\n";
    return 0;
}
