#include "binance_interface.hpp"
#include "env.hpp"
#include <iostream>
#include <string>
#include <sstream>
#include <atomic>
#include <thread>
#include <chrono>
#include <functional>

// websocketpp (header-only). Requires linking Boost.Asio and OpenSSL for TLS (wss).
#include <websocketpp/config/asio_client.hpp>
#include <websocketpp/client.hpp>

using ws_client = websocketpp::client<websocketpp::config::asio_tls_client>;
using connection_hdl = websocketpp::connection_hdl;

long BinanceInterface::getID() {
    long temp = ID;
    ID++;
    return temp;
}

bool BinanceInterface::ws_ping_test(int timeout_seconds) {
    const std::string url = environment_variables::WS_URL;
    if (url.empty()) {
        std::cerr << "[BinanceInterface] WS_URL is empty\n";
        return false;
    }

    std::ostringstream __ping_ss;
    __ping_ss << R"({"id":")" << getID() << R"(","method":"ping"})";
    const std::string json_ping = __ping_ss.str();

    // websocketpp client
    ws_client client;
    std::atomic<bool> received{false};
    std::atomic<bool> opened{false};
    std::atomic<bool> failed{false};

    client.clear_access_channels(websocketpp::log::alevel::all);
    client.clear_error_channels(websocketpp::log::elevel::all);

    // TLS init handler
    client.set_tls_init_handler([](connection_hdl) {
        std::shared_ptr<boost::asio::ssl::context> ctx =
            std::make_shared<boost::asio::ssl::context>(boost::asio::ssl::context::sslv23);
        ctx->set_options(boost::asio::ssl::context::default_workarounds
                         | boost::asio::ssl::context::no_sslv2
                         | boost::asio::ssl::context::no_sslv3
                         | boost::asio::ssl::context::single_dh_use);
        return ctx;
    });

    // on_open: send ping JSON
    client.set_open_handler([&client, &json_ping, &opened](connection_hdl hdl) {
        opened = true;
        websocketpp::lib::error_code ec;
        client.send(hdl, json_ping, websocketpp::frame::opcode::text, ec);
        if (ec) {
            std::cerr << "[BinanceInterface] send failed: " << ec.message() << "\n";
        }
    });

    // on_message: print payload and mark received
    client.set_message_handler([&received](connection_hdl, ws_client::message_ptr msg) {
        std::cout << "[BinanceInterface] ws message: " << msg->get_payload() << "\n";
        received = true;
    });

    // on_fail/on_close
    client.set_fail_handler([&failed, &client](connection_hdl hdl) {
        failed = true;
        ws_client::connection_ptr con = client.get_con_from_hdl(hdl);
        std::cerr << "[BinanceInterface] connection failed: " << con->get_ec().message() << "\n";
    });

    client.set_close_handler([&client](connection_hdl) {
        // no-op
    });

    // start ASIO
    client.init_asio();

    websocketpp::lib::error_code ec;
    ws_client::connection_ptr con = client.get_connection(url, ec);
    if (ec) {
        std::cerr << "[BinanceInterface] get_connection error: " << ec.message() << "\n";
        return false;
    }

    client.connect(con);

    // run ASIO in background thread
    std::thread run_thread([&client]() {
        client.run();
    });

    // wait for either a message received or timeout
    auto start = std::chrono::steady_clock::now();
    bool ok = false;
    while (true) {
        if (received) {
            ok = true;
            break;
        }
        if (failed) break;
        auto now = std::chrono::steady_clock::now();
        if (std::chrono::duration_cast<std::chrono::seconds>(now - start).count() >= timeout_seconds) break;
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }

    // close and stop
    try {
        websocketpp::lib::error_code close_ec;
        con->close(websocketpp::close::status::normal, "test complete", close_ec);
        if (close_ec) {
            // ignore
        }
    } catch (...) {
        // ignore
    }

    client.stop();
    if (run_thread.joinable()) run_thread.join();

    return ok;
}