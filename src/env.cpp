#include "env.hpp"
#include <iostream>
#include <fstream>
#include <string>
#include <filesystem>
#include <algorithm>
#include <cctype>
#include <vector>

// Define static members
std::string environment_variables::API_KEY = "";
std::string environment_variables::SECRET_KEY = "";
std::string environment_variables::WS_URL = "";
std::string environment_variables::API_URL = "";

void environment_variables::load() {
    std::cout << "[START] Loading environment variables from .env" << std::endl;
    // Try several sensible relative locations for the .env file. Prefer project-root `constant/.env`.
    std::vector<std::string> candidates = {"constant/.env", "./constant/.env", "../constant/.env"};
    std::string envPath;
    std::ifstream envFile;

    for (const auto &p : candidates) {
        if (std::filesystem::exists(p)) {
            envPath = p;
            envFile.open(envPath);
            break;
        }
    }

    if (!envFile.is_open()) {
        // If none of the candidates existed, still try to open the first candidate to give a clear message.
        envPath = candidates.front();
        envFile.open(envPath);
    }

    if (!envFile.is_open()) {
        std::cerr << "[WARN] Could not open any .env file. Tried:";
        for (const auto &c : candidates) std::cerr << ' ' << c;
        std::cerr << std::endl;
        return;
    }

    auto trim = [](std::string &s) {
        // left
        s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) { return !std::isspace(ch); }));
        // right
        s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) { return !std::isspace(ch); }).base(), s.end());
    };

    std::string line;
    while (std::getline(envFile, line)) {
        // remove carriage return if present
        if (!line.empty() && line.back() == '\r') line.pop_back();

        // skip empty lines and comments
        std::string tmp = line;
        trim(tmp);
        if (tmp.empty() || tmp[0] == '#') continue;

        // find first '='
        size_t eq = line.find('=');
        if (eq == std::string::npos) continue; // malformed line

        std::string key = line.substr(0, eq);
        std::string value = line.substr(eq + 1);
        trim(key);
        trim(value);

        // The .env values are not quoted per spec. Map keys to class members.
        if (key == "TESTNET_KEY") {
            environment_variables::API_KEY = value;
        } else if (key == "TESTNET_SECRET") {
            environment_variables::SECRET_KEY = value;
        } else if (key == "WS_ENDPOINT") {
            environment_variables::WS_URL = value;
        } else if (key == "API_ENDPOINT") {
            environment_variables::API_URL = value;
        } else {
            // Unknown key: ignore or log at debug level
            std::cout << "[DEBUG] Ignoring unknown env key: " << key << std::endl;
        }
    }

    envFile.close();
}