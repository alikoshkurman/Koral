#include "modules.hpp"

namespace Koral {
inline std::shared_mutex cout_mutex;

struct skip_body {
  auto sigwinch() {
    return std::signal(SIGWINCH, SIG_IGN);
  }
}
inline skip_body skip;

using str = std::string;
using strv = std::string_view;
using std::vector;

struct to_body {
std::string lower(std::string str) {
    std::transform(str.begin(), str.end(), str.begin(), [](unsigned char c) {
        return std::tolower(c);
    });
    return str;
}
}; 
to_body to;


template <typename T>
using randn = std::uniform_real_distribution<T>;

template <typename... Args>
auto f(std::format_string<Args...> fmt, Args&&... args) {
    return std::format(fmt, std::forward<Args>(args)...);
}

template <typename... Args>
void println(Args&&... arg) {
  std::unique_lock<std::shared_mutex> lock(cout_mutex);
  (std::cout << ... << std::forward<Args>(arg));
  std::cout << '\n';
}

template <typename... Args>
void raise(Args&&... arg) {
  std::unique_lock<std::shared_mutex> lock(cout_mutex);
  (std::cout << "\033[31m" << ... << std::forward<Args>(arg));
  std::cout << "\033[31m\n";
}

template <typename... Args>
void print(Args&&... arg) {
  std::unique_lock<std::shared_mutex> lock(cout_mutex);
  (std::cout << ... << std::forward<Args>(arg));
}

template <typename... Args>
void printlnErr(Args&&... arg) {
  std::unique_lock<std::shared_mutex> lock(cout_mutex);
  (std::cerr << ... << std::forward<Args>(arg));
  std::cout << '\n';
}

template <typename... Args>
void printErr(Args&&... arg) {
  std::unique_lock<std::shared_mutex> lock(cout_mutex);
  (std::cerr << ... << std::forward<Args>(arg));
}

inline std::random_device rd;
inline std::mt19937 gen(rd());

template <typename... T>
void use(const T&...) {}


template <typename T> 
using heap_ptr = std::unique_ptr<T>;

template <typename T, typename... Args>
[[nodiscard]] auto heap(Args&&... args) {
    return std::make_unique<T>(std::forward<Args>(args)...);
}

struct set_body {
  void perms(const str& name, int perm) const {
    std::filesystem::permissions(name, static_cast<std::filesystem::perms>(perm), std::filesystem::perm_options::replace);
  }
};
inline set_body set;
}
