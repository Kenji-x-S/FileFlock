#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <string>

class Config {
public:
    static bool isServerMode();
    static void loadConfig();
};

#endif // CONFIG_HPP
