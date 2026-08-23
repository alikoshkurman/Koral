#pragma once
#include <ctime>
#include <time.h>
#include <chrono>
#include <thread>


namespace Koral {
  enum TC {
    ns, mcs, ms, s, min, hour, day, week, month, year
  };
std::tm Time;


struct time_body {
  void update() noexcept {
    std::time_t nwa = std::time(nullptr);
  
    localtime_r(&nwa, &Time);
  
  }
  
  [[nodiscard]] auto hour() noexcept {
    return Time.tm_hour;
  }
  
  [[nodiscard]] auto min() noexcept {
    return Time.tm_min;
  }
  [[nodiscard]] auto sec() noexcept {
    return Time.tm_sec;
  }
  [[nodiscard]] auto day() noexcept {
    return Time.tm_mday;
  }
  [[nodiscard]] auto week() noexcept {
    return Time.tm_wday+1;
  }
  [[nodiscard]] auto month() noexcept {
    return Time.tm_mon+1;
  }
  [[nodiscard]] auto year() noexcept {
    return Time.tm_year+1900;
  }
  
  auto now() noexcept {
    return std::chrono::high_resolution_clock::now();
  }
  
  [[nodiscard]] auto ns() noexcept {
    auto t = now().time_since_epoch();
    return std::chrono::duration_cast<std::chrono::nanoseconds>(t).count() % 1'000'000'000;
  }

  [[nodiscard]] auto mcs() noexcept {
    auto t = now().time_since_epoch();
    return std::chrono::duration_cast<std::chrono::microseconds>(t).count() % 1'000'000;
  }

  [[nodiscard]] auto ms() noexcept {
    auto t = now().time_since_epoch();
    return std::chrono::duration_cast<std::chrono::milliseconds>(t).count() % 1'000;
  }
  
  
  
  template <TC sit>
  void sleep(int num) noexcept {
    if constexpr (sit == TC::ns) {
      std::this_thread::sleep_for(std::chrono::nanoseconds(num));
    } else if constexpr (sit == TC::mcs) {
      std::this_thread::sleep_for(std::chrono::microseconds(num));
    } else if constexpr (sit == TC::ms) {
      std::this_thread::sleep_for(std::chrono::milliseconds(num));
    } else if constexpr (sit == TC::s) {
      std::this_thread::sleep_for(std::chrono::seconds(num));
    } else if constexpr (sit == TC::min) {
      std::this_thread::sleep_for(std::chrono::minutes(num));
    } else if constexpr (sit == TC::hour) {
      std::this_thread::sleep_for(std::chrono::hours(num));
    } else if constexpr (sit == TC::day) {
      std::this_thread::sleep_for(std::chrono::days(num));
    } else if constexpr (sit == TC::week) {
      std::this_thread::sleep_for(std::chrono::weeks(num));
    } else if constexpr (sit == TC::month) {
      std::this_thread::sleep_for(std::chrono::months(num));
    } else if constexpr (sit == TC::year) {
      std::this_thread::sleep_for(std::chrono::years(num));
    }
    
  }
  
  template <typename DurationType, typename TimePoint>
  auto backTime(TimePoint arg2, TimePoint arg3) {
    return std::chrono::duration_cast<DurationType>(arg2 - arg3).count();
  }
};
inline time_body Clock;

}