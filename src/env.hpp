#pragma once

#include <string>

class environment_variables {
public:
    static std::string API_KEY;
    static std::string SECRET_KEY;
    static std::string WS_URL;
    static std::string API_URL;
    static void load();
};