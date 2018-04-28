#include "bitboard.hpp"
#include <stdlib.h>
#include <unordered_map>
#include <cmath>
#include <strings.h>
#include <fstream>

// Takes in a bitboard and will return the bitboard representing only the least significant bit.
// Example: the initial white_nights bitboard, the least significant 1 occurs at index 1 (...00001000010)
// therefore simply return ((lots of zeros)00000000000010)
// YOU MAY ASSUME A 1 EXISTS, (0000000000000000000) will not be given
// n &= (n - 1) to find val of leftmost maybe faster
// look into: https://stackoverflow.com/questions/757059/position-of-least-significant-bit-that-is-set
#ifdef __linux__ // much faster than WIN32 
    int Engine::lsb_digit(unsigned long long board)
    {
        // return(ffsll(board)); // i think only works on linux
        return __builtin_ffsll (board) - 1;
    }
#elif _WIN32
    int Engine::lsb_digit(unsigned long long board)
    {
        return(lsb_lookup[lsb_board(board)]);
    }
#else

#endif


// Takes in a bitboard
// Returns a bitboard with soley the least significant bit = 1
// All other bits = 0
// Alters nothing
// 4ns
unsigned long long Engine::lsb_board(unsigned long long board)
{
    return(board & (-board));
}


// See above, except return the move_list significant bit bitboard
unsigned long long Engine::msb_board(unsigned long long board)
{
    unsigned long long sol = pow(2,msb_digit(board));
    return(sol);
}

// See above, except return the move_list significant bit bitboard
// Returns the index (right = 0 left = 63) corresponding to the most significant 1
// Probably a speed up to be had by checking if its above 32 bit
unsigned long long Engine::msb_digit(unsigned long long board)
{
    uint32_t first = board >> 32;
    uint32_t second = board & 0xFFFFFFFFLL;
    if(first == 0)
        return(63-(32 + __builtin_clz(second)));
    else
        return(63-__builtin_clz(first));
}

// https://stackoverflow.com/questions/25802605/what-does-performing-a-byteswap-mean
unsigned long long Engine::reverse_8_bits(unsigned long long x)
{
    return(x * 0x0202020202 & 0x010884422010) % 1023;
}


unsigned long long Engine::reverse_64_bits(unsigned long long x)
{
    return(vertical_flip(horizontal_flip(x)));
    // return (x * np.uint64(0x0202020202) & np.uint64(0x010884422010)) % np.uint64(1023);
}

unsigned long long Engine::horizontal_flip(unsigned long long x)
{
    // unsigned long long k1 = 0x5555555555555555;
    // unsigned long long k2 = 0x3333333333333333;
    // unsigned long long k4 = 0x0f0f0f0f0f0f0f0f;
    x = ((x >> 1) & 0x5555555555555555) + 2 * (x & 0x5555555555555555);
    x = ((x >> 2) & 0x3333333333333333) + 4 * (x & 0x3333333333333333);
    x = ((x >> 4) & 0x0f0f0f0f0f0f0f0f) + 16 * (x & 0x0f0f0f0f0f0f0f0f);
    return(x);
}

// 4ns
unsigned long long Engine::vertical_flip(unsigned long long x)
{
    return __builtin_bswap64(x); // maybe only linux? check
    return (x >> 56) |
          ((x<<40) & 0x00FF000000000000) |
          ((x<<24) & 0x0000FF0000000000) |
          ((x<<8) & 0x000000FF00000000) |
          ((x>>8) & 0x00000000FF000000) |
          ((x>>24) & 0x0000000000FF0000) |
          ((x>>40) & 0x000000000000FF00) |
          (x << 56);
}

//check implementation
// 315 ns per (due to lsb_digit)
int Engine::bitboard_to_square(unsigned long long piece)
{
    return(lsb_digit(piece));
}