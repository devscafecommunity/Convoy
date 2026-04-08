#pragma once
#include <iostream>
#include <string>
#include <sstream>
#include <iomanip>

namespace convoy {

class Logger {
public:
    template<typename... Args>
    static void info(const std::string& msg, Args&&...) {
        std::cout << "[INFO] " << msg << std::endl;
    }
    
    template<typename... Args>
    static void error(const std::string& msg, Args&&...) {
        std::cerr << "[ERROR] " << msg << std::endl;
    }
};

}
