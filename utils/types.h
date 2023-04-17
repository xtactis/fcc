/* date = November 27th 2020 10:29 pm */

#ifndef TYPES_H
#define TYPES_H

#include <stdint.h>
#include <stddef.h>

#define false 0
#define true 1

typedef uint_fast8_t bool;
typedef uint8_t      b8;
typedef uint16_t     b16;
typedef uint32_t     b32;
typedef uint64_t     b64;
                     
typedef int8_t   s8;
typedef int16_t  s16;
typedef int32_t  s32;
typedef int64_t  s64;
                     
typedef uint8_t  u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef float  f32;
typedef double f64;
_Static_assert(sizeof(f32) == 4, "Machines not compliant with IEEE 754/IEC 559 are not supported. `float` should be 4 bytes (32 bits) wide.");
_Static_assert(sizeof(f64) == 8, "Machines not compliant with IEEE 754/IEC 559 are not supported. `double` should be 8 bytes (64 bits) wide.");

u64 u64_hash(const u64 *_key) {
    u64 key = *_key;
    key = (~key) + (key << 21); // key = (key << 21) - key - 1;
    key = key ^ (key >> 24);
    key = (key + (key << 3)) + (key << 8); // key * 265
    key = key ^ (key >> 14);
    key = (key + (key << 2)) + (key << 4); // key * 21
    key = key ^ (key >> 28);
    key = key + (key << 31);
    return key;
}

#define _generate_declarations(type) \
    typedef struct type##Array type##Array, *type##ArrayPtr; \
    typedef struct type##ArrayArray type##ArrayArray, *type##ArrayArrayPtr; \
    typedef struct type##ArrayArray type##Matrix, *type##MatrixPtr; \
    _generate_dynamic_array_header(type); \
    _generate_dynamic_array_header(type##Array);

#define _generate_definitions(type) \
    _generate_dynamic_array_source(type); \
    _generate_dynamic_array_source(type##Array);

#define _generate_type(type) \
    _generate_declarations(type); \
    _generate_definitions(type);

#define STRUCT_HEADER(name, body) \
    typedef struct name body name, *name##Ptr; \
    _generate_declarations(name); \
    _generate_declarations(name##Ptr);

#define STRUCT_SOURCE(name) \
    _generate_definitions(name); \
    _generate_definitions(name##Ptr); 

#define STRUCT(name, body) \
    STRUCT_HEADER(name, body); \
    STRUCT_SOURCE(name);

#endif //TYPES_H
