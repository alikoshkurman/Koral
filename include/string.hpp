#include <string>
#include <string_view>
#include <algorithm> 

namespace Koral {
template <char Ch, size_t N>
struct CompileTimeString {
  char data[N + 1]{};
  constexpr CompileTimeString() {
    for (size_t i = 0; i < N; ++i) data[i] = Ch;
    data[N] = '\0';
  }
};

struct to_body {
std::string lower(std::string& str) {
    std::transform(str.begin(), str.end(), str.begin(), [](unsigned char c) {
        return std::tolower(c);
    });
    return str;
}
std::string upper(std::string& str) {
    std::transform(str.begin(), str.end(), str.begin(), [](unsigned char c) {
        return std::toupper(c);
    });
    return str;
}
}; 
using str= std::string;
using strv = std::string_view;
using cstr = const char*;
inline to_body to;
}
