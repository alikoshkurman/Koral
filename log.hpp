#pragma once
#include <utility>
#include <iostream>

namespace Koral {
struct log_body {
    template <typename... Args>
    void error(Args&&... arg) const {
        std::cout << "\033[31m[ERROR] ";
        (std::cout << ... << std::forward<Args>(arg)) << "\033[0m";
    }

    template <typename... Args>
    void warn(Args&&... arg) const {
        std::cout << "\033[33m[WARNING] ";
        (std::cout << ... << std::forward<Args>(arg)) << "\033[0m";
    }

    template <typename... Args>
    void debug(Args&&... arg) const {
        std::cout << "\033[35m[DEBUG] "; 
        (std::cout << ... << std::forward<Args>(arg)) << "\033[0m";
    }

    template <typename... Args>
    void ok(Args&&... arg) const {
        std::cout << "\033[32m[OK] ";
        (std::cout << ... << std::forward<Args>(arg)) << "\033[0m";
    }

    template <typename... Args>
    void info(Args&&... arg) const {
        std::cout << "\033[36m[INFO] "; 
        (std::cout << ... << std::forward<Args>(arg)) << "\033[0m";
    }

    template <typename... Args>
    void panic(Args&&... arg) const {
        std::cout << "\033[1;37;41m[PANIC] "; 
        (std::cout << ... << std::forward<Args>(arg)) << "\033[0m";
    }
};


inline constexpr log_body log;
}
