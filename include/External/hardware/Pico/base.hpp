#pragma once
#include "pico/stdlib.h"
#include "../../log.hpp"
#include "../../utility.hpp"
#include <vector>

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
    bool ms(uint32_t num) noexcept {
        sleep_ms(num);
      return true;
    }
  }
  
  struct Init : public Koral::NonCopy, public Koral::NonMove {
    Init() noexcept {
      stdio_init_all();
    }

    ~Init() noexcept {
      stdio_deinit_all();
    }

    bool init() noexcept {
      stdio_init_all();
      return true;
    }

    bool deinit() noexcept {
      stdio_deinit_all();
      return true;
    }
  };

  inline constexpr bool ON = true;
  inline constexpr bool OFF = false;

   inline constexpr bool UP = true;
  inline constexpr bool DOWN = false;

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
  constexpr int PUT = 8;
}
  namespace GPIO {
    constexpr uint32_t BANK0 = NUM_BANK0_GPIOS;
    constexpr uint32_t QSPI = NUM_QSPI_GPIOS; 


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

      struct Pin : public Koral::NonCopy, public Koral::NonMove {
      bool mode = false;
      uint pins = 0;
      bool state = false;

      template <uint mode, bool up = true, typename... Args>
      Pin(Args... pin) noexcept: mode(mode), pins((0u | ... | (1u << pin))) {
          gpio_init_mask(pins);
          if constexpr (mode == 0u) {
            if constexpr (up) {
              (gpio_pull_up(pin), ...); 
            } else {
              (gpio_pull_down(pin), ...); 
            }
          } else if constexpr (mode == 1u) {
            gpio_set_dir_out_masked(pins);
          } else {
            Koral::out.error("Koral::hardware::GPIO::mask::Pin: Bad argument.\n");
          }
      }
      
      ~Pin() {
          gpio_set_function_masked(pins, GPIO_FUNC_NULL);
      }

      void deinit() noexcept {
        gpio_set_function_masked(pins, GPIO_FUNC_NULL);
      }

      void init() noexcept {
        gpio_init_mask(pins);
      }

      template <uint mode>
      void dir() noexcept {
        if constexpr (mode == 0u) {
            gpio_set_dir_in_masked(pins);
        } else if constexpr (mode == 1u) {
          gpio_set_dir_out_masked(pins);
        } else {
          Koral::out.error("Koral::hardware::GPIO::mask::Pin: Bad argument.\n");
        }
      }

      void put(bool value) noexcept {
          gpio_put_masked(pins, value);
          state = value;
      }

      template<typename T>
      void func(T func) noexcept {
        gpio_set_function_masked(pins, static_cast<gpio_function_t>(func));
      }

      void toggle() {
        state = !state;
        gpio_put_masked(pins, state);
      }

      uint get() noexcept {
        return gpio_get_all() & pins;
      }
    };
    }

    namespace all {
      void init() noexcept {
        gpio_init_mask((1u << BANK0) - 1u);
      }

      void deinit() noexcept {
        for (uint32_t pin = 0; pin < BANK0; ++pin) {
          gpio_deinit(pin);
        }
      }

      inline uint state() noexcept {
        return gpio_get_all();
      }
    
    }
    struct Pin : public Koral::NonCopy, public Koral::NonMove {
      uint pin = 0;
      bool mode = false;
      bool state = false;
  
      Pin(uint pin, bool mode = 0, bool up = true) : pin(pin), mode(mode) {
          gpio_init(pin);
          if (!mode) {
            gpio_set_dir(pin, 0);
          } else {
            if (up) {
              gpio_pull_up(pin);
            } else {
              gpio_pull_down(pin);
            }
          }
      }
      ~Pin() {
          gpio_deinit(pin);
      }

      template <bool up>
      void pull() noexcept {
        if constexpr (up) gpio_pull_up(pin);
        else gpio_pull_down(pin);
      }

      void deinit() {
          gpio_deinit(pin);
       
      }

      void init() {
          gpio_init(pin);
       
      }

      void dir(bool mode) {
          gpio_set_dir(pin, mode);
    
      }

      void put(bool value) {
          gpio_put(pin, value);
          state = value;
      }

      

      template<typename T>
      void func(T func) {
        gpio_set_function(pin, static_cast<gpio_function_t>(func));

      }

      void toggle() {
        state = !state;
        gpio_put(pin, state);
    
      }

      uint get() noexcept {
        return gpio_get(pin);
      }
    };

    inline void initAll() noexcept {
      gpio_init_mask((1u << BANK0) - 1u);
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
