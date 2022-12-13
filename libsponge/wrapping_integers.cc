#include "wrapping_integers.hh"
// Dummy implementation of a 32-bit wrapping integer

// For Lab 2, please replace with a real implementation that passes the
// automated checks run by `make check_lab2`.

using namespace std;

#define BASE 0xffffffff 
//! Transform an "absolute" 64-bit sequence number (zero-indexed) into a WrappingInt32
//! \param n The input absolute 64-bit sequence number
//! \param isn The initial sequence number
WrappingInt32 wrap(uint64_t n, WrappingInt32 isn) {
    uint32_t value = (n + isn.raw_value()) & BASE;
    return WrappingInt32{value};
}

//! Transform a WrappingInt32 into an "absolute" 64-bit sequence number (zero-indexed)
//! \param n The relative sequence number
//! \param isn The initial sequence number
//! \param checkpoint A recent absolute 64-bit sequence number
//! \returns the 64-bit sequence number that wraps to `n` and is closest to `checkpoint`
//!
//! \note Each of the two streams of the TCP connection has its own ISN. One stream
//! runs from the local TCPSender to the remote TCPReceiver and has one ISN,
//! and the other stream runs from the remote TCPSender to the local TCPReceiver and
//! has a different ISN.
uint64_t abs(uint64_t a, uint64_t b) {
    return a > b ? a - b : b - a;
}
uint64_t unwrap(WrappingInt32 n, WrappingInt32 isn, uint64_t checkpoint) {
    uint32_t part_n = n.raw_value() - isn.raw_value(); // automate overflow
    uint64_t choice_A = (checkpoint >> 32) << 32 | part_n;
    if (choice_A < checkpoint) {
        uint64_t choice_B = choice_A + BASE + 1;
        return abs(checkpoint, choice_A) < abs(choice_B, checkpoint) ? choice_A : choice_B;
    } else {
        uint64_t choice_B = choice_A - BASE - 1;
        return abs(choice_A, checkpoint) < abs(checkpoint, choice_B) ? choice_A : choice_B;
    }
}
