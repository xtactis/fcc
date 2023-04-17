#include "common.h"

#ifndef _MSC_VER

u64 max(u64 a, u64 b) {
    return a > b ? a : b;
}

u64 min(u64 a, u64 b) {
    return a < b ? a : b;
}

#endif

_generate_definitions(u8);
_generate_definitions(u16);
_generate_definitions(u32);
_generate_definitions(u64);
_generate_definitions(s8);
_generate_definitions(s16);
_generate_definitions(s32);
_generate_definitions(s64);
_generate_definitions(b8);
_generate_definitions(b16);
_generate_definitions(b32);
_generate_definitions(b64);
_generate_definitions(bool);
_generate_definitions(char);
_generate_definitions(int);
_generate_definitions(f32);
_generate_definitions(f64);
