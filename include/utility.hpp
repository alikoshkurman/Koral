#pragma once
#include <chrono>
#include <csignal>
#include <cstdlib>
#include <filesystem>
#include <memory>
#include <random>
#include <string>
#include <string_view>
#include <thread>
#include <type_traits>
#include <utility>
#ifdef _MSVC_LANG
  #define MY_CPP_STANDARD _MSVC_LANG
#else
  #define MY_CPP_STANDARD __cplusplus
#endif

#if MY_CPP_STANDARD < 202302L
  #error "Need C++23 or greater."
#endif

#undef MY_CPP_STANDARD

namespace Koral {
  struct NonCopy {
protected:
    NonCopy() = default;
    ~NonCopy() = default;
public: 
    NonCopy(const NonCopy&) = delete;
    NonCopy& operator=(const NonCopy&) = delete;
};
    struct NonMove {
  protected:
    NonMove() = default;
    ~NonMove() = default;
  public:
    NonMove(NonMove&&) = delete;
    NonMove& operator=(NonMove&&) = delete;
  };
  using argv_t=char**; using ptr_t=const void*; using ptr_mut_t=void*;
  
  
  void Terminate() noexcept {
     std::abort();
  }

  
  template <typename T>
  [[nodiscard]] inline constexpr const T* cast_ptr_to(ptr_t ptr) noexcept { return static_cast<const T*>(ptr); }
  
  template <typename T>
  [[nodiscard]] inline T* cast_ptr_to(ptr_mut_t ptr) noexcept { return static_cast<T*>(ptr); }
  
  template <typename T, typename UnknownType>
  inline void cast_ptr_to(UnknownType x) = delete;
  
  inline std::random_device rd;
inline std::mt19937 gen(rd());

template <typename T>
using randn = std::conditional_t<
    std::is_floating_point_v<T>,
    std::uniform_real_distribution<T>,
    std::uniform_int_distribution<T>
>;
struct skip_body {
    auto sigwinch() {
      return std::signal(SIGTERM, SIG_IGN);
    }
  };
  inline skip_body skip;

template<typename Func>
void GNIFE(std::string_view ptr, Func func) {
  const std::string_view fpath = std::move(ptr);
  while (!std::filesystem::exists(fpath)) {
    if (func()) { 
      break; 
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
  }
}
template <typename... T>
void use(const T&...) {}
std::string_view trim(std::string_view sv) {
  size_t start = 0;
  while (start < sv.size() && (sv[start] == ' ' || sv[start] == '\n' || sv[start] == '\t' || sv[start] == '\r')) {
    ++start;
  }

  size_t end = sv.size();
  while (end > start && (sv[end - 1] == ' ' || sv[end - 1] == '\n' || sv[end - 1] == '\t' || sv[end - 1] == '\r')) {
    --end;
  }

  return sv.substr(start, end - start);
}
struct set_body {
  void perms(const std::string& name, int perm) const {
    std::filesystem::permissions(name, static_cast<std::filesystem::perms>(perm), std::filesystem::perm_options::replace);
  }
};
inline set_body set;
  

template <typename T>
[[nodiscard]] constexpr const void* ptr_cast(const T& some) noexcept {
    using PureT = std::remove_cvref_t<T>;

    if constexpr (std::is_function_v<PureT> || std::is_function_v<std::remove_pointer_t<PureT>>) {
        return reinterpret_cast<const void*>(some);
    } else if constexpr (std::is_pointer_v<PureT>) {
        return static_cast<const void*>(some);
    } else if constexpr (requires { std::to_address(some); }) {
        return static_cast<const void*>(std::to_address(some));
    } else {
        return static_cast<const void*>(std::addressof(some));
    }
}

}
