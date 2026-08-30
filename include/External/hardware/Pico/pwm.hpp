#pragma once
#include "hardware/pwm.h"
#include "base.hpp"
#include "../../log.hpp"

namespace Koral::hardware::pico::PWM {
  struct config {
    static constexpr uint16_t MAX_LEVEL = 5000;
    bool succ = true;
    pwm_config cfg = {};

    config() noexcept: cfg(pwm_get_default_config()) {}

    config(uint32_t frequency, float clkdiv, bool phase = true, bool A = false, bool B = true, pwm_clkdiv_mode mode = PWM_DIV_FREE_RUNNING) noexcept {
      if(frequency == 0) { 
        Koral::out.error("Koral::hardware::pico::PWM::config: Frequency must be > 0.\n");
        succ = false;
        return;
      }
      pwm_config_set_wrap(&cfg, 125000000 / frequency);
      pwm_config_set_clkdiv(&cfg, clkdiv);
      pwm_config_set_phase_correct(&cfg, phase);
      pwm_config_set_output_polarity(&cfg, A, B);
      pwm_config_set_clkdiv_mode(&cfg, mode);
      succ = true;
    }

    config(uint32_t frequency, int clkdiv, bool phase = true, bool A = false, bool B = true, pwm_clkdiv_mode mode = PWM_DIV_FREE_RUNNING) noexcept {
      if(frequency == 0) { 
        Koral::out.error("Koral::hardware::pico::PWM::config: Frequency must be > 0.\n");
        succ = false;
        return;
      }
      pwm_config_set_wrap(&cfg, 125000000 / frequency);
      pwm_config_set_clkdiv_int(&cfg, clkdiv);
      pwm_config_set_phase_correct(&cfg, phase);
      pwm_config_set_output_polarity(&cfg, A, B);
      pwm_config_set_clkdiv_mode(&cfg, mode);
      succ = true;
    }

    config& custom(uint32_t frequency, float clkdiv, bool phase = true, bool A = false, bool B = true, pwm_clkdiv_mode mode = PWM_DIV_FREE_RUNNING) noexcept {
      if(frequency == 0) { 
        Koral::out.error("Koral::hardware::pico::PWM::config::custom: Frequency must be > 0.\n");
        succ = false;
        return *this;
      }
      pwm_config_set_wrap(&cfg, 125000000 / frequency);
      pwm_config_set_clkdiv(&cfg, clkdiv);
      pwm_config_set_phase_correct(&cfg, phase);
      pwm_config_set_output_polarity(&cfg, A, B);
      pwm_config_set_clkdiv_mode(&cfg, mode);
      succ = true;
      return *this;
    }

    config& custom(uint32_t frequency, int clkdiv, bool phase = true, bool A = false, bool B = true, pwm_clkdiv_mode mode = PWM_DIV_FREE_RUNNING) noexcept {
      if(frequency == 0) { 
        Koral::out.error("Koral::hardware::pico::PWM::config::custom: Frequency must be > 0.\n");
        succ = false;
        return *this;
      }
      pwm_config_set_wrap(&cfg, 125000000 / frequency);
      pwm_config_set_clkdiv_int(&cfg, clkdiv);
      pwm_config_set_phase_correct(&cfg, phase);
      pwm_config_set_output_polarity(&cfg, A, B);
      pwm_config_set_clkdiv_mode(&cfg, mode);
      succ = true;
      return *this;
    }

    config& init(uint slice, bool channel) noexcept {
      pwm_init(slice, &cfg, channel);
      return *this;
    }

    config& level(uint slice, bool channel, uint strongPercent) noexcept {
      if(strongPercent > 100) { 
        Koral::out.error("Koral::hardware::pico::PWM::config::level: Duty cycle must be 0-100%.\n");
        succ = false;
        return *this;
      }
      pwm_set_chan_level(slice, channel, MAX_LEVEL/100*strongPercent);
      succ = true;
      return *this;
    }

  }; 

  inline void loop() noexcept {
    tight_loop_contents();
  }

  enum class div : uint {
    Frunning = 0, 
    Bhigh = 1, 
    Bfalling = 2, 
    Brising = 3
  };

  enum chan : bool {
    A = true,
    B = false
  };
  
  struct Pin : public NonCopy, public NonMove {
    uint chan = 0;
    uint slice = 0;
    uint pin = 0;
    bool succ = true;

    
    Pin(uint pin_num): chan(pwm_gpio_to_channel(pin_num)), slice(pwm_gpio_to_slice_num(pin_num)), pin(pin_num) {
      gpio_init(pin_num);
        
      if (pin_num >= NUM_BANK0_GPIOS) {
        Koral::out.error("Koral::hardware::pico::pwm::Pin: Invalid pin number.\n");
        succ = false;
        return;
      }
      
      gpio_set_function(pin, GPIO_FUNC_PWM);
      if (gpio_get_function(pin) != GPIO_FUNC_PWM) {
        Koral::out.error("Koral::hardware::pico::pwm::Pin: Can't set pin to PWM.\n");
        succ = false;
      } else {
        succ = true;
      }
    }
    ~Pin() {
      pwm_set_enabled(slice, false);
      gpio_set_function(pin, GPIO_FUNC_SIO);
      if(gpio_get_function(pin) != GPIO_FUNC_SIO) {
        Koral::out.error("Koral::hardware::pico::pwm::Pin: Can't set pin to SIO.\n");
        succ = false;
        return;
      }
      
      gpio_set_dir(pin, GPIO_IN);
      if(gpio_get_dir(pin) != GPIO_IN) {
        Koral::out.error("Koral::hardware::pico::pwm::Pin: Can't set pin to INPUT.\n");
        succ = false;
      } else {
        succ = true;
      }
    }



    bool disable() noexcept {
      pwm_set_enabled(slice, false);
      gpio_set_function(pin, GPIO_FUNC_SIO);
      if(gpio_get_function(pin) != GPIO_FUNC_SIO) {
        Koral::out.error("Koral::hardware::pico::pwm::Pin: Can't set pin to standart I/O.\n");
        return false;
      }
      gpio_set_dir(pin, GPIO_IN);
      if(gpio_get_dir(pin) != GPIO_IN) {
        Koral::out.error("Koral::hardware::pico::pwm::Pin: Can't set pin to Input.\n");
        return false;
      }
      return true;
    }
    bool enableN() noexcept {
      pwm_set_enabled(slice, true);
      gpio_set_function(pin, GPIO_FUNC_PWM);
      if(gpio_get_function(pin) != GPIO_FUNC_PWM) {
        Koral::out.error("Koral::hardware::pico::pwm::Pin: Can't set pin to PWM.\n");
        return false;
      }
      return true;
    }
    void enable() noexcept {
      pwm_set_enabled(slice, true);  
    }
    
  };
}