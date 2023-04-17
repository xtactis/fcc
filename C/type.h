/* date = January 1st 2021 11:28 pm */

#ifndef TYPE_H
#define TYPE_H

#include "../utils/common.h"
#include "arena.h"

#include "struct_type.h"
#include "union_type.h"
#include "array_type.h"
#include "function_type.h"

typedef enum {
    BASIC_ERROR = 0,
    BASIC_VOID  = 1,
    BASIC_CHAR  = 2,
    
    BASIC_SCHAR  = 3,
    BASIC_SSHORT = 4,
    BASIC_SINT   = 5,
    BASIC_SLONG  = 6,
    BASIC_SLLONG = 7,
    
    BASIC_FLOAT   = 8,
    BASIC_DOUBLE  = 9,
    BASIC_LDOUBLE = 10,
    
    BASIC_UCHAR  = 3+32,
    BASIC_USHORT = 4+32,
    BASIC_UINT   = 5+32,
    BASIC_ULONG  = 6+32,
    BASIC_ULLONG = 7+32,
} BasicType;

STRUCT_HEADER(Type, {
    union {
        StructType *struct_type;
        UnionType *union_type;
        Type *typedef_type;
        ArrayType *array_type;
        FunctionType *function_type;
        BasicType basic_type;
    };
    
    Bitset is_const;
    Bitset is_volatile;
    Bitset is_restrict;
    
    u64 pointer_count;
    
    int is_static:1;
    int is_struct:1;
    int is_union:1;
    int is_typedef:1;
    int is_array:1;
    int is_function:1;
});

u64 Type_sizeof(Type *type);
char *Type_toStr(char *s, const Type *type, bool in_line, u64 indent);
void Type_print(Type *type, u64 indent);

#endif //TYPE_H
