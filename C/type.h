/* date = January 1st 2021 11:28 pm */

#ifndef TYPE_H
#define TYPE_H

#include "arena.h"

// NOTE(mdizdar): who's gonna need more than 8*64 pointers for real tho
static const u64 BITSET_SIZE = 8LL;
typedef u64 Bitset[8]; // cl is stupid

// NOTE(mdizdar): for all of these we're hoping the compiler will notice the division and modulo operations can be rewritten as bitshifts
static inline bool Bitset_isSet(Bitset bitset, u64 bit) {
    return !!(bitset[bit/sizeof(*bitset)] & (1LL << (bit % sizeof(*bitset))));
}

static inline void Bitset_set(Bitset bitset, u64 bit) {
    bitset[bit/sizeof(*bitset)] |= (1LL << (bit % sizeof(*bitset)));
}

static inline void Bitset_clear(Bitset bitset, u64 bit) {
    bitset[bit/sizeof(*bitset)] &= ~(1LL << (bit % sizeof(*bitset)));
}

typedef struct {
    void *data;
    u64 element_size;
    u64 count;
    u64 capacity;
} DynArray;

DynArray *DynArray_add(DynArray *array, void *element) {
    if (!array->capacity) {
        array->data = malloc(array->element_size * 2);
        memcpy(array->data, element, array->element_size);
        array->count = 1;
        array->capacity = 2;
        return array;
    }
    if (array->capacity == array->count) {
        array->capacity += array->capacity;
        array->data = realloc(array->data, array->capacity * array->element_size);
    }
    memcpy((u8 *)array->data + array->count*array->element_size, element, array->element_size);
    ++array->count;
    return array;
}

// NOTE(mdizdar): this doesn't work for some reason, figure it out bitch
void *DynArray_at(DynArray *array, u64 index) {
    assert(index < array->count);
    return (void *)((u8 *)array->data + index*array->element_size);
}

static inline void DynArray_construct(DynArray *array, u64 element_size) {
    array->element_size = element_size;
    array->capacity = 0;
    array->count = 0;
}

struct _Type;
struct u64_Type;
struct bool_Type;
typedef struct _Type Type;

typedef struct {
    Type *type; // NOTE(mdizdar): at one point we'll have common types multiple variables can point to, so we use a pointer
    String name;
} Declaration;

typedef struct {
    DynArray members;
    //SymbolTable member_table;
} StructType;

typedef struct {
    DynArray members;
} UnionType;

typedef struct {
    Type *element;
    u64 size;
} ArrayType;

typedef struct {
    u64 size_of;
    Type *return_type;
    DynArray parameters;
    Node *block;
} FunctionType;

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

struct _Type {
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
};

struct u64_Type {
    union {
        StructType *struct_type;
        UnionType *union_type;
        Type *typedef_type;
        ArrayType *array_type;
        BasicType basic_type;
    };
    Bitset is_const; // bitset
    Bitset is_volatile; // bitset
    Bitset is_restrict; // bitset
    
    u64 pointer_count;
    
    u64 is_static;
    u64 is_struct;
    u64 is_union;
    u64 is_typedef;
    u64 is_array;
    u64 is_function;
};

struct bool_Type {
    union {
        StructType *struct_type;
        UnionType *union_type;
        Type *typedef_type;
        ArrayType *array_type;
        BasicType basic_type;
    };
    Bitset is_const; // bitset
    Bitset is_volatile; // bitset
    Bitset is_restrict; // bitset
    
    u64 pointer_count;
    
    bool is_static;
    bool is_struct;
    bool is_union;
    bool is_typedef;
    bool is_array;
    bool is_function;
};

void Type_print(Type *type, u64 indent) {
    for (u64 i = 0; i < indent; ++i) putchar(' ');
    if (type->is_static) printf("static ");
    if (Bitset_isSet(type->is_const, 0)) printf("const ");
    if (Bitset_isSet(type->is_volatile, 0)) printf("volatile ");
    if (Bitset_isSet(type->is_restrict, 0)) printf("restrict ");
    if (type->is_struct) {
        printf("struct { [%lu] \n", type->struct_type->members.count);
        for (u64 i = 0; i < type->struct_type->members.count; ++i) {
            Type_print(((Declaration*)type->struct_type->members.data)[i].type, indent+2);
            
        }
        for (u64 i = 0; i < indent; ++i) putchar(' ');
        printf("}");
    } else if (type->is_union) {
        // TODO(mdizdar)
    } else if (type->is_typedef) {
        // TODO(mdizdar)
    } else if (type->is_array) {
        // TODO(mdizdar)
    } else if (type->is_function) {
        // TODO(mdizdar)
    } else {
        switch(type->basic_type) {
            case BASIC_ERROR: printf("ERROR type "); break;
            case BASIC_VOID:  printf("void "); break;
            case BASIC_CHAR:  printf("char "); break;
            
            case BASIC_SCHAR:  printf("signed char "); break;
            case BASIC_SSHORT: printf("short int "); break;
            case BASIC_SINT:   printf("int "); break;
            case BASIC_SLONG:  printf("long int "); break;
            case BASIC_SLLONG: printf("long long int "); break;
            
            case BASIC_FLOAT:   printf("float "); break;
            case BASIC_DOUBLE:  printf("double "); break;
            case BASIC_LDOUBLE: printf("long double "); break;
            
            case BASIC_UCHAR:  printf("unsigned char "); break;
            case BASIC_USHORT: printf("unsigned short int "); break;
            case BASIC_UINT:   printf("unsigned int "); break;
            case BASIC_ULONG:  printf("unsigned long int "); break;
            case BASIC_ULLONG: printf("unsigned long long int "); break;
        }
    }
    
    for (u64 i = 1; i <= type->pointer_count; ++i) {
        printf("* ");
        if (Bitset_isSet(type->is_const, i)) printf("const ");
        if (Bitset_isSet(type->is_volatile, i)) printf("volatile ");
        if (Bitset_isSet(type->is_restrict, i)) printf("restrict ");
    }
    puts("");
}

//~ some globals to keep the cost of type-making lower

// TODO(mdizdar): these should be set in main (because MSVC doesn't have struct literals, since it's 1993) so these common types can be reused
Type *TYPE_VOID;
Type *TYPE_CHAR;

Type *TYPE_SCHAR;
Type *TYPE_SSHORT;
Type *TYPE_SINT;
Type *TYPE_SLONG;
Type *TYPE_SLLONG;

Type *TYPE_FLOAT;
Type *TYPE_DOUBLE;
Type *TYPE_LDOUBLE;

Type *TYPE_UCHAR;
Type *TYPE_USHORT;
Type *TYPE_UINT;
Type *TYPE_ULONG;
Type *TYPE_ULLONG;

#endif //TYPE_H
