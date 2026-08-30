#pragma once
#include "pico/cyw43_arch.h"
#include "../..log.hpp"
#include "../utility.hpp"

namespace Koral::hardware::pico::WIFI {
  namespace conn {
    bool us(const char* srr, const char* srr2, uint32_t some, uint32_t num) {
      if(cyw43_arch_wifi_connect_timeout_ms(srr, srr2, some, (num + 999u) / 1000u)) {
        Koral::out.error("Koral::firmware::pico::WIFI: Can't connect to Wi-Fi.\n");
        return false;
      }
      return true;
    }
    bool ms(const char* srr, const char* srr2, uint32_t some, uint32_t num) {
      if(cyw43_arch_wifi_connect_timeout_ms(srr, srr2, some, num)) {
        Koral::out.error("Koral::firmware::pico::WIFI: Can't connect to Wi-Fi.\n");
        return false;
      }
      return true;
    }
    bool now(const char* srr, const char* srr2, uint32_t some) {
      if(cyw43_arch_wifi_connect_blocking(srr, srr2, some)) {
        Koral::out.error("Koral::firmware::pico::WIFI: Can't connect to Wi-Fi.\n");
        return false;
      }
      return true;
    }
    bool s(const char* srr, const char* srr2, uint32_t some, uint32_t num) {
      if(cyw43_arch_wifi_connect_timeout_ms(srr, srr2, some, num*1000)) {
        Koral::out.error("Koral::firmware::pico::WIFI: Can't connect to Wi-Fi.\n");
        return false;
      }
      return true;
    }
  }
  namespace disconn {
    bool us(uint32_t num) {
      (void)num;
      cyw43_arch_disable_sta_mode();
      return true;
    }
    bool now() {
      cyw43_arch_disable_sta_mode();
      return true;
    }
    bool ms(uint32_t num) {
      (void)num;
      cyw43_arch_disable_sta_mode();
      return true;
    }
    bool s(uint32_t num) {
      (void)num;
      cyw43_arch_disable_sta_mode();
      return true;
    }
  }
    
  enum driver {
    open = 0,
    WPA = 0x00200002,
    WPA2 = 0x00400004,
    WPA3 = 0x00400006
  };
  
  namespace ERR {
    constexpr int INIT = 30;
    constexpr int ENABLE = 31;
    constexpr int PUT = 32;
    constexpr int POLL = 33;
    constexpr int CONN = 34;
    constexpr int DISCONN = 35;
  }

  struct Init {
    Init() noexcept {
      if(cyw43_arch_init()) {
        Koral::out.error("Koral::firmware::pico::WIFI: Can't initialise Wi-Fi.\n");
      }
    }
    ~Init() noexcept = default;

    bool init() noexcept {
      if(cyw43_arch_init()) {
        Koral::out.error("Koral::firmware::pico::WIFI: Can't initialise Wi-Fi.\n");
        return false;
      }
      return true;
    }

    bool deinit() noexcept {
      cyw43_arch_deinit();
      return true;
    }
  };

  struct Pin {
    uint pin = 0;
    bool state = false;

    Pin(uint pin) : pin(pin) {}

    static constexpr uint LED() noexcept { return 0; }
    static constexpr uint SMPS() noexcept { return 1; }
    static constexpr uint VBUS() noexcept { return 2; }
    
    bool toggle() noexcept {
      state = !state;
      cyw43_arch_gpio_put(pin, state);
      return true;
    }

    bool put(bool value) noexcept {
      state = value;
      cyw43_arch_gpio_put(pin, state);
      return true;
    }
  };

  inline bool put(uint pin, bool state) noexcept {
    cyw43_arch_gpio_put(pin, state);
    return true;
  }

 
  inline bool enable() noexcept {
    cyw43_arch_enable_sta_mode();
    return true;
  
}
  bool poll() noexcept {
    cyw43_arch_poll();
    return true;
  }
  }