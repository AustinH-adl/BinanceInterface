#include <iostream>
#include "env.hpp"

int main() {
    environment_variables::load();
    std::cout << "API_KEY set: " << (!environment_variables::API_KEY.empty()) << "\n";
    std::cout << "SECRET_KEY set: " << (!environment_variables::SECRET_KEY.empty()) << "\n";
    std::cout << "BASE_URL: " << (environment_variables::BASE_URL.empty() ? "<empty>" : environment_variables::BASE_URL) << "\n";
    return 0;
}
