#include "config.hpp"
#include <iostream>

bool Config::isServerMode() {
    return true;  // For now, we'll hardcode it to return true (server mode)
}

void Config::loadConfig() {
    // Load configuration from a file or hardcoded settings
    std::cout << "Config loaded.\n";
}
