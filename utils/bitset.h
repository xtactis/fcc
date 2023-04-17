#ifndef BITSET_H
#define BITSET_H

#include "common.h"

// NOTE(mdizdar): who's gonna need more than 8*64 pointers for real tho
static const u64 BITSET_SIZE = 8LL;
typedef u64 Bitset[8]; // cl is stupid

// NOTE(mdizdar): for all of these we're hoping the compiler will notice the division and modulo operations can be rewritten as bitshifts
inline bool Bitset_isSet(Bitset bitset, u64 bit) {
    return !!(bitset[bit/sizeof(*bitset)] & (1LL << (bit % sizeof(*bitset))));
}

inline void Bitset_set(Bitset bitset, u64 bit) {
    bitset[bit/sizeof(*bitset)] |= (1LL << (bit % sizeof(*bitset)));
}

inline void Bitset_clear(Bitset bitset, u64 bit) {
    bitset[bit/sizeof(*bitset)] &= ~(1LL << (bit % sizeof(*bitset)));
}

#endif //BITSET_H
