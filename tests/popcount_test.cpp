#include "../cpp_chess_engine/BitOps.hpp"
#include <cassert>
#include <iostream>

int main() {
    Bitboard v1 = 0ULL;
    Bitboard v2 = 0xF0F0F0F0F0F0F0F0ULL; // 32 bits set
    Bitboard v3 = 0b1011ULL; // 3 bits set

    if (popcount(v1) != 0) {
        std::cerr << "popcount failed on zero" << std::endl;
        return 1;
    }
    if (popcount(v2) != 32) {
        std::cerr << "popcount failed on pattern" << std::endl;
        return 1;
    }
    if (popcount(v3) != 3) {
        std::cerr << "popcount failed on three" << std::endl;
        return 1;
    }

#if defined(_MSC_VER)
    std::cout << "Running MSVC popcount implementation" << std::endl;
#else
    std::cout << "Running generic/GNU popcount implementation" << std::endl;
#endif

    return 0;
}
