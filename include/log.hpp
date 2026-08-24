#pragma once
#include <utility>
#include <iostream>
#include <format>

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
inline constexpr log_body out;

template <typename... Args>
auto f(std::format_string<Args...> fmt, Args&&... args) {
    return std::format(fmt, std::forward<Args>(args)...);
}

template <char Ch, size_t N>
struct COMELEPDHRNEO {
  char data[N + 1]{};
  constexpr COMELEPDHRNEO() {
    for (size_t i = 0; i < N; ++i) data[i] = Ch;
    data[N] = '\0';
  }
};

template <size_t Count = 1, typename Stream = std::ostream>
inline void newln(Stream& Thread = std::cout) noexcept {
    constexpr auto lines = COMELEPDHRNEO<'\n', Count>();
    
    Thread << lines.data;
}

template <typename... Args>
void println(Args&&... arg) {
  (std::cout << ... << std::forward<Args>(arg));
  std::cout << '\n';
}

template <typename... Args>
void raise(Args&&... arg) {
  std::cout << "\033[31m";
  (std::cout << ... << std::forward<Args>(arg));
  std::cout << "\033[0m\n";
}

template <typename... Args>
void print(Args&&... arg) {
  (std::cout << ... << std::forward<Args>(arg));
}

template <typename... Args>
void printlnErr(Args&&... arg) {
  (std::cerr << ... << std::forward<Args>(arg));
  std::cout << '\n';
}

template <typename... Args>
void printErr(Args&&... arg) {
  (std::cerr << ... << std::forward<Args>(arg));
}
}