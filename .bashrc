cppt() {
    g++ -std=c++23 -Wall -Wextra -Wpedantic -Wfloat-conversion -Wconversion -fsanitize=address -fsanitize=undefined -g -l -O1 -O2 -O3 -o "${1%.*}" "$1" && ./"${1%.*}"
}
cppf() {
    g++ -std=c++23 -l -s -O3 -o "${1%.*}" "$1" && ./"${1%.*}"
    upx -qq --best --ultra-brute --lzma "${1%.*}"
}
