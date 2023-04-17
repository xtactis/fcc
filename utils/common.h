#ifndef COMMON_H
#define COMMON_H

#include "printing.h"
#include "types.h"
#include "dyn_array.h"
#include "hash_map.h"
#include "string.h"
#include "bitset.h"

#ifndef _MSC_VER
u64 max(u64 a, u64 b);
u64 min(u64 a, u64 b);
#endif

_generate_declarations(u8);
_generate_declarations(u16);
_generate_declarations(u32);
_generate_declarations(u64);
_generate_declarations(s8);
_generate_declarations(s16);
_generate_declarations(s32);
_generate_declarations(s64);
_generate_declarations(b8);
_generate_declarations(b16);
_generate_declarations(b32);
_generate_declarations(b64);
_generate_declarations(bool);
_generate_declarations(char);
_generate_declarations(int);
_generate_declarations(f32);
_generate_declarations(f64);

#endif // COMMON_H
