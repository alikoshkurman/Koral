#pragma once
#include <cmath>
#include <concepts>
#include "log.hpp"
#include <ratio>

namespace Koral::math {
    namespace SI {
        struct qm   { static constexpr double scale = 1e-28; };
        struct rm   { static constexpr double scale = 1e-25; };
        struct ym   { static constexpr double scale = 1e-22; };
        struct zm   { static constexpr double scale = 1e-19; };
        struct am   { static constexpr double scale = 1e-16; };
        struct fm   { static constexpr double scale = 1e-13; };
        struct pm   { static constexpr double scale = 1e-10; };
        struct nm   { static constexpr double scale = 1e-7;  };
        struct mcm  { static constexpr double scale = 1e-4;  };
        struct mm   { static constexpr double scale = 0.1;   };
        struct cm   { static constexpr double scale = 1.0;   };
        struct dm   { static constexpr double scale = 10.0;  };
        struct m    { static constexpr double scale = 100.0; };
        struct dam  { static constexpr double scale = 1000.0; };
        struct hm   { static constexpr double scale = 10000.0; };
        struct km   { static constexpr double scale = 100000.0; };
        struct megm { static constexpr double scale = 1e8;   };
        struct gigm { static constexpr double scale = 1e11;  };
        struct term { static constexpr double scale = 1e14;  };
        struct petm { static constexpr double scale = 1e17;  };
        struct exam { static constexpr double scale = 1e20;  };
        struct zetm { static constexpr double scale = 1e23;  };
        struct yotm { static constexpr double scale = 1e26;  };
        struct ronm { static constexpr double scale = 1e29;  };
        struct quet { static constexpr double scale = 1e32;  };
    }

    namespace Imperial {
        struct inch { static constexpr double scale = 2.54; };
        struct feet { static constexpr double scale = 30.48; };
        struct yard { static constexpr double scale = 91.44; };
        struct mile { static constexpr double scale = 160934.4; };
    }

    template<typename From, typename To>
    inline constexpr double convert(double value) {
        return value * (From::scale / To::scale);
    }
    
    namespace Pyth {
      template <typename T>
      requires std::floating_point<T> || std::integral<T>
      [[nodiscard]] constexpr double dist(T num, T num2) noexcept {
        if (num <= static_cast<T>(0) || num2 <= static_cast<T>(0)) {
          Koral::out.error("math::Pyth::dist: Bad number.\n");
          return 0.0;
        }
      
        return std::hypot(static_cast<double>(num), static_cast<double>(num2));
      }
    
    }
}