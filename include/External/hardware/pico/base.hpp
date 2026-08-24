#pragma once
#include "pico/stdlib.h"
#include "../log.hpp"
#include "../utility.hpp"

namespace Koral::hardware::pico {
  namespace sleep {
    bool us(uint32_t num) noexcept {
        sleep_us(num);
      return true;
    }

    bool s(uint32_t num) noexcept {
        sleep_ms(num * 1000);
      return true;
    }
    s
    bool ms(uint32_t num) noexcept {
        sleep_ms(num);
      return true;
    }
  }
  
  struct Init {
    Init() noexcept {
      if(stdio_init_all()) {
        Koral::out.error("Koral::firmware::pico: Can't initialise I/O.\n");
      } 
    }
    ~Init() noexcept {
      stdio_deinit_all();
    }

    bool init() noexcept {
      if(stdio_init_all()) {
        Koral::out.error("Koral::firmware::pico: Can't initialise I/O.\n");
        return false;
      } 
      return true;
    }

    bool deinit() noexcept {
      if(stdio_deinit_all()) {
        Koral::out.error("Koral::firmware::pico: Can't deinitialise I/O.\n");
        return false;
      }
      return true;
    }
  };
  inline constexpr bool on = true;
  inline constexpr bool off = false;

  namespace Func {
      enum Func : uint32_t {
        XIP  = 0u,
        SPI  = 1u,
        UART = 2u,
        I2C  = 3u,
        PWM  = 4u,
        SIO  = 5u,
        PIO0 = 6u,
        PIO1 = 7u,
        GPCK = 8u,
        USB  = 9u,
        Null = 15u
      };
  }
  namespace ERR {
  constexpr int INIT = 7;
   constexpr int INITIO = 7;
}
  namespace GPIO {
    [[nodiscard]] constexpr uint32_t B0() noexcept { return NUM_BANK0_GPIOS; }
    [[nodiscard]] constexpr uint32_t QS()  noexcept { return NUM_QSPI_GPIOS; }


    namespace mask {
      template<typename... Args>
      bool init(Args... pin) noexcept {
        if (!(gpio_is_valid(pin) && ...)) {
          Koral::out.error("Koral::hardware::pico::GPIO: Invalid pin.\n");
          return false;
        }

        gpio_init_mask((0u | ... | (1u << pin)));
        return true;
      }
    }

    namespace all {
      void init() noexcept {
        gpio_init_mask((1u << B0()) - 1u);
      }

      void deinit() noexcept {
        for (uint32_t pin = 0; pin < B0(); ++pin) {
          gpio_deinit(pin);
        }
      }
    
    }
    struct Pin : public Koral::NonCopy, public Koral::NonMove {
      uint pin = 0;
      bool mode = false;
      bool state = false;
  
      Pin(uint pin, bool mode) : pin(pin), mode(mode) {
          gpio_init(pin);
          gpio_set_dir(pin, mode);
      }
      ~Pin() {
          gpio_deinit(pin);
      }

      bool deinit() {
          gpio_deinit(pin);
        return true;
      }

      bool init() {
          gpio_init(pin);
        return true;
      }

      bool dir(bool mode) {
          gpio_set_dir(pin, mode);
        return true;
      }

      bool put(bool value) {
          gpio_put(pin, value);
          state = value;
        return true;
      }

      template<typename T>
      bool func(T func) {
        gpio_set_function(pin, static_cast<gpio_function_t>(func));
        return true;
      }

      bool toggle() {
        state = !state;
        gpio_put(pin, state);
        return true;
      }
    };

    bool initAll() noexcept {
      gpio_init_mask((1u << B0()) - 1u);
      return true;
    }

    enum Dir : uint32_t {
        In  = 0u,
        Out = 1u
    };

    enum IrqEvent : uint32_t {
        LevelLow  = 0x1u,
        LevelHigh = 0x2u,
        EdgeFall  = 0x4u,
        EdgeRise  = 0x8u
    };

    enum DriveStrength : uint32_t {
        Drive2mA  = 0u,
        Drive4mA  = 1u,
        Drive8mA  = 2u,
        Drive12mA = 3u
    };

    enum SlewRate : uint32_t {
        Slow = 0u,
        Fast = 1u
    };

    enum PullResistor : uint32_t {
        None = 0u,
        Up   = 1u,
        Down = 2u
    };

  }

}
